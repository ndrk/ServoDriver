/*	servodriver.c
	Implementation for servo driver on Atmel ATmega16.
*/

#include "servodriver.h"

int main()
{
	Init();
	LCD_Init();

	sei();	//Enable Interrupts
	
	WelcomeMsg();
	
	while (1)
	{
		IdleTaskStateMachine();
	}
	
	return 0;
}

void Init()
{
	int i;

	PORTA = (1<<PA1)  | (1<<PA2)  | (1<<PA3)  | (1<<PA4)  | (1<<PA5);	//Turn ON internal pullups
	DDRA  = (1<<DDA0) | (1<<DDA6) | (1<<DDA7);							//Set PA0,6,7 as outputs

	PORTB = (1<<PB0)  | (1<<PB5)  | (1<<PB6)  | (1<<PB7);		//Turn ON internal pullups and Initialize outputs OFF
	DDRB  = (1<<DDB1) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4);	//Set PB1-4 as outputs, rest as inputs

	PORTC = 0x00;	//Initialize all outputs OFF
	DDRC  = 0xff;	//Set PORTC to all outputs
	
	PORTD = 0x00;	//(char) ~(1<<PD7);	//Set all as output except PD7
	DDRD  = 0xff;	//(1<<DDD7);		//Turn on internal pullup for PD7 and initialize outputs OFF
	
	//20ms Timer - Used to initiate a sequential update of all channels
	OCR0 = TIMER_20ms_COUNTs;
	TIMSK |= (1<<OCIE0);
	TCCR0 = (1<<WGM01) | (1<<CS02) | (1<<CS00);
	
	//Pulse Width Timer - Used to time the pulses
	OCR2 = TIMER_COUNTS;	//Set the TIMER2 Output Compare Register to 20 (20µs @ 8MHz clock/8)
	TIMSK |= (1<<OCIE2);	//Enable TIMER2 Compare Match Interrupt
	TCCR2 = 0;	//Disable TIMER2 - TIMER0 will enable it

	for(i=0; i<NUM_SERVOS; ++i)
	{
		ServoPos[i] = MID_SERVO_POS;	//Set all servos to 1/2 (middle)
	}
	
	ServoDisplayNum = 0;
	ServoPercent = GetServoPercent(ServoPos[ServoDisplayNum]);
}

ISR(TIMER0_COMP_vect)
{
	int i;
	
	//Calculate pulse timer count values
	for(i=0; i<NUM_SERVOS; ++i)
	{
		ServoTimer[i] = (((ServoPos[i]-1000.0)/1000.0) * 125.0) + 125.0;
	}

	//Start an update on all channels
	TCCR2 = (1<<WGM21) | (1<<CS22);	//Enable TIMER2 w/ clk/64 prescaler

	//Update counters for other timing functions
	++LCDUpdateTics;
	++ChannelChangeWaitTics;
	++StepWaitTics;
}

ISR(TIMER2_COMP_vect)
{
	static int ServoNum;
	
	PORTD = 0;	//Turn OFF all outputs
	
	//Did we just turn off the last channel?
	if(ServoNum == NUM_SERVOS)
	{
		TCCR2 = 0;
		ServoNum = 0;
		return;
	}
	
	//Start pulse on next channel
	PORTD = (1<<ServoNum);			//Turn ON output for this channel
	OCR2 = ServoTimer[ServoNum];	//Set timer for pulse width
	
	//Get ready for next channel
	++ServoNum;
}

void WelcomeMsg()
{
	char sBuf[LCD_LINE_LENGTH+1];
	
	//20 char display    01234567890123456789
	strcpy_P(sBuf, PSTR("Servo Driver  v"));
	strcat(sBuf, VERSION_NUM);
	
	LCD_write_line(sBuf, 0);

	//20 char display    01234567890123456789
	strcpy_P(sBuf, PSTR("Ryan Knowlton   2006"));
	LCD_write_line(sBuf, 1);
	
	DelaySec(WELCOME_MSG_DELAY);
	
	LCD_cmd(LCD_Clear);
}

