#pragma once

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "gsm_utils.h"
// gsm_open
// gsm_read
// gsm_lseek
// 

unsigned long gsm_open(char* file_name , uint8_t mode);  // returns file handle

unsigned int gsm_read(int file_handle , int chunk_size , char* buffer); // returns number of bytes read.

unsigned int gsm_lseek(int file_handle , unsigned long off_set , uint8_t pos);  // 

bool gsm_close(int file_handle);