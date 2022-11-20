// DynamiteDodger.c
// Runs on TM4C123
// Jishnu Basu and Kashif Bandali
// Last Modified: 5/3/2022 

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#include "Plant.h"
#include "Coin.h"
#define coinMax 4

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void drawScreen(void); //Function to draw screen
void Delay1msTest(uint32_t n); //Delay 1ms function
void printUpdate(void); // Updates points, powerups, and timer values on screen
void initScreen(void); //Initializes Screen
uint32_t Convert(uint32_t x); // Convert function for ADC

const unsigned short *plantImages[4] = {mainPlantOpenBuffer, mainPlantOpenBuffer, mainPlantClosedBuffer, mainPlantClosedBuffer};
const unsigned short *coinImages[8] = {coin1, coin1, coin2, coin2, coin3, coin3, coin2, coin2};

uint8_t flag = 0; //Global flag to keep track of ADC sampling
plant_t mainPlant; //Global struct for plant
uint32_t points = 0; //Global variable for points
uint32_t time = 120; //Global variable for time in seconds
uint32_t powerUps = 0; //Global variable for powerUps
coin_t coinArray[coinMax]; //Array holding our coin structs

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

int mainGiven(void){ 
	char l;
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black 
	//ST7735_DrawBitmap(25, 159, mainPlantClosed, 18, 25);
	for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  //Delay100ms(30);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    //Delay100ms(20);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }  
}

void SysTick_Init(uint32_t period)
{
  //DisableInterrupts(); Done through main program
	NVIC_ST_CTRL_R = 7;
	NVIC_SYS_PRI3_R = 0x40000000; //Set priority to 2; May need to be adjusted
	NVIC_ST_RELOAD_R = period;
	NVIC_ST_CURRENT_R = 0;
	//EnableInterrupts(); Done through main program
}

void SysTick_Handler(void)
{
  //GPIO_PORTF_DATA_R ^= 0x04; Go back and use with Texas display to check frequency
	uint32_t temp = ADC_In();
	temp = Convert(temp); //temp is where plant should be on screen
	if(mainPlant.x > temp)
		mainPlant.x = mainPlant.x - mainPlant.vx; //Means move plant left
  else if (mainPlant.x < temp)
		mainPlant.x = mainPlant.x + mainPlant.vx;; //Means move plant right
	mainPlant.image = (mainPlant.image + 1) % 4;
	flag = 1;
}

uint32_t Convert(uint32_t x){
  return (x*105)/4096; //x-position bounds will be from 0 to 105
}

// Draws Screen
// Inputs & Outputs: None
void drawScreen()
{
	for(int i = 0; i < coinMax; i++)
	{
		if(coinArray[i].state == aliveCoin) //Updates all coins
		{
			ST7735_DrawBitmap(coinArray[i].x, coinArray[i].y, coinImages[coinArray[i].image], coinArray[i].w, coinArray[i].h); //Drawing coin
			coinArray[i].y = coinArray[i].y + coinArray[i].vy; //Update position
			coinArray[i].image = (coinArray[i].image + 1) % 8; //Updates image
		}
		if(coinArray[i].y > 132) //Kills coin if hits ground
		{
			  coinArray[i].state = deadCoin;
			  ST7735_FillRect(coinArray[i].x, coinArray[i].y - 25, 22, 25, ST7735_Color565(0, 0, 0)); //Draws black rectangle
				coinArray[i].y = 0; //Resets to top of screen
				coinArray[i].x = Random()%112; //Resets to random x position
		}
	}
}

void Delay1msTest(uint32_t n)
{
	uint32_t volatile time;
  while(n){
    time = 72724*2/91;  // 1msec, tuned at 80 MHz
    while(time){
	  	time--;
    }
    n--;
  }
}

// Updates points, powerups, and timer values on screen
// Inputs & Outputs: None
void printUpdate(void)
{
  uint8_t temp;
	
	ST7735_SetCursor(8, 15);
	LCD_OutDec(points);
	
	ST7735_SetCursor(19, 15);
	LCD_OutDec(points);
	
	ST7735_SetCursor(11, 14);
	LCD_OutDec(time/60);
	ST7735_OutString(":");
	temp = time%60;
	LCD_OutDec(temp);
	LCD_OutDec(temp%10);
}

// Initializes Screen
// Inputs & Outputs: None
void initScreen(void)
{
	ST7735_FillScreen(0x0000); // Set screen to black
	
	ST7735_SetCursor(0, 15);
	ST7735_OutString("Points:"); //Prints text
	ST7735_SetCursor(10, 15);
	ST7735_OutString("Powerups:"); //Prints text
	ST7735_SetCursor(5, 14);
	ST7735_OutString("Time:"); //Prints text
	printUpdate(); //Prints all initial values
	
	ST7735_FillRect(0, 135, 160, 1, ST7735_Color565(0, 255, 0)); //Draw horizontal line
	ST7735_DrawBitmap(mainPlant.x, mainPlant.y, plantImages[mainPlant.image], mainPlant.w, mainPlant.h);
}

int main(void) //Tester main; Need to write heartbeat initializations; points/time/powerups are globals initialized at compile
{ 
	DisableInterrupts();
  TExaS_Init(NONE); // Bus clock set 80 MHz 
  Output_Init(); //Initializes LCD
	SysTick_Init(4000000); //Initalizes SysTick for ADC sampling
	ADC_Init();
	Random_Init(1);
	plantInit(&mainPlant);
	for(int i = 0; i < coinMax; i++) //Initializes all coins
	{
		coinInit(&coinArray[i]);
	}
	initScreen();
	EnableInterrupts();
	
	while(1) //Creates infinite loop to allow ADC testing
	{ 
		drawScreen();
		Delay1msTest(100);
		if(flag == 1)
		{
			ST7735_DrawBitmap(mainPlant.x, mainPlant.y, plantImages[mainPlant.image], mainPlant.w, mainPlant.h);
			flag = 0;
		}
	}
}
