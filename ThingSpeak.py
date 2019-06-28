import sys
from lxml import html
import re
import time
import requests
import datetime
import json


# THINGSPEAK
WEB_BASE_URL_THINGSPEAK = 'https://api.thingspeak.com'
WEB_PATH_THINGSPEAK = '/channels/810599/feeds.json?api_key='
API_KEY_THINGSPEAK = '5OGLD1VGIM49ELGP'

# TELEGRAM
API_KEY_TELEGRAM = '894167827:AAGm0LArM6E9QlMwpkWxC-JXbNSHP2nNtg4'
BASE_URL_TELEGRAM = 'https://api.telegram.org/bot'
SEND_MESSAGE = "/sendMessage?text="
CHAT_ID = '&chat_id='
CHAT_ID_RODRIGO = '698900494'


def telegram_warn(message):
    print("Avisando telegram")
    url = BASE_URL_TELEGRAM + API_KEY_TELEGRAM + SEND_MESSAGE + message + CHAT_ID + CHAT_ID_RODRIGO
    session = requests.Session()
    response = session.get(url)
    print("Telegram response code: " + str(response.status_code))
    print("\n\n")


def get_api_data():
    s = requests.Session()
    s.headers.update({
        'Content-Type': 'application/json'
    })
    url = WEB_BASE_URL_THINGSPEAK + WEB_PATH_THINGSPEAK + API_KEY_THINGSPEAK
    request = s.get(url)
    response = json.loads(request.text)

    print("ThingSpeak request status code: " + str(request.status_code) + "\n" "\n" "\n")

    feeds = response['feeds']
    for feed in feeds:
        print(feed)
        telegram_warn(str(feed))


if __name__ == '__main__':
    print("----------------")
    print("Iniciando script")
    print("----------------")
    timeNow = datetime.datetime.now()
    print("Baixando dados: " + str(timeNow))
    get_api_data()

