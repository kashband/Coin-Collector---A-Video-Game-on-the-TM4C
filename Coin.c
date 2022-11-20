// Coin.c
// Runs on TM4C123
// Provide functions operating on coins
// Last Modified: 5/03/2022
// Student names: Jishnu Basu & Kashif Bandali

#include <stdint.h>
#include "Coin.h"
#include "Random.h"
#include "../inc/tm4c123gh6pm.h"

// Coin initialization function 
// Input: Coin struct created in main
// Output: none
void coinInit(coin_t *temp){ 
	
	temp -> x = Random()%112;
	temp -> y = 0;
	temp -> vy = 1;
	temp -> w = 15; 
	temp -> h = 22; 
	temp -> image  = 0; 
	temp -> contact  = unhitCoin;
	temp -> state  = aliveCoin;
}
