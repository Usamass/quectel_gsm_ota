#pragma once 

#include <string.h>
#include <driver/uart.h>
#include <driver/gpio.h>

void gsm_uart_init ();
int gsm_write_command(char* command); // returns number of bytes written.
int gsm_read_response(char* buf , size_t buf_size); // returns number of bytes read.

void gsm_parse_response(char* buf , char* resp_need);

// gsm_lseek
// gsm_get_file_size
// gsm_is_downloaded?
// gsm_get_filename.

// gsm_uart_init
// gsm_is_uart
