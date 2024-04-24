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
#define BUF_SIZE (500)
#define RD_BUF_SIZE (BUF_SIZE)

#define REBOOT_TIME (2000) //ms
#define RECEIVE_BUF_DELAY (1000) //ms
#define DOWNLOAD_DELAY (30000) //ms
#define MAX_TRY (3)

static void _uart_init();
static void http_server_resp (long* resp_arr , char* resp); 

const esp_partition_t* running_partition;
const esp_partition_t* update_partition;
esp_ota_handle_t ota_firmware_handle = 0;  /* Handler for OTA update. */

int data_read , ret;

ssize_t bytes_read;
off_t offset = 0;  // Initial offset

#define TAG "gsm_ota"
void gsm_begin()     // this will configure the gsm for internet connection.
{
    _uart_init();

    char command_buf  [100] = {0};
    uint8_t response_buf [BUF_SIZE] = {0};

    sprintf(command_buf , "AT\r\n");

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write AT command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open http get response\n");
    }


    char* str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "GSM is not responding..\n");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        

    }
    
    sprintf(command_buf , "AT+QIFGCNT=0\r\n");

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write AT command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open http get response\n");
    }


    str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "PDP activation failed..\n");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        

    }


    sprintf(command_buf , "AT+QICSGP=1,\"JAZZ\"\r\n");

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write AT command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open http get response\n");
    }


    str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "PDP activation failed..\n");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        

    }




}

esp_err_t gsm_ota_begin(gsm_ota_https_config_t* gsm_ota , gsm_remote_firmware_conf_t* remote_firmware)
{
    esp_err_t err = ESP_OK;
    int max_try = 0;
    char* remote_url = gsm_ota->http_config->url;
    int time_out = gsm_ota->http_config->timeout_ms;
    char* file_name = remote_firmware->file_name;
    unsigned long firmware_size = remote_firmware->firmware_size;

    char command_buf  [100] = {0};
    uint8_t response_buf [BUF_SIZE] = {0};
    long* resp = (long*)malloc(sizeof(long) * 3);


    sprintf(command_buf , "AT+QHTTPURL=%d,%d\r\n" , strlen(remote_url) ,  time_out);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to set http url command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read http url  response\n");
    }

    char* str = strstr((char*)response_buf , "CONNECT");
    if (str == NULL) {
        ESP_LOGE(TAG , "\n");
        vTaskDelay(pdMS_TO_TICKS(REBOOT_TIME));
        esp_restart();

    }


    sprintf(command_buf , remote_url);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to set http url command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read http url  response\n");
    }

    str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "\n");
        vTaskDelay(pdMS_TO_TICKS(REBOOT_TIME));
        esp_restart();

    }
    


    sprintf(command_buf , "AT+QHTTPGET=%d\r\n" , time_out);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write http get command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(DOWNLOAD_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read http get response\n");
    }

    str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "GET request failed\nRestarting esp32!\n");
        vTaskDelay(pdMS_TO_TICKS(REBOOT_TIME));
        esp_restart();

    }

    sprintf(command_buf , "AT+QHTTPDL=\"RAM:%s\",%ld\r\n" , file_name , firmware_size);
    

    // do {

        if (gsm_write_command(command_buf) == -1) {
            ESP_LOGW(TAG , "unable to write http get command\n");
        }

        vTaskDelay(pdMS_TO_TICKS(DOWNLOAD_DELAY));  // delay for uart buffer to fillup with response.

        if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
            ESP_LOGW(TAG , "unable to read open http get response\n");
        }

        str = strstr((char*)response_buf , ":");
        printf("%s" , str);

        // http_server_resp(resp , str);

        // for (int i = 0 ; i < 3 ; i++) {
        //     printf("%ld\n" , resp[i]);
        // }
        // max_try++;



    // } while (resp[HTTP_DOWNLOADED_SIZE] != resp[HTTP_TOTAL_SIZE] && max_try < MAX_TRY);

    // free(resp);

    // if (max_try > 2) {
    //     ESP_LOGE(TAG , "Cannot Download firmware\nesp is going to reboot\n");
    //     esp_restart();
    // }


    sprintf(command_buf , "AT+QIDEACT\r\n");

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write http get command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(RECEIVE_BUF_DELAY));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open http get response\n");
    }

    str = strstr((char*)response_buf , "DEACT OK");
    if (str == NULL) {
        ESP_LOGE(TAG , "PDP deactivation context failed\n");
        // vTaskDelay(pdMS_TO_TICKS(REBOOT_TIME));
        // esp_restart();

    }


    // this will begin downloading the firmware into gsm RAM.


    return err;


}

esp_err_t gsm_ota_perform(gsm_remote_firmware_conf_t* remote_firmware)
{
    running_partition = esp_ota_get_running_partition();
    update_partition = esp_ota_get_next_update_partition(NULL);


    // ESP_LOGI(TAG , "running partition: %s \t update partition: %s" , running_partition->label , update_partition->label);

    unsigned long fh = gsm_open(remote_firmware->file_name , 0);

    
    ESP_LOGI(TAG, "Starting OTA");
    update_partition = esp_ota_get_next_update_partition(NULL);
    
    assert(update_partition != NULL);
    // Reset of this partition
    ESP_ERROR_CHECK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_firmware_handle));
    // Temporary buffer for writing the chunks of file read from the file ota_firmware.bin(gsm).
    char ota_buffer[CHUNK_SIZE];
    memset(ota_buffer, 0, sizeof(ota_buffer));


    do{
        gsm_lseek(fh , offset , 0);
        
        data_read = gsm_read(fh , CHUNK_SIZE , ota_buffer);
        
        ret = esp_ota_write(ota_firmware_handle, (const void*)ota_buffer, data_read);
        // In case of failure it sends a log and exits.
        if(ret != ESP_OK){
            ESP_LOGE(TAG, "Firmware upgrade failed");
            gsm_close(fh);
            return ESP_FAIL;
            
        }
      
        offset += CHUNK_SIZE;
    } 
    while(data_read == CHUNK_SIZE);
    ESP_ERROR_CHECK(esp_ota_end(ota_firmware_handle));
    // // OTA partition configuration
    ESP_ERROR_CHECK(esp_ota_set_boot_partition(update_partition));

    gsm_close(fh);
    ESP_LOGI(TAG, "Restarting...");
    // // REBOOT!!!!!
    esp_restart();

    // this will begin copying firmware from gsm RAM to ota partition.


    return ESP_OK;

}


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


}


static void http_server_resp (long* resp_arr , char* resp) {
    char* token , *rest , *endptr;
    rest = resp;
    char* new_str = strstr(resp , " ");
    rest = (new_str +1);  // skip 1 space
    long num = 0;
    int iterater = 0;

    while (*rest != 0) {
        token = strtok_r(rest , "," , &rest);
        num = strtol(token , &endptr , 10);
        resp_arr[iterater] = num;
        // printf("num: %ld\n" , num);
        iterater ++;
    }


}