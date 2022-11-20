// Coin.h
// Runs on TM4C123
// Provide functions operating on coins
// Last Modified: 5/03/2022
// Student names: Jishnu Basu & Kashif Bandali

#ifndef Coin_H
#define Coin_H
#include <stdint.h>

typedef enum contactCoin {hitCoin, unhitCoin} contactCoin_t;
typedef enum stateCoin {aliveCoin, deadCoin} stateCoin_t;

typedef struct coin {
	int8_t x;
	int16_t y;
	int8_t vy;
	uint8_t w;
	uint8_t h;
	uint8_t image;
	contactCoin_t contact;
	stateCoin_t state;
} coin_t;

// Coin initialization function 
// Input: Coin struct created in main
// Output: none
void coinInit(coin_t *temp);

#endif
