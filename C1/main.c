#include "Base.h"     // Basic definitions
#include "lcd.h"
#include "Keyboard.h"


char str1[] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
int currentPos;


int main(void){

	baseInit();
	LCD_Init();
	initKeyboard();
	LCD_ClearDisplay();
	
	while(1){
	

		currentPos = readKeyboard();
		if(currentPos != 32){
			LCD_GotoXY(currentPos,0);
			LCD_SendChar(str1[currentPos]);
			SLEEP_MS(2000);
			LCD_ClearDisplay();
		}else{
			LCD_ClearDisplay();
		}
	}

}

/*
int main(void){

	baseInit();
	LCD_Init();
	LCD_ClearDisplay();
	initKeyboard();
	intConfigKeyboard();

return 0;

}
*/

/*
int main(void){

	baseInit();
	LCD_Init();
	initKeyboard();
	LCD_ClearDisplay();
	int i, var;
	
	while(1){
	

		currentPos = readKeyboard();
		if(currentPos != 32){
			LCD_ClearDisplay();
			for(i=15; i>=0; i--){
				if((currentPos&BIT(i)) !=0);{
				var = str1[15-i];
				LCD_SendChar(var);
				}
			}

		}else{
			LCD_ClearDisplay();
		}
	}

}
*/
