#pragma once 

#include <string.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <esp_log.h>

int gsm_write_command(char* command); // returns number of bytes written.
int gsm_read_response(uint8_t* buf , size_t buf_size); // returns number of bytes read.

unsigned long gsm_get_file_size(char* file);

void gsm_parse_response(char* buf , char* resp_need);

//* gsm_open
// gsm_lseek
//* gsm_get_file_size
// gsm_is_downloaded?
// gsm_get_filename.

