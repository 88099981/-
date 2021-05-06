#ifndef _aicar_error_h
#define _aicar_error_h

#include"common.h"
#define ERROR_EDGE 15 
#define KP_cam 
#define KD_cam
extern int16 camera_error;

void aicar_camera_error();
void aicar_adc_error();

#endif