#include "os.h"
#include "TM4C123GH6PM.h"

enum LED_COLOR {
	dark = 0,
	red = 0x02,
	blue = 0x04,
	green = 0x08,
	yellow = 0x0A,
	sky_blue = 0x0C,
	white = 0x0E,
	pink = 0x06
};

typedef enum {
	true = 1,
	false = 0
} bool;


void PortF_Init(void);
void setLedColor(unsigned int color);
void delay(void);
bool isEnableSW1(void);
bool isEnableSW2(void);
void blink(void);
void BlinkLedTask(void* sleepTime);
void exitBlinkedTask(void);

int time_sleep = 100000;
OS_ERR error;
OS_TCB task1TCB;
OS_TICK task1Tick;
CPU_STK beginStackAddr = 0x20000000; 

int main()
{
	OSInit(&error);
	
	OSTaskCreate(
								&task1TCB,
								"Cpu1", 
								BlinkLedTask,
								&time_sleep, 
								1,
								&beginStackAddr,
								100,
								100,
								100,
								task1Tick,
								exitBlinkedTask,
								OS_OPT_TASK_NONE,
								&error);
	OSStart(&error);
	return 0;
}

// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
void BlinkLedTask(void* sleepTime){    

	PortF_Init();
  while(1){
		if(isEnableSW1())
			blink();
		else
			setLedColor(yellow);
  }
}
// Subroutine to initialize port F pins for input and output
// PF4 is input SW1 and PF2 is output Blue LED
// Inputs: None
// Outputs: None
// Notes: ...
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL->RCGC2 |= 0x00000020;     // 1) F clock
  delay = SYSCTL->RCC2;           // delay   
  GPIOF->LOCK = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIOF->CR = 0x1F;           // allow changes to PF4-0       
  GPIOF->AMSEL = 0x00;        // 3) disable analog function
  GPIOF->PCTL = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIOF->DIR = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIOF->AFSEL = 0x00;        // 6) no alternate function
  GPIOF->PUR = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIOF->DEN = 0x1F;          // 7) enable digital pins PF4-PF0        
}

void setLedColor(unsigned int color)
{
	GPIOF->DATA = color;
}

bool isEnableSW1(void)
{
	bool result = GPIOF->DATA&0x01;
	return !result;
}

bool isEnableSW2(void)
{
	bool result = GPIOF->DATA&0x10;
	return !result;
}

void delay()
{
	int count = 1000000;
	while(count > 0)
		count -= 1;
}

void blink(void)
{
	setLedColor(dark);
	delay();
	setLedColor(red);
	delay();
	setLedColor(blue);
	delay();
	setLedColor(green);
	delay();
	setLedColor(yellow);
	delay();
	setLedColor(sky_blue);
	delay();
	setLedColor(white);
	delay();
	setLedColor(pink);
	delay();
}

void exitBlinkedTask(void)
{
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06