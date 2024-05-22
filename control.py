import socket
import time

import pywifi
from pywifi import const


def start(ssid, password):
    '''Auto Connet Wifi'''
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[0]
    iface.disconnect()

    time.sleep(1)

    profile = pywifi.Profile()
    profile.ssid = ssid
    profile.auth = const.AUTH_ALG_OPEN
    profile.akm.append(const.AKM_TYPE_WPA2PSK)
    profile.cipher = const.CIPHER_TYPE_CCMP
    profile.key = password

    iface.remove_all_network_profiles()
    tmp_profile = iface.add_network_profile(profile)

    iface.connect(tmp_profile)
    time.sleep(5)

    if iface.status() == const.IFACE_CONNECTED:
        return 'Connect Success!'
    else:
        return 'Connect Failed!'


def conn_TCP(host: str, port: str, send: str) -> str:
    '''Connect UI and Diving Machine
    Args:
        host: Esp32 host
        port: Esp32 port
        send: control command
    Returns:
        serverMessage: Esp32 return success or failed
    '''
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        port = int(port)
        # timeout
        #client.settimeout(10)
        client.connect((host, port))
        clientMessage = send
        print(f'Send:{clientMessage} to Server.')
        client.sendall(clientMessage.encode())

        serverMessage = str(client.recv(1024), encoding='utf-8')
        print('Server:', serverMessage)
        print('End')
        client.close()
    
    except TimeoutError as toe:
        print(toe)
        serverMessage = 'Connect timeout.'
    finally:
        return serverMessage


if __name__ == '__main__':
    ssid = "ESP32_WiFi"
    psw = "3B017126"
    wifi_result = start(ssid, psw)
    print(f'Wifi connect status: {wifi_result}')
    host = '192.168.4.1'
    port = 80
    send = input('Message:')
    print('-' * 10, 'Execute', '-' * 10)
    conn_TCP(host, port, send)

#   NULL,40,60,10,5,35,25
#   simple,60