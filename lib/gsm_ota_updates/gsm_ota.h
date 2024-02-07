#include <esp_ota_ops.h>
#include <esp_https_ota.h>
#include <sys/cdefs.h>
#include <esp_err.h>
#include <esp_log.h>

#define BUFF_SIZE 1024



typedef enum 
{
    // OPEN_TCP_COMMAND,
    SET_URL_COMMAND,
    GET_REQUEST_COMMAND,
    READ_RESPONSE_COMMAND

}Quectel_command;


typedef struct 
{
    const char* url;
    const char* cert_pem;
    int timeout_ms;

}gsm_ota_client_config_t;

typedef struct  
{

    gsm_ota_client_config_t* http_config;
    esp_https_ota_config_t https_ota_handle;

    

}gsm_ota_https_config_t;

typedef struct 
{
    const char* file_name;
    char data_buffer[BUFF_SIZE];


}gsm_spiffs_ota_partition_t;

void gsm_begin(gsm_ota_https_config_t*); 
esp_err_t gsm_ota_begin(gsm_ota_https_config_t* , gsm_spiffs_ota_partition_t*);
esp_err_t gsm_ota_perform(gsm_spiffs_ota_partition_t*);


