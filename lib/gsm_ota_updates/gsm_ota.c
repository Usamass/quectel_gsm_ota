#include "gsm_ota.h"
#include <driver/uart.h>
#include <freertos/queue.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define EX_UART_NUM UART_NUM_1
#define TX_PIN GPIO_NUM_17
#define RX_PIN GPIO_NUM_16
#define BUF_SIZE 2000
#define RD_BUF_SIZE (BUF_SIZE)

// static QueueHandle_t uart0_queue;
static void _uart_init();


static char* GSM_HTTPS_REQUESTS[] = {
    // "AT+QICSGP=1,1,\"JAZZ\",\"\",\"\",1\r\n",
    // "AT+QIACT=1\r\n",
    // "AT+QHTTPCFG=\"contextid\",1\r\n",
    // "AT+QHTTPURL=20,30\r\n",
    // "AT+QHTTPGET=60\r\n",
    // "AT+QHTTPREAD=30\r\n",
    // "AT+QHTTPDL=\"RAM:test_remote.txt\",15000\r\n",
    // "AT+QIDNSGIP=\"www.google.com\"\r\n",
    // "AT+QPING=\"www.google.com\"\r\n",
    // "AT+QFLDS=\"UFS\"\r\n",
    "AT\r\n",
    // "AT+QHTTPCFG =\"responseheader\",1\r\n",
    "AT+QIFGCNT=0\r\n",
    "AT+QICSGP=1,\"JAZZ\"\r\n",
    
    // "AT+QFLST=\"test_remote.bin\"\r\n",
    // "AT+QFDEL=\"test_remote.bin\"\r\n",
    "AT+QHTTPURL=39,30\r\n",
    "AT+QHTTPGET=60\r\n",
    // "AT+QHTTPREAD=60\r\n",
    "AT+QHTTPDL=\"RAM:ota_firmware.bin\",200000\r\n",
    // "AT+QFLST=\"RAM:ota.bin\"\r\n",
    
    // "AT+QFUPL=\"test_remote.bin\",300000\r\n",
    // "AT+QFLST=\"RAM:*\"\r\n",
    // "AT+QFLST=\"RAM:ota_remote.bin\"\r\n"
    // "AT+QFOPEN=\"RAM:ota_remote.bin\",0\r\n",
    // "AT+QFSEEK=134072,10254,1\r\n",
    // "AT+QFREAD=134072,1000\r\n",
    // "AT+QFCLOSE=134072\r\n",
    "AT+QIDEACT\r\n",


    // "AT+QFLST=\"RAM:*\"\r\n",

    // "AT+QFUPL=\"ota.txt\",\"1024\"\r\n",
    
    // "AT+QHTTPDL=\"RAM:1.TXT\",1024",

    NULL
};

#define TAG "gsm_ota"
void gsm_begin(gsm_ota_https_config_t* gsm_ota)     // this will configure the gsm for internet connection.
{
    printf("inside gsm begin function\n");
    _uart_init();

    
    uint8_t* data = (uint8_t*)malloc(BUF_SIZE);

    // const char* url = gsm_ota->http_config->url;
    // size_t url_len = strlen(url);
    char* url_ptr = "http://ota2.ismart.link/ota_firmware-1.bin";
    //"https://bcdb8831-1995-4e0f-a891-42ccf33e4f79.mock.pstmn.io/firmware";
    
    //"http://ash-speed.hetzner.com/100MB.bin";

    


    //"http://ota2.ismart.link/get_file.php";
    //"http://www.google.com";
    
    //"http://speedtest.dallas.linode.com/100MB-dallas.bin";
    // (char*)malloc(url_len +2);
    // sprintf(url_ptr , "%s\r\n" , url);

    char url_buffer[50] = {0};

    sprintf(url_buffer , "AT+QHTTPURL=%d,30\r\n" , strlen(url_ptr));

    int bytes = uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[0] , 
                            strlen(GSM_HTTPS_REQUESTS[0]));

    printf("number of bytes written: %d\n" , bytes);

    vTaskDelay(pdMS_TO_TICKS(1000));

    int len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[1] , 
                            strlen(GSM_HTTPS_REQUESTS[1]));

    vTaskDelay(pdMS_TO_TICKS(1000));

    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[2] , 
                            strlen(GSM_HTTPS_REQUESTS[2]));

    vTaskDelay(pdMS_TO_TICKS(1000));

    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , url_buffer , 
                            strlen(url_buffer));

    vTaskDelay(pdMS_TO_TICKS(1000));

    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , url_ptr , 
                            strlen(url_ptr));

    vTaskDelay(pdMS_TO_TICKS(1000));

    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }



    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[4] , 
                            strlen(GSM_HTTPS_REQUESTS[4]));

    vTaskDelay(pdMS_TO_TICKS(30000));

    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[5] , 
                            strlen(GSM_HTTPS_REQUESTS[5]));

    vTaskDelay(pdMS_TO_TICKS(30000));

   len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }


    uart_flush(EX_UART_NUM);
    uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[6] , 
                            strlen(GSM_HTTPS_REQUESTS[6]));

    vTaskDelay(pdMS_TO_TICKS(1000));

   len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)data);
    }

    // char ota_buffer[1000] = {0};
    // // gsm_get_file_size("ota_remote.bin");
    // unsigned long fh = gsm_open("ota_firmware.bin" , 0);

    // gsm_lseek(fh , 0 , 1);

    // gsm_read(fh , 1000 , ota_buffer);

