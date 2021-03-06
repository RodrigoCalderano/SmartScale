import sys
from lxml import html
import re
import time
import requests
import datetime
import json
import sendgrid
from sendgrid.helpers.mail import Mail, From, To, Subject, PlainTextContent, HtmlContent, SendGridException
import Constants

# SENDGRID
API_KEY_SENDGRID = Constants.API_KEY_SENDGRID

# TELEGRAM
API_KEY_TELEGRAM = Constants.API_KEY_TELEGRAM
BASE_URL_TELEGRAM = 'https://api.telegram.org/bot'
SEND_MESSAGE = "/sendMessage?text="
CHAT_ID = '&chat_id='
CHAT_ID_RODRIGO = '698900494'

# THINGSPEAK
API_KEY_THINGSPEAK = Constants.API_KEY_THINGSPEAK
WEB_BASE_URL_THINGSPEAK = 'https://api.thingspeak.com'
WEB_PATH_THINGSPEAK = '/channels/810599/feeds.json?api_key='
WEB_PATH_RESULTS_THINGSPEAK = '&results=2'


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
    entry_file = open("LastEntry.txt", "+r")
    last_entry = entry_file.readline()
    entry_file.close()

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

        if last_entry >= str(feed['entry_id']):
            print("Nenhuma informação nova")
            continue

        entry_file = open("LastEntry.txt", "w")
        entry_file.write(str(feed['entry_id']))
        entry_file.close()

        peso = int(feed['field1'])
        gas_sensor = float(feed['field2'])
        if peso < 15:
            msg = "O meu querido, parece que está quase acabando o gás do seu botijão. O peso dele no momento é: " + str(peso)
            telegram_warn(msg)
            build_email(msg)
        if gas_sensor > 300:
            msg = "O meu querido, parece que está vazando gás. O sensor dele está indicando: " + str(gas_sensor)
            telegram_warn(msg)
            build_email(msg)


if __name__ == '__main__':
    print("----------------")
    print("Iniciando script")
    print("----------------")
    timeNow = datetime.datetime.now()
    print("Baixando dados: " + str(timeNow))

    # segundos * minutos
    waitTime = 5 * 1
    while True:
        print("Baixando dados")
        get_api_data()
        print("Esperando para baixar novamente...")
        time.sleep(waitTime)
