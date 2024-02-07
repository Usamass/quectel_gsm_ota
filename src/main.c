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


const esp_partition_t* running_partition;
const esp_partition_t* update_partition;
esp_ota_handle_t ota_firmware_handle = 0;  /* Handler for OTA update. */

#define TAG "OTA_APP"
#define SEND_DATA 1024

static const char* FIRM_FILE = "/spiffs/ota_firmware.bin";
esp_https_ota_handle_t ota_handle;



int data_read , ret;
void app_main(void)
{
 
    gsm_ota_client_config_t ota_client = {
        .url = "ota2.ismart.link/test_remote.bin",
    };

    gsm_ota_https_config_t ota_https = {
        .http_config = &ota_client,
    };

    gsm_begin(&ota_https);



    // file_system_init();

    // struct stat st;
    // if (stat("/spiffs/ota_firmware.bin", &st) == 0)
    // {
    //     printf(">> ota_firmware.bin. %ld\n", st.st_size);
    // }
    // else
    // {
    //     printf(">> ota_firmware.bin NOT found\n");
    // }

    // running_partition = esp_ota_get_running_partition();
    // update_partition = esp_ota_get_next_update_partition(NULL);

    // ESP_LOGI(TAG , "running partition: %s \t update partition: %s" , running_partition->label , update_partition->label);

    // FILE* f = fopen(FIRM_FILE , "r");

    // if (f == NULL) {
    //     ESP_LOGI(TAG , "unable to open file\n");
    //     abort();
    // }
    
    // ESP_LOGI(TAG, "Starting OTA");
    // // It finds the partition where it should write the firmware
    // update_partition = esp_ota_get_next_update_partition(NULL);
    
    // assert(update_partition != NULL);
    // // Reset of this partition
    // ESP_ERROR_CHECK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_firmware_handle));
    // // Temporary buffer where I write the chunks of file read from the file ota_firmware.bin.
    // char buf[SEND_DATA];
    // memset(buf, 0, sizeof(buf));
    // do{
    //     // Put the data read from the file inside buf.
    //     data_read = fread(buf, 1, SEND_DATA, f);
    //     // I write data from buffer to the partition
    //     ret = esp_ota_write(ota_firmware_handle, buf, data_read);
    //     // In case of failure it sends a log and exits.
    //     if(ret != ESP_OK){
    //         ESP_LOGE(TAG, "Firmware upgrade failed");
    //         fclose(f);
    //         while (1) {
    //             vTaskDelay(1000 / portTICK_PERIOD_MS);
    //         }
            
    //     }
    // } 
    // while(data_read == SEND_DATA);
    // ESP_ERROR_CHECK(esp_ota_end(ota_firmware_handle));
    // fclose(f);
    // // OTA partition configuration
    // ESP_ERROR_CHECK(esp_ota_set_boot_partition(update_partition));
    // ESP_LOGI(TAG, "Restarting...");
    // // REBOOT!!!!!
    // esp_restart();



    
}




    
