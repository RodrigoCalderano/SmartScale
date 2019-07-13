/* http_get - Retrieves a web page over HTTP GET.
 *
 * See http_get_ssl for a TLS-enabled version.
 *
 * This sample code is in the public domain.,
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp8266.h"

#include "ssid_config.h"

#define WEB_SERVER "api.thingspeak.com"
#define API_KEY  "UE9SJKO1DN8W9RSB" 
#define FIELD1 "field1=" //temp
#define FIELD2 "field2=" //hum
#define WEB_PORT "80"
#define WEB_PATH "/update?api_key="


const int gpio = 15;


int data_fix(int dado)
{	
	switch(dado){
    	case 48:
    		dado = 0;
    		break;
    	case 49:
    		dado = 1;
    		break;
    	case 50:
    		dado = 2;
    		break;
    	case 51:
    		dado = 3;
    		break;
    	case 52:
    		dado = 4;
    		break;
    	case 53:
    		dado = 5;
    		break;
    	case 54:
    		dado = 6;
    		break;
    	case 55:
    		dado = 7;
    		break;
    	case 56:
    		dado = 8;
    		break;
    	case 57:
    		dado = 9;
    		break;
   	}
	return dado;
}


void http_get_task(void *pvParameters)
{
     

	gpio_enable(gpio, GPIO_OUTPUT);
	

    int successes = 0, failures = 0;
    printf("HTTP get task starting...\r\n");

    while(1) {

    	vTaskDelay(1000 / portTICK_PERIOD_MS);

    	uart_clear_rxfifo(0);
    	uart_flush_rxfifo(0);

    	gpio_write(gpio, 1);  // FAZ REQUISICAO

        int dado1_gas = uart_getc(0);  // PEGA OS DADOS DO SENSOR DE GAS

      	int dado2_gas = uart_getc(0);

      	int dado3_gas = uart_getc(0);

      	int dado4_gas = uart_getc(0);

      	int dado1_peso = uart_getc(0);  // PEGA OS DADOS DO 'SENSOR' DE PESO

      	int dado2_peso = uart_getc(0);

      	int dado3_peso = uart_getc(0);

      	int dado4_peso = uart_getc(0);


      	// FIX DATA
      	dado1_gas = data_fix(dado1_gas);
      	dado2_gas = data_fix(dado2_gas);
      	dado3_gas = data_fix(dado3_gas);
      	dado4_gas = data_fix(dado4_gas);
      	int dado_gas = dado1_gas*1000 + dado2_gas*100 + dado3_gas*10 + dado4_gas*1;

      	dado1_peso = data_fix(dado1_peso);
      	dado2_peso = data_fix(dado2_peso);
      	dado3_peso = data_fix(dado3_peso);
      	dado4_peso = data_fix(dado4_peso);
      	int dado_peso = dado1_peso*1000 + dado2_peso*100 + dado3_peso*10 + dado4_peso*1;



      	gpio_write(gpio, 0); // FIM DA REQUISICAO

        
        const struct addrinfo hints = {
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM,
        };

        struct addrinfo *res;

        printf("Running DNS lookup for %s...\r\n", WEB_SERVER);
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if (err != 0 || res == NULL) {
            printf("DNS lookup failed err=%d res=%p\r\n", err, res);
            if(res)
            freeaddrinfo(res);
            failures++;
            continue;
        }

#if LWIP_IPV6
        {
            struct netif *netif = sdk_system_get_netif(0);
            int i;
            for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                printf("  ip6 %d state %x\n", i, netif_ip6_addr_state(netif, i));
                if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)))
                    printf("  ip6 addr %d = %s\n", i, ip6addr_ntoa(netif_ip6_addr(netif, i)));
            }
        }
#endif

        struct sockaddr *sa = res->ai_addr;
        if (sa->sa_family == AF_INET) {
            printf("DNS lookup succeeded. IP=%s\r\n", inet_ntoa(((struct sockaddr_in *)sa)->sin_addr));
        }
#if LWIP_IPV6
        if (sa->sa_family == AF_INET6) {
            printf("DNS lookup succeeded. IP=%s\r\n", inet6_ntoa(((struct sockaddr_in6 *)sa)->sin6_addr));
        }
#endif

        int s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            printf("... Failed to allocate socket.\r\n");
            freeaddrinfo(res);
           failures++;
            continue;
        }

        printf("... allocated socket\r\n");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            close(s);
            freeaddrinfo(res);
            printf("... socket connect failed.\r\n");
            failures++;
            continue;
        }

        printf("... connected\r\n");
        freeaddrinfo(res);
        
        //int data = dado_peso * 10000 + dado_gas;

        char temp2[10];
        snprintf(temp2, sizeof(temp2),"%d", dado_peso);
        printf("%s \n",temp2);
        printf(temp2);
        
        char hum1[6];
        snprintf(hum1,sizeof(hum1), "%d", dado_gas );
        printf("%s \n",hum1 );
        

        char req[300]="GET " WEB_PATH API_KEY "&" FIELD1;
        strncat(req,temp2,10);
        strncat(req,"&",2);
        strncat(req,FIELD2,20);
        strncat(req,hum1,6);
        strncat(req," HTTP/1.1\r\nHost: "WEB_SERVER"\r\nUser-Agent: esp-open-rtos/0.1 esp8266\r\nConnection: close\r\n\r\n",250);
        
        
        printf("%s \n",req );
        
        
         printf("%s \n",req );
        
        if (write(s, req, strlen(req)) < 0) {
            printf("... socket send failed\r\n");
            close(s);
            failures++;
            continue;
        }
        printf("... socket send success\r\n");

        static char recv_buf[128];
        int r;
        do {
            bzero(recv_buf, 128);
            r = read(s, recv_buf, 127);
            if(r > 0) {
                printf("%s", recv_buf);
            }
        } while(r > 0);

        printf("... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        printf("successes = %d failures = %d\r\n", successes, failures);        
        printf("\r\nEnding!\r\n");
        vTaskDelay(100);
        continue;
    }
}

void user_init(void)
{
    uart_set_baud(0, 9600);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    xTaskCreate(&http_get_task, "get_task", 384, NULL, 2, NULL);
}

