from time import sleep_ms


def do_connect(ssid, password=None):
    import machine, network
    #wlan = network.WLAN()
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect(ssid, password)
        while not wlan.isconnected():
            print('.', end='')
            sleep_ms(100)
            machine.idle()
    print('network config:', wlan.ipconfig('addr4'))


do_connect('Mada_Guest')
