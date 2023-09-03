//
// Created by victor on 3-9-23.
//

#ifndef LVGL_SMARTCARD_AUTH_H
#define LVGL_SMARTCARD_AUTH_H
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pcsc_binding.h>

void *SmartCardTask(void *vargp)
{
    SCARDHANDLE card_handle;
    long status;
    unsigned char card_uid[10];
    status = establish_connection_with_smartcard_system_daemon();
    if(status != 0) {
        return NULL;
    }
    const char *smartcard_reader_handle = get_available_smartcard_reader();
    printf("Reader name: %s \n", smartcard_reader_handle);
    while(1){
        status = smartcard_wait_for_card_presence(smartcard_reader_handle);
        printf("Card detected!\n");
        if(status == 0) {
            printf("Connecting!");
            status = connect_with_smartcard(smartcard_reader_handle, &card_handle);
            if(status == 0){
                unsigned long bytes_read = get_smartcard_uid(&card_handle, card_uid, 10);
                for (uint8_t i =0; i < bytes_read; i++){
                    printf("%02X ", card_uid[i]);
                }
                printf("\n");
                disconnect_smartcard(&card_handle);
                sleep(2);
            }
        }

    }
    return NULL;
}
#endif //LVGL_SMARTCARD_AUTH_H
