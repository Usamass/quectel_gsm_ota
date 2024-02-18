#include "gsm_file_system.h"

#define TAG "GSM_FILE_SYS"
#define BUF_SIZE 255
#define READ_BUF_SIZE 1500



unsigned long gsm_open(char *file_name, uint8_t mode)
{
    char command_buf  [100] = {0};
    uint8_t response_buf [BUF_SIZE] = {0};
    unsigned long file_handle = 0;

    sprintf(command_buf , "AT+QFOPEN=\"RAM:%s\",%d\r\n" , file_name , mode);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write open command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open command response\n");
    }

    char* str = strstr((char*)response_buf , "+QFOPEN:");
    if (str != NULL) {
        if (sscanf(str, "%*[^:]:%ld", &file_handle) == 1) {
            printf("Size after the first comma: %ld\n", file_handle);
        } else {
            printf("Error parsing the input string.\n");
        }

    }
    else {
        ESP_LOGW(TAG , "unable to get the size of file\n");

        // need error handling code here..

    }



    return file_handle;
}


unsigned int gsm_read(int file_handle , int chunk_size , char* buffer) // returns number of bytes read.
{
    unsigned int bytes_read = 0;

    char command_buf  [100] = {0};
    // uint8_t response_buf [READ_BUF_SIZE] = {0};
    uint8_t* response_buf = (uint8_t*)malloc(READ_BUF_SIZE);

    sprintf(command_buf , "AT+QFREAD=%d,%d\r\n" , file_handle , chunk_size);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write open command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(3000));  // delay for uart buffer to fillup with response.

    bytes_read = gsm_firmware_read(buffer , response_buf , READ_BUF_SIZE);

    free(response_buf);






    return bytes_read;
}

unsigned int gsm_lseek(int file_handle , unsigned long off_set , uint8_t pos) {

    char command_buf  [100] = {0};
    uint8_t response_buf [BUF_SIZE] = {0};
    unsigned long file_offset = 0;

    sprintf(command_buf , "AT+QFSEEK=%d,%ld,%d\r\n" , file_handle , off_set , pos);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write open command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(response_buf , BUF_SIZE) == -1) {
        ESP_LOGW(TAG , "unable to read open command response\n");
    }

    char* str = strstr((char*)response_buf , "OK");
    if (str == NULL) {
        
        ESP_LOGW(TAG , "unable to get the size of file\n");

        // need error handling code here..

    }



    return off_set;

} 

