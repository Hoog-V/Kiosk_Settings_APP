//
// Created by victor on 3-9-23.
//

#ifndef LVGL_PCSC_BINDING_H
#define LVGL_PCSC_BINDING_H

typedef long SCARDHANDLE;

long establish_connection_with_smartcard_system_daemon(void);
char *get_available_smartcard_reader();
unsigned long get_smartcard_uid (SCARDHANDLE* smartcard_handle, unsigned char* pbRecvBuffer, unsigned long dwRecvLength);
long connect_with_smartcard(const char *smartcard_reader_handle, SCARDHANDLE* smartcard_handle);
long disconnect_smartcard(SCARDHANDLE* smartcard_handle);
long smartcard_wait_for_card_presence(const char *reader_handle);
void *SmartCardTask(void *vargp);

#endif //LVGL_PCSC_BINDING_H
