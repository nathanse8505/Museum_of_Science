# Photochromic Manipulator
A self assembly kit that draw on a photochromic paper!  
Designed and programed with ❤ by [Arad.E](https://github.com/arduino12/) and [Jerusalem Science Museum](https://mada.org.il/) team.

## Technologies
* **Photochemistry:**  
The photochromic pigment in the paper darken its color for a while after being exposed to ultraviolet (UV) light.
* **Kinematics:**  
The LED moves on the paper using a mechanism called [Five-Bar-Planar-Parallel Manipulator](https://en.wikipedia.org/wiki/Five-bar_linkage).
* **Electro-mechanics:**  
The [servo motors](https://gabbyshimoni.wixsite.com/arduino-programming/blank-18) convert electric current into rotary motion.
* **Electro-optics:**  
The UV-LED converts an electric current into UV light.

## Getting started
### Hardware
1. Assemble the kit by following the [**assembly guide**](https://drive.google.com/file/d/1ZZT2MTnF2FmKq9z3wkU-5HSkHxZSyMiZ).
2. Learn the basics from the [**presentation**](https://docs.google.com/presentation/d/10CCbnRxoIO8JoA44LrwucI05HlEwSopz_qcIy2chwl0) and visit some of it's links.
3. View [**S2-Mini schematics**](https://www.wemos.cc/en/latest/_static/files/sch_s2_mini_v1.0.0.pdf) and [**pm_v3**](https://drive.google.com/file/d/1Le-dcOo2rCa9OkG9LgLotUU8tVFwa3I2/view) schematics.
### Software and Firmware
## [**!! See a video of all the following steps !!**](https://drive.google.com/file/d/1zq4tPx_3L1xqlrCyhS9WZJXLbCVjM0xy/view)
1. Install a MicroPython IDE, lets go with [**Thonny IDE**](https://thonny.org/). (here are [other options](https://randomnerdtutorials.com/micropython-ides-esp32-esp8266/) FYI).
2. Open Thonny.exe (press WinKey and type `Thonny`).  
3. Flash the ESP32-S2 Mini MicroPython firmware v1.23.0 to the microcontroller using Thonny (or [manually](https://micropython.org/download/ESP32_GENERIC_S2/)).  
  **3.1** **Press the small boot button (marked "0" on the PCB)** while connecting the microcontroller to your computer via a USB-C cable.  
  **3.2** Press toolbar's `Run` -> `Configure interpreter`.  
  **3.3** Select `MicroPython (ESP32)` on the second line.  
  **3.4** Press the bottom `Install or update MicroPython (esptool)`.  
  **3.5** Select `MicroPython family` = `ESP32-S2`.  
  **3.6** Select `variant` = `Wemos S2 mini`.  
  **3.7** Press the `Install` button and wait for "Done" message!  
  **3.8** **Press the small reset button (marked "RST" on the PCB).**  
  **3.9** Close the sub dialog (press ESC) and press `OK` on the main dialog (press ENTER).  
  **The microcontroller's MicroPython REPL prompt should appear in Thonny's terminal!**
4. Download this [repository](https://github.com/arduino12/photochromic-manipulator/archive/refs/heads/main.zip) and extract it (delete the compressed zip file).
5. Upload the content of micropython folder to the microcontroller using Thonny.  
   **5.1** Press View (top menu-bar) and select `Files`, do the same with `Object inspector`.  
   **5.2** Navigate to the path of the unzipped `photochromic-manipulator/micropython`.  
   **5.3** Select all files -> right-click -> `Upload to /` and make sure all were copied to the device.  
6. Run an example by writing `from examples.basic import test_leds` and press Enter,  
can also run by opening the file and pressing F5.
### Creating a Telegram bot
1. Install the [**Android Telegram App**](https://play.google.com/store/apps/details?id=org.telegram.messenger) or the [**iOS Telegram App**](https://apps.apple.com/us/app/telegram-messenger/id686449807).
2. Register to Telegram...
3. Open a chat with [**Bot Father**](https://telegram.me/BotFather) and create a new bot.
4. Copy the bot token to [**secrets.py**](micropython/secrets.py).
5. Set the `WIFI_SSID` and `WIFI_PASS` parameters in [**secrets.py**](micropython/secrets.py) to your local WiFi network.
6. Run the Telegram Bot client by running the following command in the Micro-Python shell:  
   `from examples.advanced import telegram_controlled`
7. Send a `/start` text message to the bot to start the conversation.
