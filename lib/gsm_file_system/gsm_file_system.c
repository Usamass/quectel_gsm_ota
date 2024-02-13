#include "gsm_file_system.h"

#define TAG "GSM_FILE_SYS"


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
    "AT+QIFGCNT=0\r\n",
    "AT+QICSGP=1,\"JAZZ\"\r\n",
    // "AT+QHTTPCFG =\"responseheader\",1\r\n",
    // "AT+QFLST=\"test_remote.bin\"\r\n",
    // "AT+QFDEL=\"test_remote.bin\"\r\n",
    "AT+QHTTPURL=39,30\r\n",
    "AT+QHTTPGET=60\r\n",
    // "AT+QHTTPREAD=60\r\n",
    "AT+QHTTPDL=\"RAM:firmware.bin\",150000\r\n",
    // "AT+QFLST=\"RAM:ota.bin\"\r\n",
    
    // "AT+QFUPL=\"test_remote.bin\",300000\r\n",
    // "AT+QFLST=\"RAM:*\"\r\n",
    "AT+QFOPEN=\"RAM:firmware.bin\",0\r\n",
    "AT+QFSEEK=134072,135850,0\r\n",
    "AT+QFREAD=134072,4000\r\n",
    "AT+QFCLOSE=134072\r\n",


    // "AT+QFLST=\"RAM:*\"\r\n",

    // "AT+QFUPL=\"ota.txt\",\"1024\"\r\n",
    
    // "AT+QHTTPDL=\"RAM:1.TXT\",1024",

    NULL
};

unsigned long gsm_open(char *file_name, uint8_t mode)
{
    char* endptr;
    char command_buf  [100] = {0};
    char response_buf [100] = {0};
    char response_need[50 ] = {0};
    sprintf(command_buf , "AT+QFOPEN=\"RAM:%s\",%d\r\n" , file_name , mode);

    gsm_uart_init();
    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write open command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , 50) == -1) {
        ESP_LOGW(TAG , "unable to read open command response\n");
    }

    gsm_parse_response(response_buf , response_need);

    return strtol(response_need , &endptr , 10);




    return 0;
}
