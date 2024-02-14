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