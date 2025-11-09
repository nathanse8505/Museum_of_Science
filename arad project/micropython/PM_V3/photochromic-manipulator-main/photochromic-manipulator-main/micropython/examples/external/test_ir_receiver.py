from sys import modules
from machine import Pin
from time import sleep
from gc import collect
try:
    from ir_rx.print_error import print_error
    from ir_rx.nec import NEC_8, NEC_16, SAMSUNG
    from ir_rx.sony import SONY_12, SONY_15, SONY_20
    from ir_rx.philips import RC5_IR, RC6_M0
    from ir_rx.mce import MCE
except ImportError:
    print('''
Failed to import /lib/ir_rx!
Please download from:
https://github.com/peterhinch/micropython_ir''')
    

def ir_received(data, addr, ctrl):
    if data < 0:
        print('NEC repeat...')
    else:
        print(f'\nData 0x{data:02x} Addr 0x{addr:04x} Ctrl 0x{ctrl:02x}')


def ir_receiver_loop():
    try:
        protocol = int(input('Enter protocol index: '))
        PROTOCOLS = (NEC_8, NEC_16, SONY_12, SONY_15, SONY_20, RC5_IR, RC6_M0, MCE, SAMSUNG)
        ir_receiver = PROTOCOLS[protocol](Pin(14), ir_received)
    except KeyboardInterrupt:
        return
    ir_receiver.error_function(print_error) # debug prints

    try:
        while True:
            print('.', end='')
            sleep(1)
            collect() # free unneeded RAM allocations (takes 10-100ms)
    except KeyboardInterrupt:
        ir_receiver.close()


print('''
Test IR Receiver:
0: for NEC 8 bit protocol.
1: for NEC 16 bit.
2: for Sony SIRC 12 bit.
3: for Sony SIRC 15 bit.
4: for Sony SIRC 20 bit.
5: for Philips RC-5 protocol.
6: for RC6 mode 0.
7: for Microsoft Vista MCE.
8: for Samsung.

Press ctrl-c to exit test.
''')
ir_receiver_loop()
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press ctrl-d to soft-reset.
''')
