// Plant.h
// Runs on TM4C123
// Provides functions operating on plant main character
// Last Modified: 5/03/2022
// Student names: Jishnu Basu & Kashif Bandali

#ifndef Plant_H
#define Plant_H
#include <stdint.h>

typedef enum contactPlant {hitPlant, unhitPlant} contactPlant_t;
typedef enum statePlant {alivePlant, deadPlant} statePlant_t;

typedef struct plant {
	int8_t x;
	uint8_t y;
	int8_t vx;
	uint8_t w;
	uint8_t h;
	uint8_t image;
	contactPlant_t contact;
	statePlant_t state;
} plant_t;

// Plant initialization function 
// Input: Plant struct created in main
// Output: none
void plantInit(plant_t *temp);

#endif
