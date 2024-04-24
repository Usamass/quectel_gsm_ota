#include <esp_ota_ops.h>
#include <esp_https_ota.h>
#include <sys/cdefs.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include "gsm_utils.h"
#include "gsm_file_system.h"

#define BUFF_SIZE 1024
#define CHUNK_SIZE 1000


typedef enum 
{
    // OPEN_TCP_COMMAND,
    SET_URL_COMMAND,
    GET_REQUEST_COMMAND,
    READ_RESPONSE_COMMAND

}Quectel_command;

typedef enum 
{
    HTTP_DOWNLOADED_SIZE,
    HTTP_TOTAL_SIZE,
    HTTP_RESPONSE_CODE


}http_server_resp_e;


typedef struct 
{
    const char* url;
    const uint8_t* cert_pem;
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
    unsigned long firmware_size;

}gsm_remote_firmware_conf_t;

void gsm_begin(); 
esp_err_t gsm_ota_begin(gsm_ota_https_config_t* , gsm_remote_firmware_conf_t*);
esp_err_t gsm_ota_perform(gsm_remote_firmware_conf_t*);


