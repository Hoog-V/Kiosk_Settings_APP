#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <winscard.h>
#include <lvgl.h>


void error_window(const char *module_name, const char *tip, long error_code);

#define ERROR_MSG_BUFFER_SIZE 512
#define PCSC_ERROR(rv, tip) \
if (rv != SCARD_S_SUCCESS) \
{ \
	printf("E: %s (0x%lX) Tip:" tip "\n", pcsc_stringify_error(rv), rv); \
    error_window("smartcard driver", tip, rv);  \
    }


#define SMARTCARD_NAME_MAX_LENGTH (DWORD)(512)
#define SMARTCARD_UID_MAX_LENGTH_BYTES 10
#define SMARTCARD_REQ_UID_APDU_CMD {0xFF, 0xCA, 0x00, 0x00, 0x00}

static char smartcard_handle_buff[SMARTCARD_NAME_MAX_LENGTH];
SCARDCONTEXT hContext;
SCARD_READERSTATE smartcard_reader_state;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
}

void error_window(const char *module_name, const char *tip, long error_code)
{
    static const char * btns[] ={"Close", ""};
    char error_msg[ERROR_MSG_BUFFER_SIZE];
    snprintf(error_msg, ERROR_MSG_BUFFER_SIZE, "Error occured in %s module: %s \r\n Tip: %s", module_name, pcsc_stringify_error(error_code), tip);
    lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Error occured!", error_msg, btns, true);
    lv_obj_add_event(mbox1, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox1);
}

char * get_available_smartcard_reader() {

    DWORD smartcard_handle_buff_len = SMARTCARD_NAME_MAX_LENGTH;
    long status = SCardListReaders(hContext, NULL, (LPSTR)smartcard_handle_buff, &smartcard_handle_buff_len);
    PCSC_ERROR(status, "Try to reconnect the smartcard reader..")
    return smartcard_handle_buff;
}

unsigned long get_smartcard_uid (SCARDHANDLE* smartcard_handle, unsigned char* pbRecvBuffer, unsigned long dwRecvLength) {
    const SCARD_IO_REQUEST *smartcard_send_protocol_type = SCARD_PCI_T1;
    BYTE pbSendBuffer[] = SMARTCARD_REQ_UID_APDU_CMD;
    DWORD dwSendLength;
    SCARD_IO_REQUEST pioRecvPci;
    /* exchange APDU */
    dwSendLength = sizeof(pbSendBuffer);
    LONG rv = SCardTransmit(*smartcard_handle, smartcard_send_protocol_type, pbSendBuffer, dwSendLength,
                       &pioRecvPci, pbRecvBuffer, &dwRecvLength);
    PCSC_ERROR(rv, "Rest the card on the reader for a longer period")
    return dwRecvLength;
}

long connect_with_smartcard(const char *smartcard_reader_handle, SCARDHANDLE* smartcard_handle) {
    DWORD dwActiveProtocol;
    /* connect to a card */
    dwActiveProtocol = -1;
    unsigned long status = SCardConnect(hContext, smartcard_reader_handle, SCARD_SHARE_SHARED,
                                        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, smartcard_handle, &dwActiveProtocol);
    PCSC_ERROR(status, "Rest the card on the reader for a longer period")
    return status;
}

long disconnect_smartcard(SCARDHANDLE* smartcard_handle) {
    long rv = SCardDisconnect(*smartcard_handle, SCARD_UNPOWER_CARD);
    return rv;
}

long establish_connection_with_smartcard_system_daemon(void) {
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    PCSC_ERROR(rv, "Try to (re)start the smartcard daemon (pcscd)")
//    char * reader = get_available_smartcard_reader(&hContext);
//    rv = connect_with_smartcard(&hContext, reader, &hCard);
//    if(rv != SCARD_S_SUCCESS) {
//        return;
//    }
//    unsigned char recv_buffer[SMARTCARD_UID_MAX_LENGTH_BYTES];
//    unsigned long amount_of_bytes_received = get_smartcard_uid(&hCard, SCARD_PCI_T1, recv_buffer, SMARTCARD_UID_MAX_LENGTH_BYTES);
//    unsigned int i;
//    printf("Received: ");
//    for (i=0; i< amount_of_bytes_received; i++)
//        printf("%02X ", recv_buffer[i]);
//    printf("\n");
//    rv = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
    return rv;

}

long smartcard_wait_for_card_presence(const char *reader_handle){
    long status;
    smartcard_reader_state.szReader = reader_handle;
    smartcard_reader_state.dwCurrentState = SCARD_STATE_UNAWARE;
    status = SCardGetStatusChange(hContext, INFINITE, &smartcard_reader_state, 1);
    if (smartcard_reader_state.dwEventState & SCARD_STATE_EMPTY)
    {
        smartcard_reader_state.dwCurrentState = smartcard_reader_state.dwEventState;
        status = SCardGetStatusChange(hContext, INFINITE, &smartcard_reader_state, 1);
    }
   return status;
}

