#include "gsm_utils.h"

#define EX_UART_NUM UART_NUM_1

#define BUF_SIZE 255

static bool uart_init_flag = 0;

#define TAG "UART TAG"


int gsm_write_command(char* command) { // returns number of bytes written.

    uart_flush(EX_UART_NUM);
    return uart_write_bytes(EX_UART_NUM , command , strlen(command));

}


int gsm_read_response(uint8_t* buf , size_t buf_size) {  // returns number of bytes read.
    int len = uart_read_bytes(EX_UART_NUM, buf, buf_size, 0 / portTICK_PERIOD_MS);

    if (len > 0) {
        buf[len] = '\0'; // null terminator --> end of the string / array
        printf("Response: %s\n", (char*)buf);
    }

    return len;

}

int gsm_firmware_read(char* ota_buffer , uint8_t* buf , size_t buf_size) {
    int len = uart_read_bytes(EX_UART_NUM, buf, buf_size, 0 / portTICK_PERIOD_MS);
    unsigned int read_size = 0;
    unsigned int ota_size = 0;

    if (len > 0) {
        int start_index = 0;
        int end_index = 0;
        uint8_t newlines = 0;
        uint8_t carr_ret = 0;
        uint8_t iterater = 0;
        while (newlines < 2) {   // this will skip extra msg from response (two new lines(chars) skipped).
            if (buf[iterater] == '\n'){
                newlines++;
            } 
            iterater++;
        }
        start_index = iterater;
        
        iterater = 0;
        while (carr_ret < 2) {      // this will skip extra msg from the bottom of the response
            end_index = (len -iterater);
            if (buf[end_index] == '\n'){
                carr_ret++;
            } 
            iterater++;
            
        }


        if (strstr((char*)buf , "CONNECT") != NULL) {
            if (sscanf((char*)buf, "%*[^ ] %d", &read_size) == 1) {
                printf("Size after the first space: %d\n", read_size);
            } else {
                printf("Error parsing the input string.\n");
            }
        }

        
        printf("length: %d\ndata length: %d\nend index: %d\n" , len , read_size , end_index);


        for (int i = start_index ; i < (end_index -1) ; i++) {
            ota_buffer[i -start_index] = buf[i];
            ota_size++;

        }

        printf("ota size: %d\n" , ota_size);
        
    }
    

    return read_size;

}


unsigned long gsm_get_file_size(char* file) {

    uint8_t resp_buf[BUF_SIZE] = {0};
    unsigned long size = 0;

    char command_buf[100] = {0};

    sprintf(command_buf , "AT+QFLST=\"RAM:%s\"\r\n" , file);

    if (gsm_write_command(command_buf) == -1) {
        ESP_LOGW(TAG , "unable to write open command\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // delay for uart buffer to fillup with response.

    if (gsm_read_response(resp_buf , 255) == -1) {
        ESP_LOGW(TAG , "unable to read open command response\n");
    }

    if (strstr((char*)resp_buf , "+QFLST") != NULL) {
        if (sscanf((char*)resp_buf, "%*[^,],%ld", &size) == 1) {
            printf("Size after the first comma: %ld\n", size);
        } else {
            printf("Error parsing the input string.\n");
        }

    }
    else {
        ESP_LOGW(TAG , "unable to get the size of file\n");
        // need error handling code hare...

    }

    
    return size;




}


