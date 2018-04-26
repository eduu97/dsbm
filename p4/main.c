/*
 * intMain.c
 *
 *  Created on: 10 dic. 2017
 *      Author: sergi
 */


#include "Base.h"     // Basic definitions
#include "lcd.h"
#include "accel.h"
#include "int.h"



int main(void){

	baseInit();
	LCD_Init();
	initAccel();


	interruptTest();
}
