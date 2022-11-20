// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 4/12/2022
// Student names: Jishnu Basu
// Last modification date: 4/12/2022
// Labs 8 and 9 specify PD2
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function using PD2 
// Input: none
// Output: none
void ADC_Init(void){ 

		volatile uint32_t delay; //Used in time wasting applications
		
		SYSCTL_RCGCGPIO_R |= 0x08; //Activates clock for Port D
		while((SYSCTL_PRGPIO_R&0x08) == 0) {}; //Time wasting
		GPIO_PORTD_DIR_R &= (~0x04);
		GPIO_PORTD_AFSEL_R |= 0x04;
		GPIO_PORTD_DEN_R &= (~0x04);
		GPIO_PORTD_AMSEL_R |= 0x04;
			
		SYSCTL_RCGCADC_R |= 0x01; //Activates ADC0
		delay = SYSCTL_RCGCADC_R;
		delay = SYSCTL_RCGCADC_R;
		delay = SYSCTL_RCGCADC_R;
		delay = SYSCTL_RCGCADC_R;
		ADC0_PC_R |= 0x01; //Configures for 125K
		ADC0_SSPRI_R = 0x0123; //Seq3 is highest priority
		ADC0_ACTSS_R &= (~0x0008); //Disable Seq3
		ADC0_EMUX_R &= (~0xF000); //Seq3 is software triggered
		ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 5; //Sets ADC to take input from channel 5
		ADC0_SSCTL3_R  = 0x0006; //Disable temp sensor, notify on complete, indicate single sample in sequence, denote single-ended signal mode
		ADC0_IM_R &= (~0x0008); //Disable SS3 interrupts
		ADC0_ACTSS_R |= 0x0008; //Enable sample sequencer 3
		ADC0_SAC_R = 5;
}
//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	// 1) initiate SS3
  // 2) wait for conversion done
  // 3) read result
  // 4) acknowledge completion
	
	uint32_t data;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R & 0x08) == 0) {}
	data = ADC0_SSFIFO3_R & 0xFFF;
	ADC0_ISC_R = 0x08;
  return data;
}
