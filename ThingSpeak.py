import sys
from lxml import html
import re
import time
import requests
import datetime
import json
import sendgrid
from sendgrid.helpers.mail import Mail, From, To, Subject, PlainTextContent, HtmlContent, SendGridException

# THINGSPEAK
WEB_BASE_URL_THINGSPEAK = 'https://api.thingspeak.com'
WEB_PATH_THINGSPEAK = '/channels/810599/feeds.json?api_key='
API_KEY_THINGSPEAK = '5OGLD1VGIM49ELGP'
WEB_PATH_RESULTS_THINGSPEAK = '&results=1'

# TELEGRAM
API_KEY_TELEGRAM = '894167827:AAGm0LArM6E9QlMwpkWxC-JXbNSHP2nNtg4'
BASE_URL_TELEGRAM = 'https://api.telegram.org/bot'
SEND_MESSAGE = "/sendMessage?text="
CHAT_ID = '&chat_id='
CHAT_ID_RODRIGO = '698900494'

# SENDGRID
API_KEY_SENDGRID = "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"


def send_email(email_address, email_subject, email_content, from_email):
    print("Enviando email: " + str(email_subject))
    sg = sendgrid.SendGridAPIClient(API_KEY_SENDGRID)
    content = PlainTextContent(email_content)
    mail = Mail(from_email, email_address, email_subject, content)
    response = sg.send(mail)
    print("SendGrid email aviso response status code: " + str(response.status_code) + "\n\n\n")


def build_email(message):
    print("Iniciando envio de email para aviso")
    email_subject = Subject("SmartScale")
    email_address = To("rodrigo.calbar@gmail.com")
    from_email = From("rodrigo.calderano@gmail.com")
    email_content = "Olá, esta é uma mensagem automática da sua SmartScale\n\n\n" + message
    send_email(email_address, email_subject, email_content, from_email)


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
    url = WEB_BASE_URL_THINGSPEAK + WEB_PATH_THINGSPEAK + API_KEY_THINGSPEAK + WEB_PATH_RESULTS_THINGSPEAK
    request = s.get(url)
    response = json.loads(request.text)

    print("ThingSpeak request status code: " + str(request.status_code) + "\n" "\n" "\n")

    feeds = response['feeds']
    for feed in feeds:
        print(feed)
        telegram_warn(str(feed))
        build_email(str(feed))


if __name__ == '__main__':
    print("----------------")
    print("Iniciando script")
    print("----------------")
    timeNow = datetime.datetime.now()
    print("Baixando dados: " + str(timeNow))
    get_api_data()

