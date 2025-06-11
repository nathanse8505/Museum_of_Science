#
# telegram_bot.py - MicroPython library for Telegram Bot API.
# Supports conversation, keyboards and surrogate emojis...
# https://core.telegram.org/bots/api
#
# https://github.com/arduino12/micropython-libs 2024/07/27
#

__version__ = '1.0.1'

from re import match
from requests import post
from json import dumps


def fix_surrogate(s): # https://en.wikipedia.org/wiki/UTF-16
    i = 0
    while i < len(s):
        if 0xD800 <= ord(s[i]) <= 0xDFFF:
            c = ((ord(s[i]) - 0xD800) * 0x400) + (ord(s[i + 1]) - 0xDC00) + 0x10000
            s = s.replace(s[i: i + 2], chr(c))
        i += 1
    return s


class Bot():
    _HTTP_JSON = {'content-type': 'application/json'}

    def __init__(self, token):
        url = 'https://api.telegram.org/bot{}/'.format(token)
        self._api_receive = '{}getUpdates'.format(url)
        self._api_send = '{}sendMessage'.format(url)
        self._api_edit = '{}editMessageText'.format(url)
        self._last_update = 0
        self._receive_parameters = {
            'offset': 1,
            'timeout': 2,
            'allowed_updates': ['messages']
        }
        self._command_handlers = {}
        self._callback_handlers = {}
        self._message_handlers = {}
        self._get_updates()

    def _get_updates(self):
        self._receive_parameters['offset'] = self._last_update + 1
        try:
            response = post(self._api_receive, json=self._receive_parameters)
            data = response.json()
            response.close()
            if data['result']:
                self._last_update = data['result'][-1]['update_id']
                return [Update(self, update) for update in data['result']]
        except Exception as e:
            raise OSError('_get_updates: ', e)

    def _handle_update(self, update):
        if update.is_callback:
            self._callback_handlers[update.callback_data](update)
        if 'text' not in update.message:
            return
        update.message['text'] = fix_surrogate(update.message['text'])
        text = update.message['text']
        if text.startswith('/'):
            command = text.split(' ')[0].replace('/','')
            if command in set(self._command_handlers.keys()):
                self._command_handlers[command](update)
                return
        else:
            for expression in set(self._message_handlers.keys()):
                if match(expression, text):
                    self._message_handlers[expression](update)
                    return

    def update(self):
        updates = self._get_updates()
        if updates:
            for update in updates:
                self._handle_update(update)

    def add_message_handler(self, regular_expression):
        def decorator(function):
            self._message_handlers[regular_expression] = function
        return decorator

    def add_callback_handler(self, callback_data):
        def decorator(function):
            self._callback_handlers[callback_data] = function
        return decorator

    def add_command_handler(self, command):
        def decorator(function):
            self._command_handlers[command] = function
        return decorator

    def _message(self, chat_id, text, reply_markup=None, message_id=None):
        parameters = {
            'chat_id': chat_id,
            'text': text.replace('.', '\\.')
        }
        if message_id is not None:
            parameters['message_id'] = message_id
        if reply_markup is not None:
            parameters['reply_markup'] = reply_markup.data
        data = dumps(parameters).encode()
        try:
            message = post(self._api_send, data=data, headers=Bot._HTTP_JSON)
            assert message
            message.close()
        except Exception:
            raise OSError('message not sent')

    def send_message(self, chat_id, text, reply_markup=None):
        self._message(chat_id, text, reply_markup)

    def update_message(self, chat_id, message_id, text, reply_markup=None):
        self._message(chat_id, text, reply_markup, message_id)


class ReplyKeyboardMarkup():
    def __init__(self, keyboard, resize_keyboard=False, one_time_keyboard=False, selective=False):
        self.data = {
            'keyboard': [[k.data for k in row] for row in keyboard],
            'resize_keyboard': resize_keyboard,
            'one_time_keyboard': one_time_keyboard,
            'selective': selective,
        }


class InlineKeyboardMarkup():
    def __init__(self, keyboard):
        self.data = {
            'inline_keyboard': [[k.data for k in row] for row in keyboard]
        }


class KeyboardButton():
    def __init__(self, text):
        self.data = {
            'text': text,
        }


class InlineKeyboardButton():
    def __init__(self, text,url='', callback_data=''):
        self.data = {
            'text': text,
            'url': url,
            'callback_data': callback_data
        }


class Update():
    def __init__(self, b, update):
        self.bot = b
        self.update_id = update['update_id']
        self.is_callback = False
        self.callback_data = ''
        try:
            if update['callback_query']:
                self.is_callback = True
                self.message = update['callback_query']['message']
                self.callback_data = update['callback_query']['data']
        except KeyError as e:
            self.message = update['message']

    def reply(self, text, reply_markup=None):
        self.bot.send_message(self.message['chat']['id'], text, reply_markup)

    def edit(self, text, reply_markup=None):
        self.bot.update_message(self.message['chat']['id'], self.message['message_id'], text, reply_markup)
