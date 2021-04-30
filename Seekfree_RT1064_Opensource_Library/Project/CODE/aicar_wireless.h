#ifndef _aicar_wireless_h
#define _aicar_wireless_h
#include "common.h"
#include "SEEKFREE_WIRELESS.h"
#include "zf_uart.h"
#include "board.h"

//º¯Êý
void aicar_pid_wireless(void);
void aicar_camera_wireless(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height);

#endif
