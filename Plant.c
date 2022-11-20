// Plant.c
// Runs on TM4C123
// Provides functions operating on plant main character
// Last Modified: 5/03/2022
// Student names: Jishnu Basu & Kashif Bandali

#include <stdint.h>
#include "Plant.h"
#include "../inc/tm4c123gh6pm.h"

// Plant initialization function 
// Input: Plant struct created in main
// Output: none
void plantInit(plant_t *temp){ 
	
	temp -> x = 50;
	temp -> y = 133;
	temp -> vx = 1;
	temp -> w = 22; 
	temp -> h = 25; 
	temp -> image  = 0; 
	temp -> contact  = unhitPlant;
	temp -> state  = alivePlant;
}
