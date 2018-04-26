/**************************************************************

 i n t . c

 Interrupt test source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "int.h"      // This module header file

#include "accel.h"
#include "lcd.h"


#include "stm32f4xx.h"


/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Configures the button (PA0) ISR
// PA0 is already configured as input in the halinit() call
// This function must:
//
//    * Activate the SYSCFG clock
//
//    * Assign port A to EXTI0
//
//    * Enable the EXTI0 interrupt
//    * Activate EXTI0 on rising edge
//    * Clear the pending interrupt flag at EXTI0
//
//    * Enable EXTI0 at NVIC

volatile int switchFlag;
int32_t readValue;
char str[3];
volatile int ledOn = 0;

void interruptTest(void)
 {

	RCC->APB2ENR = (RCC->APB2ENR)|(RCC_APB2ENR_SYSCFGEN);		//Activates System Configuration Controller

	//-------------------------------------------------------------------------------

	SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0]) & (~SYSCFG_EXTICR1_EXTI0_PA); //Pin 0 can generate interruptions

	//-------------------------------------------------------------------------------

	EXTI->IMR = (EXTI->IMR)|(EXTI_IMR_MR0);		//Port A interruptions enabled
	EXTI->RTSR = (EXTI->RTSR)|(EXTI_RTSR_TR0);	//Rising trigger selection

	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR0);		//Pending interruptions delated

	nvicEnableVector( EXTI0_IRQn , CORTEX_PRIORITY_MASK(STM32_EXT_EXTI0_IRQ_PRIORITY) );


	while(1){

		switchFlag = 0;

		while(!switchFlag); // wait while switchFlag changes. When switchFlag changes, the while, breaks.

		LCD_ClearDisplay();
		LCD_GotoXY(0,0);
		LCD_SendString("Y = ");

		readValue = readAccel(0x2B, 1);	// read the Y value of the accel

		LCD_SendString(itoa(readValue, str, 10)); 	//print the LCD with the Y value
		SLEEP_MS(500);

	}
 }


/********************* ISR FUNCTIONS ******************************
Those functions should never be called directly or indirectly from
the main program. They are automatically called by the NVIC
interrupt subsystem.
*******************************************************************/

// EXTI 0 ISR
// Associated to the user button
// Tasks to do:
//    * Erase the pending interrupt flag
//    * Change the green led status
//    * Activate the flag so that the accelerometer Y axis is read
CH_IRQ_HANDLER(EXTI0_IRQHandler) {
	CH_IRQ_PROLOGUE();
		// Start of the ISR code
 /*
		• Esborrar el bit del registre Pending register (PR) per indicar que s'ha atès la interrupció.
*/
	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR0);	// An '1' is written
/*
		• Canviar l'estat del led Verd
*/
	(LEDS_PORT->ODR) = (LEDS_PORT->ODR)^(BIT(GREEN_LED_PAD));
/*
		• Escriure un valor diferent de zero a la variable switchFlag.
*/
	switchFlag = 1;
		// End of the ISR code
	CH_IRQ_EPILOGUE();
}



