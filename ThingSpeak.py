import sys
from lxml import html
import re
import time
import requests
import datetime
import json


WEB_BASE_URL_THINGSPEAK = 'https://api.thingspeak.com'
WEB_PATH_THINGSPEAK = '/channels/810599/feeds.json?api_key='
API_KEY_THINGSPEAK = '5OGLD1VGIM49ELGP'


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


if __name__ == '__main__':
    print("----------------")
    print("Iniciando script")
    print("----------------")
    timeNow = datetime.datetime.now()
    print("Baixando dados: " + str(timeNow))
    get_api_data()

