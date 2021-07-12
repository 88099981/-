#ifndef _aicar_meau_main_h
#define _aicar_meau_main_h
#include "common.h"

extern vuint8 meau_run_mode;
extern vuint8 meau_garage_mode;

void aicar_meau_init(void);
void aicar_meau_scan(void);
void up_status(void);
void down_status(void);
void go_status(void);
void back_status(void);
#endif