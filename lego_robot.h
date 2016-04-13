/*
 * lego_robot.h
 *
 * Author: In Hwan "Chris" Baek
 *         chris.inhwan.baek@gmail.com
 */
 
#ifndef __LEGO_ROBOT_H__
#define __LEGO_ROBOT_H__

#include <stdio.h>
#include <mraa.h>

extern mraa_pwm_context pwm1, pwm2;
extern mraa_gpio_context A1, A2, B1, B2, standbyPin;

void initialize();
void straight(float);
void left_turn();
void right_turn();
void rotate_CW();
void rotate_CCW();
void forward_A();
void reverse_A();
void forward_B();
void reverse_B();
void standby(int);
void brake_A();
void brake_B();
void stop();
void set_speed(float);
int offLine(int L, int R);
int turn(int L, int R, bool turn);

#endif //LEGO_ROBOT_H