/*
 * accelMain.c
 *
 *  Created on: 9 dic. 2017
 *      Author: sergi
 */


#include "Base.h"     // Basic definitions
#include "lcd.h"
#include "accel.h"



int32_t meanValue(int32_t N, int32_t reg){
	int32_t i;
	int32_t result=0;
	for(i=0;i<N;i++){
		int32_t var = readAccel(reg,0);
		result=result+(var/N);
	}
	return result;
}

void initDisplay(void){

	LCD_Config(1,0,0);		//hide the cursor

	LCD_ClearDisplay();

	LCD_GotoXY(7,0);
	LCD_SendChar('*');
	LCD_GotoXY(7,1);
	LCD_SendChar('*');

}

void refreshDisplay(void){

	int32_t centralXPos = meanValue(10,0x29);
	int32_t centralYPos = meanValue(10,0x2B);

	SLEEP_MS(200);


	while(1){

		LCD_ClearDisplay();

		LCD_GotoXY(0,0);
		LCD_SendChar('X');

		LCD_GotoXY(0,1);
		LCD_SendChar('Y');


		SLEEP_MS(10);
		int32_t readXVal = meanValue(10,0x29);
		int32_t readYVal = meanValue(10,0x2B);

		LCD_GotoXY(8+(2*(readXVal - centralXPos)*16)/256, 0);
		LCD_SendChar('*');

		LCD_GotoXY(8+(2*(readYVal - centralYPos)*16)/256, 1);
		LCD_SendChar('*');

	}

}


int main(void){

	baseInit();
	LCD_Init();
	initAccel();
	initDisplay();

	refreshDisplay();	//Refresh the x'*' and y'*' positions with the mean value of N samples.
	DELAY_US(100);

	return 0;
}


int main(void)
{

	baseInit();
	LCD_Init();
	initAccel();
	LCD_ClearDisplay();

	char str[6];
	int32_t readValue = readAccel(0x0F,0);		//Shows 'who i am' response
	itoa(readValue, str, 16);

	LCD_SendString(str);

	while(1);
	return 0;
}


int main(void){

	baseInit();
	LCD_Init();
	initAccel();
	LCD_ClearDisplay();


	char str[6];
	while(1){
		itoa(readAccel(0x29,1), str, 10);	//Refresh OutX value
		LCD_SendString(str);
		SLEEP_MS(500);
		LCD_ClearDisplay();

	}

return 0;

}

int main(void){ while(true){

	 char string = ' ';
	 LCD_SendString("X:");
	 LCD_SendString(itoa( readAccel (0x29,1),&string,10));
	 LCD_SendString("Y:");
	 LCD_SendString(itoa( readAccel (0x2B,1),&string,10));
	 LCD_SendString("Z:");
	 LCD_SendString(itoa( readAccel (0x2D,1),&string,10));
	 SLEEP_MS(200);
	 LCD_GotoXY(0,0);
	// LCD_ClearDisplay();

 }