//     uart_flush(EX_UART_NUM);
//     uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[8] , 
//                             strlen(GSM_HTTPS_REQUESTS[8]));

//     vTaskDelay(pdMS_TO_TICKS(5000));

//    len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
//     char ota_buffer[1500] = {0};
//     if (len > 0) {
//         // data[len] = '\0'; // null terminator --> end of the string / array
//         printf("Len: %d\n", len);
//         for (int i = 38 ; i < len -2 ; i++) {
//             ota_buffer[i] = data[i];
//             printf("%c" , ota_buffer[i]);
//         }
//         ota_buffer[len] = '\0';
//     }
    


    



    // uart_flush(EX_UART_NUM);
    // uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[6] , 
    //                         strlen(GSM_HTTPS_REQUESTS[6]));

    // vTaskDelay(pdMS_TO_TICKS(5000));

    // uart_flush(EX_UART_NUM);
    // uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[7] , 
    //                         strlen(GSM_HTTPS_REQUESTS[7]));

    // vTaskDelay(pdMS_TO_TICKS(5000));

    // uart_flush(EX_UART_NUM);
    // uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[8] , 
    //                         strlen(GSM_HTTPS_REQUESTS[8]));

    // vTaskDelay(pdMS_TO_TICKS(10000));

    // uart_flush(EX_UART_NUM);
    // uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[9] , 
    //                         strlen(GSM_HTTPS_REQUESTS[9]));

    // vTaskDelay(pdMS_TO_TICKS(5000));






    free(data);

}

esp_err_t gsm_ota_begin(gsm_ota_https_config_t* gsm_ota ,  gsm_spiffs_ota_partition_t* gsm_spiffs)
{

    // this will begin downloading the firmware into gsm_spiffs.


    return ESP_OK;


}

esp_err_t gsm_ota_perform(gsm_spiffs_ota_partition_t* gsm_spiffs) 
{

    // this will begin copying firmware from spiffs to ota partition.


    return ESP_OK;

}


// static void uart_event_task(void *pvParameters)
// {
//     uart_event_t event;
//     uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
//     for (;;) {
//         //Waiting for UART event.
//         if (xQueueReceive(uart0_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
//             printf("received data\n");
//             bzero(dtmp, RD_BUF_SIZE);
//             switch (event.type) {
          
//             case UART_DATA:
//                 uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
//                 char* found = strstr((char*)dtmp , "+QHTTPDL:");
//                 if (found != NULL) {

//                 uart_flush(EX_UART_NUM);

//                 uart_write_bytes(EX_UART_NUM , GSM_HTTPS_REQUESTS[5] , 
//                                         strlen(GSM_HTTPS_REQUESTS[5]));
//                     printf("string found: %s\n" , found);
//                 }
//                 printf("[DATA SIZE: %d]\t[DATA ]: %s" ,event.size, (const char*)dtmp);
//                 break;
//             //Event of UART ring buffer full
//             case UART_BUFFER_FULL:
//                 printf("ring buffer full");
//                 uart_flush_input(EX_UART_NUM);
//                 xQueueReset(uart0_queue);
//                 break;
//             //Event of UART RX break detected
//             case UART_BREAK:
//                 printf("uart rx break");
//                 break;
//             //Event of UART parity check error
//             case UART_PARITY_ERR:
//                 printf("uart parity error");
//                 break;
//             //Event of UART frame error
//             case UART_FRAME_ERR:
//                 printf("uart frame error");
//                 break;
//             //Others
//             default:
//                 printf("uart event type: %d", event.type);
//                 break;
//             }
//         }
//     }
//     free(dtmp);
//     dtmp = NULL;
//     vTaskDelete(NULL);
// }





static void _uart_init () 
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));

    // ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 5, &uart0_queue, 0));


    // xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);



}