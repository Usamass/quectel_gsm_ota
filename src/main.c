#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <sys/stat.h>
#include <stdio.h>
#include <esp_ota_ops.h>
#include <esp_log.h>
#include <string.h>
#include "file_system.h"
#include "gsm_ota.h"
#include "gsm_file_system.h"


#define TAG "OTA_APP"
#define FIRMWARE_SIZE 200000 //bytes.


void app_main(void)
{
 
    gsm_ota_client_config_t ota_client = {
        .url = "https://ash-speed.hetzner.com/100MB.bin",
        .timeout_ms = 60,
        // .cert_pem = server_cert_pem_start,
    };

    gsm_ota_https_config_t ota_https = {
        .http_config = &ota_client,
    };

    gsm_remote_firmware_conf_t remote_firmware = {
        .file_name = "ota_firmware.bin",
        .firmware_size = FIRMWARE_SIZE,
    };

    gsm_begin();
    printf("ready to download firmware!\n");
    gsm_ota_begin(&ota_https , &remote_firmware);

    // gsm_ota_perform(&remote_firmware);


   


   

    
}




    
