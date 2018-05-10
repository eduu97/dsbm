
#include "Base.h"
#include "Keyboard.h"
#include "stm32f4xx.h"




int32_t interruptCode;
volatile int32_t switchFlag1;
volatile int32_t SF;

void initKeyboard(){
	//Bits imparells a 0 i parells a 1, per configurar com a sortida

	int32_t n;
	for(n=0; n<=3; n++){
		(KEY_PORT->MODER)=((KEY_PORT->MODER)|(BIT(n*2)));
		//SLEEP_MS(20);
		(KEY_PORT->MODER)=((KEY_PORT->MODER)&(!BIT(n*2+1)));
		//SLEEP_MS(20);
		(KEY_PORT->OTYPER)=(KEY_PORT->OTYPER)|(BIT(n));
		//SLEEP_MS(20);
	}

	for(n=6;n<=9;n++){
		(KEY_PORT->MODER)=(KEY_PORT->MODER)&(!BIT(2*n));
		//SLEEP_MS(20);
		(KEY_PORT->MODER)=(KEY_PORT->MODER)&(!BIT(2*n+1));
		//SLEEP_MS(20);
		(KEY_PORT->PUPDR)=(KEY_PORT->PUPDR)|(BIT(2*n));
		//SLEEP_MS(20);
		(KEY_PORT->PUPDR)=(KEY_PORT->PUPDR)&(!BIT(2*n+1));
		//SLEEP_MS(20);
	}
	
}


int32_t readKeyboard(void){
	int32_t code = 32;
	int n,m;

	for(n=0; n<=3; n++){
		(KEY_PORT->ODR)=(KEY_PORT->ODR)&(!BIT(n));//The n-row force '0'
		SLEEP_MS(10);
	
		for(m=6; m<=9; m++){			//for each column, find where the '0' is
			if( (KEY_PORT->IDR) == ((KEY_PORT->IDR)&(!BIT(m)))){
				code = (m-6)+4*n;						//return the code associated with the pressed button
			}
		}
		(KEY_PORT->ODR)=(KEY_PORT->ODR)|(BIT(n));
		SLEEP_MS(10);
	}
	return code;										//No button is pressed, return the initial code,32
	
}






void intConfigKeyboard(void) {

int n;

for(n=0; n<=3; n++){		//Rows force '0'

	(KEY_PORT->ODR) = (KEY_PORT->ODR)&(!BIT(n));

}

(RCC->APB2ENR) = (RCC->APB2ENR)|(RCC_APB2ENR_SYSCFGEN);	//System Configuration Controller


(SYSCFG->EXTICR[1])=(SYSCFG->EXTICR[1])|(SYSCFG_EXTICR2_EXTI6_PD)|(SYSCFG_EXTICR2_EXTI7_PD);
(SYSCFG->EXTICR[2])=(SYSCFG->EXTICR[2])|(SYSCFG_EXTICR3_EXTI8_PD)|(SYSCFG_EXTICR3_EXTI9_PD);

(EXTI->IMR) = (EXTI->IMR)|(EXTI_IMR_MR6)|(EXTI_IMR_MR7)|(EXTI_IMR_MR8)|(EXTI_IMR_MR9);		//Interrupt Mask Register

(EXTI->FTSR) = (EXTI->FTSR)|(EXTI_FTSR_TR6)|(EXTI_FTSR_TR7)|(EXTI_FTSR_TR8)|(EXTI_FTSR_TR9);	//Falling Trigger Selection Register

(EXTI->PR) = (EXTI->PR);	//Pending Register

nvicEnableVector(EXTI9_5_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI5_9_IRQ_PRIORITY));

while(1){
	switchFlag1 = 0;
	SF = switchFlag1;

	while(SF == switchFlag1);
	if((EXTI->PR) != ((EXTI->PR)|(EXTI_PR_PR5))){
		interruptCode = readKeyboard();
		LCD_SendString(interruptCode);		//Quizá haya que pasarlo a string(puntero)
	}
}
}




CH_IRQ_HANDLER(EXTI9_5_IRQHandler)
 {
 CH_IRQ_PROLOGUE();

 // Start of the ISR code

/*
• Esborrar el bit del registre Pending register (PR) per indicar que s'ha atès la interrupció.
*/
	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR6);	// An '1' is written
	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR8);
	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR7);
	EXTI->PR = (EXTI->PR)|(EXTI_PR_PR9);
	

/*
• Escriure un valor diferent de zero a la variable switchFlag.
*/

	switchFlag1 = 1;
 // End of the ISR code

 CH_IRQ_EPILOGUE();
 }



/*int32_t readKeyboard ( void ) {
 // Just to remember it ODR ( output ) and IDR ( input )

 uint32_t j, k;

 // We put all the rows to ’1 ’
 (GPIOD - >ODR) |= BIT (0) |BIT (1) |BIT (2) |BIT (3) ;

for(j=0; j <4; j++) {
 // We prove row by row searching any ’0 ’ in a



 (GPIOD - >ODR) &= (~ BIT(j) ) ;
 // We wait in order to don ’t have problems in


 DELAY_US (10) ;
 for (k=6; k <10; k++) {
 // If the column is ’0 ’ we set the


 if (!(( GPIOD - > IDR) & BIT(k) ) ) {
 (GPIOD - >ODR) |= BIT(j) ;
 // We return the value with


 return (4* j+k -6) ;
 }
 }
 // If this row does not match , we put it


 (GPIOD - >ODR) |= BIT(j) ;
 }
 return -1;
}*/
