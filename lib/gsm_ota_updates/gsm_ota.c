#include "gsm_ota.h"
#include <driver/uart.h>
#include <freertos/queue.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>


#define EX_UART_NUM UART_NUM_1
#define TX_PIN GPIO_NUM_17
#define RX_PIN GPIO_NUM_16
#define BUF_SIZE 2048
#define RD_BUF_SIZE (BUF_SIZE)

static QueueHandle_t uart0_queue;
static esp_err_t _uart_init();

static const char* GSM_HTTPS_REQUESTS[] = {
    "AT+QIFGCNT=0",
    "AT+QICSGP=1,\"CMNET\"",
    "AT+QIMODE=0",
    "AT+QIMUX=1",
    "AT+QIREGAPP= \"CMNET\"",
    "AT+QIACT",
    "AT+QILOCIP",
    "AT+QINDI=1",
    // "AT+QIOPEN=1,0,\"TCP\",\"ota2.ismart.link\",80",
    // "AT+QIRD=0,1500",

    // "AT+QFLDS=\"UFS\"",
    // "AT+QFLST=\"*\"",

    // "AT+QHTTPURL=30,10",
//     "AT+QIREGAPP",
//     "AT+QIACT",
//     // "AT+USEROTA=39",
    "AT+QHTTPURL=39,20",
    "http://ota2.ismart.link/test_remote.bin",

//     // // Download the file to RAM (adjust the size according to your file size)
   "AT+QHTTPGET=60",

// //    "AT+QHTTPREAD=30",

//    "AT+QHTTPDL=\"hello_world.bin\",264240",

    
// //    "+QHTTPDL: 4803,102400,401",
  

    // "AT+QFDWL=\"hello_world.bin\"",


    // "AT+QFOPEN=\"hello_world.bin\",0",
    // "AT+QFSEEK=1027,0,0",
    // "AT+QFREAD=1027,10240",

    // "AT+QFCLOSE=1027",


// "AT+QIDEACT",

//     "AT+QFREAD=1028,5000",

//     "AT+QFCLOSE=1028",
//     // "AT+QFDEL=\"*\"",
//      "AT+QFLST=\"*\"",

//     //  "AT+QFDEL=\"*\"",
//     "AT+QHTTPDL=?",
//     "AT+QFREAD=?",
// //    "AT+QFREAD=\"RAM:test.bin\",3803",


//     // "AT+QIDEACT",
//     // // Save the file to the module's local storage (optional)
//     // "AT+QHTTPRCV=\"test_remote.bin\"",

//     // Terminate the HTTP session
//     "AT+QHTTPDEL",

    // "AT+QICSGP=1,\"CMNET\"",
    // "AT+QIMODE=0",
    // "AT+QIMUX=1",
    // "AT+QIMUX?",
    // "AT+QIREGAPP= \"CMNET\"",
    // "AT+QIACT",
    // "AT+QILOCIP",
    // "AT+QINDI=1",
    // "AT+QIOPEN=1,\"TCP\",\"ota2.ismart.link\",80",
    // "AT+QIOPEN?",
    // "AT+QIRD=1,1500",
    NULL
};

#define TAG "gsm_ota"
esp_err_t gsm_begin() 
{
    _uart_init();
    // this will configure the gsm for internet connection.
    

    return ESP_OK;
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


static void uart_event_task(void *pvParameters)
{
    ESP_LOGI(TAG , "inside uart event task\n");
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(uart0_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            ESP_LOGI(TAG , "received data\n");
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type) {
            //Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA SIZE]: %d", event.size);
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[DATA ]: %s" , (const char*)dtmp);
                break;
            //Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            //Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider increasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            //Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            //Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            //Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            //Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}





static esp_err_t _uart_init () 
{

    esp_err_t esp_err = ESP_OK;

    // uart_config_t uart_config = {
    //     .baud_rate = 115200,
    //     .data_bits = UART_DATA_8_BITS,
    //     .parity = UART_PARITY_DISABLE,
    //     .stop_bits = UART_STOP_BITS_1,
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    //     .source_clk = UART_SCLK_APB,
    // };

    // uart_driver_install(EX_UART_NUM , BUF_SIZE * 2 , BUF_SIZE * 2 , 5 , &uart0_queue , 0);
    // uart_param_config(EX_UART_NUM , &uart_config);
    // uart_set_pin(EX_UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

 
    // esp_log_level_set("*", ESP_LOG_INFO);

    // uart_write_bytes(EX_UART_NUM, "hello uart", 11);

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 5, &uart0_queue, 0);

    // Set UART as the standard output
    esp_log_level_set("*", ESP_LOG_INFO);

    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
    
    uart_write_bytes(EX_UART_NUM , "AT\r\n" , 5);

    return esp_err;

}