void IdleTaskStateMachine()
{
	static IdleStates_t iState = isIDLE;
	
	switch (iState)
	{
	case isIDLE:
		if(LCDUpdateTics >= LCD_UPDATE_TICS)
		{
			LCDUpdateTics = 0;
			iState = isUPDATE_LCD;
		}
		
		else if(IsButtonPressed(btFULL_CCW))
			iState = isFULL_CCW;
		
		else if(IsButtonPressed(btINC_CCW))
			iState = isINC_CCW;
		
		else if(IsButtonPressed(btCENTER))
			iState = isCENTER;
		
		else if(IsButtonPressed(btINC_CW))
			iState = isINC_CW;
		
		else if(IsButtonPressed(btFULL_CW))
			iState = isFULL_CW;
		
		else if(IsButtonPressed(btINC_CHANNEL))
			iState = isINC_CHANNEL;
		
		break;
		
	case isUPDATE_LCD:
		UpdateLCD();
		
		iState = isIDLE;
		break;
		
	case isFULL_CCW:
		SetServoPosition(MIN_SERVO_POS);
		iState = isCHANNEL_CHANGE_WAIT;
		break;
		
	case isINC_CCW:
		SetServoPosition(ServoPos[ServoDisplayNum] - SERVO_STEP);
		iState = isSTEP_WAIT;
		break;
		
	case isCENTER:
		SetServoPosition(MID_SERVO_POS);
		iState = isCHANNEL_CHANGE_WAIT;
		break;
		
	case isINC_CW:
		SetServoPosition(ServoPos[ServoDisplayNum] + SERVO_STEP);
		iState = isSTEP_WAIT;
		break;
		
	case isFULL_CW:
		SetServoPosition(MAX_SERVO_POS);
		iState = isCHANNEL_CHANGE_WAIT;
		break;
		
	case isINC_CHANNEL:
		if(ServoDisplayNum >= (NUM_SERVOS-1))
			ServoDisplayNum = 0;
		else
			++ServoDisplayNum;
			
		ServoPercent = GetServoPercent(ServoPos[ServoDisplayNum]);
		UpdateLCD();
		
		iState = isCHANNEL_CHANGE_WAIT;
		break;		

	case isCHANNEL_CHANGE_WAIT:
		ChannelChangeWaitTics = 0;
		iState = isCHANNEL_CHANGE_WAIT2;
		break;		

	case isCHANNEL_CHANGE_WAIT2:
		if(ChannelChangeWaitTics >= CHANNEL_CHANGE_WAIT_TICS)
			iState = isIDLE;
			
		break;

	case isSTEP_WAIT:
		StepWaitTics = 0;
		iState = isSTEP_WAIT2;
		break;
		
	case isSTEP_WAIT2:
		if(StepWaitTics >= STEP_WAIT_TICS)
			iState = isIDLE;
			
		break;
	}
}

int GetServoPercent(int Pos)
{
	float fPos;
	
	fPos = (float) Pos;
	
	fPos = ((2.0/10.0) * (fPos - 1000.0)) - 100.0;
	
	return (int) fPos;
}

void Delay_ms(int ms)
{
	int i;
	
	for(i=0; i<ms; ++i)
		_delay_ms(1);
}

void DelaySec(int Sec)
{
	int i;
	int Loops;

	Loops = Sec * 31;
 
	for(i=0; i<Loops; ++i)
		_delay_ms(32);
}

void UpdateLCD()
{
	char sBuf[LCD_LINE_LENGTH+2];

	//LCD_cmd(LCD_Clear);	//Removed to avoid flicker
	
	//01234567890123456789
	//Throttle       -100%
	strcpy_P(sBuf, ServoNames[ServoDisplayNum]);
	
	if(ServoPercent != 0)
		sprintf(&sBuf[MAX_SERVO_NAME_LENGTH-1], "%+4d%%", ServoPercent);
	else
		sprintf(&sBuf[MAX_SERVO_NAME_LENGTH-1], "%4d%%", ServoPercent);
	
	LCD_write_line(sBuf, 0);
	
	//01234567890123456789
	//Pulse Width = 1500µs
	sprintf(sBuf, "Pulse Width   %4däs", ServoPos[ServoDisplayNum]);	//NOTE: 'ä' shows as 'µ'
	LCD_write_line(sBuf, 1);
}

void SetServoPosition(int Position)
{
	ServoPos[ServoDisplayNum] = Position;
	ServoPercent = GetServoPercent(ServoPos[ServoDisplayNum]);
	UpdateLCD();
}

int  IsButtonPressed(Buttons_t Btn)
{
	int bReturn;
	
	switch (Btn)
	{
	case btFULL_CCW:
		bReturn = !(PINB & (1<<PB0));
		break;
		
	case btINC_CCW:
		bReturn = !(PINA & (1<<PA1));
		break;
	
	case btCENTER:
		bReturn = !(PINA & (1<<PA2));
		break;
	
	case btINC_CW:
		bReturn = !(PINA & (1<<PA3));
		break;
	
	case btFULL_CW:
		bReturn = !(PINA & (1<<PA4));
		break;
	
	case btINC_CHANNEL:
		bReturn = !(PINA & (1<<PA5));
		break;
	
	default:
		bReturn = FALSE;
		break;
	}
	
	return bReturn;
}
