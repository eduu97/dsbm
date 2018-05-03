

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
