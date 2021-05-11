#ifndef _aicar_error_h
#define _aicar_error_h

#include"common.h"
#define ERROR_EDGE 15 
#define KP_cam 8.0
#define KD_cam 5.0
#define A_cam 0.005

extern int16 camera_error;
extern float kp_cam;
extern float kd_cam;
extern float a_cam;

void aicar_camera_error();
void aicar_adc_error();

#endif