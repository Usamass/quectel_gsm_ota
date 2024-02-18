#include "gsm_utils.h"

#define EX_UART_NUM UART_NUM_1
// #define TX_PIN GPIO_NUM_17
// #define RX_PIN GPIO_NUM_16
#define BUF_SIZE 255

static bool uart_init_flag = 0;

#define TAG "UART TAG"

// void gsm_uart_init () 
// {
//     if (!uart_init_flag) {

//         uart_config_t uart_config = {
//             .baud_rate = 115200,
//             .data_bits = UART_DATA_8_BITS,
//             .parity = UART_PARITY_DISABLE,
//             .stop_bits = UART_STOP_BITS_1,
//             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//             .source_clk = UART_SCLK_APB
//         };

//         ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
//         ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
//         ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));

//         // ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 5, &uart0_queue, 0));


//         // xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
//         uart_init_flag = 1;
//     }


// }


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
        while (newlines < 2) {
            if (buf[iterater] == '\n'){
                newlines++;
            } 
            iterater++;
        }
        start_index = iterater;
        
        iterater = 0;
        while (carr_ret < 2) {
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

        

        int substring_length = end_index - start_index +1;

        printf("length: %d\ndata length: %d\n" , len , read_size);

        // printf("substring len: %d\n" , substring_length);

        // data[len] = '\0'; // null terminator --> end of the string / array
        // for (int i = 38 ; i < len -6 ; i++) {
        //     printf("%c" , buf[i]);
        // }
        
        // for (int i = 0; i < substring_length; i++) {
        //     // ota_buffer[i] = buf[start_index + i];
        //     printf("%c" , buf[i +start_index]);
        // }
        // printf("hello world");

        for (int i = start_index ; i < end_index ; i++) {
            printf("%d\n" , buf[i]);

        }
        // printf("end_index: %d\n" , buf[end_index]);

        // printf("last six bytes\n");

        // printf("%d\n" , buf[len]);
        // printf("%d\n" , buf[len -1]);
        // printf("%d\n" , buf[len -2]);
        // printf("%d\n" , buf[len -3]);
        // printf("%d\n" , buf[len -4]);
        // printf("%d\n" , buf[len -5]);



        
        
    }
    

    return read_size;
    // return (len -44);


}


unsigned long gsm_get_file_size(char* file) {

    // uint8_t* resp_buf = (uint8_t*)malloc(255);
    uint8_t resp_buf[BUF_SIZE] = {0};
    unsigned long size = 0;

    char command_buf[100] = {0};

    sprintf(command_buf , "AT+QFLST=\"RAM:%s\"\r\n" , file);
    // gsm_uart_init();

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




void gsm_parse_response(char* buf , char* resp_need) {
    char* token , *rest;
    rest = buf;

    if (strstr(buf , "OK") != NULL) {
        strcpy(resp_need , buf);
    }
    else if (strstr(buf , "+QFOPEN") != NULL) {
        token = strtok_r(rest , ":" , &rest);
        strcpy(resp_need , rest);
        
    }
    else if (strstr(buf , "+QFLST") != NULL) {
        token = strtok_r(rest , "," , &rest);
        strcpy(resp_need , rest);
    }

    // yet to implement....






}
