/*	servodriver.h
	Defines used by servo driver project.
*/

#include "version.h"
#include "common.h"
#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define NUM_SERVOS 					7							//Controller is designed for 7 servos
#define MAX_SERVO_NAME_LENGTH		16							//15 char description + NULL

#define TIMER_VALUE					20							//We're using a 20탎 timer
#define TIMER_COUNTS				20							//This is the timer count that produces 20탎
#define TIMER_20ms_VALUE			20000						//20ms = 20000탎
#define TIMER_20ms_COUNTs			156							//8Mhz w/ clk/64 prescaler * 156 = 20ms
#define MIN_SERVO_POS				1000						//1000탎 (1ms) minimum pulse width
#define MAX_SERVO_POS				2000						//2000탎 (2ms) maximum pulse width
#define MID_SERVO_POS				((MAX_SERVO_POS - MIN_SERVO_POS) / 2) + MIN_SERVO_POS
#define SERVO_STEP					TIMER_VALUE					//The resolution of the timer is what
																//the resulution of our pulse will be

#define WELCOME_MSG_DELAY			4							//Show welcome message for 4 seconds

#define LCD_UPDATE_TICS				(250000 / TIMER_20ms_VALUE)	//0.25s pause between repeats of command
#define CHANNEL_CHANGE_WAIT_TICS	(500000 / TIMER_20ms_VALUE)	//0.5s pause between repeats of command
#define STEP_WAIT_TICS				(250000 / TIMER_20ms_VALUE)	//0.25s pause between repeats of command


enum IdleStates
{
	isIDLE,
	isUPDATE_LCD,
	isFULL_CCW,
	isINC_CCW,
	isCENTER,
	isINC_CW,
	isFULL_CW,
	isINC_CHANNEL,
	isCHANNEL_CHANGE_WAIT,
	isCHANNEL_CHANGE_WAIT2,
	isSTEP_WAIT,
	isSTEP_WAIT2
};
typedef enum IdleStates IdleStates_t;

enum Buttons
{
	btFULL_CCW,
	btINC_CCW,
	btCENTER,
	btINC_CW,
	btFULL_CW,
	btINC_CHANNEL
};
typedef enum Buttons Buttons_t;


int ServoPos[NUM_SERVOS];
int ServoTimer[NUM_SERVOS];
int ServoDisplayNum;
int ServoPercent = 50;
int LCDUpdateTics;
int ChannelChangeWaitTics;
int StepWaitTics;


char ServoNames[NUM_SERVOS][MAX_SERVO_NAME_LENGTH] PROGMEM = {
	//012345678901234
	{"1. Aileron     "},
	{"2. Elevator    "},
	{"3. Throttle    "},
	{"4. Rudder      "},
	{"5. Gear        "},
	{"6. Channel 6   "},
	{"7. Channel 7   "} };
	
	
void Init();
void WelcomeMsg();
void IdleTaskStateMachine();
int  GetServoPercent(int Pos);
void Delay_ms(int ms);
void DelaySec(int Sec);
void UpdateLCD();
void SetServoPosition(int Position);
int  IsButtonPressed(Buttons_t Btn);
