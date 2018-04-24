/**************************************************************

 a c c e l . h

 Accelerometer module source file

 ***************************************************************/

#include "Base.h"    // Basic definitions
#include "accel.h"   // Header file for this module
// Some functions need to be implemented as indicated on P3

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
 ******************************************************************/

// Init the accelerometer SPI bus

void writeAccel(int32_t reg, int32_t data) {



	// Although it is not mandatory for read,
	// we verify its is not a reserved register
	if (reg < 0x0F)
		return;
	if ((reg < 0x20) && (reg > 0x0F))
		return;
	if ((reg < 0x27) && (reg > 0x23))
		return;
	if ((reg < 0x30) && (reg > 0x2D))
		return;

	// return if we try to write in read - only registers
	// outputs x, y and z
	//if ((reg == 0x29) || (reg == 0x2B) || (reg == 0x2D))	//AQUI ESTAVA EL PUTU ERROR, L'HEM ARREGLAR A BASE DE POTRA :)

		//return;

	// Activates Chip Select
	LIS_CS_PORT->BSRR.H.clear = LIS_CS_BIT;

	// Posem el bit RW a 0 per indicar que es una escriptura.
	//reg &= (~BIT(7)); //Crec que en default és 0 per tant alumillor no fa falta aquesta sentencia

	// Send the command
	SPI1->DR = reg;

	// Small delay before reading the buffer
	DELAY_US(2);

	// Wait to the receiver buffer to fill
	while (!((SPI1->SR) & BIT0))
		;

	// Empty the receiver buffer
	// We don't need the returned information
//	val = (SPI1->DR);		//Alumillor ens ho podem estalviar

	// Write data to obtain a response		// OJO
	SPI1->DR = data;

	// Small delay before reading the buffer
	DELAY_US(2);

	// Wait to the receiver buffer to fill
	while (!((SPI1->SR) & BIT0))
		;

	// Recover the response in the reception buffer
	//val = (SPI1->DR);

	// Deactivates Chip Select
	LIS_CS_PORT->BSRR.H.set = LIS_CS_BIT;

	return;
}


void initAccel(void) {

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Configure the CR1 register
	// Configure the CPOL and CPHA registers
	SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;

	// Configure the DFF
	SPI1->CR1 &= (~SPI_CR1_DFF);

	// Configure the BR [2:0]
	SPI1->CR1 = (SPI1->CR1 | SPI_CR1_BR) & (~SPI_CR1_BR_2);

	// Configure the SSM i SSI
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

	// Activate SPE i MSTR in CR1 register
	SPI1->CR1 |= SPI_CR1_SPE | SPI_CR1_MSTR;

	GPIOE->MODER |= BIT(6);
	GPIOE->BSRR.H.set = BIT(3);

	writeAccel(0x20,0x47);

	/*
	 // Habilitar el rellotge del perifèric SPI1 del bus APB2
	 RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);

	 //Habilitar el CPHA i CPOL que són el començament del flanc i el tipus de flanc.
	 SPI1 -> CR1 |=  SPI_CR1_CPHA;
	 SPI1 -> CR1 |= SPI_CR1_CPOL;

	 //Posem el DFF en 0 per tenir-lo en baix nivell
	 SPI1-> CR1 &= (~SPI_CR1_DFF);

	 //Activem la fclock adequada trobada a EP1
	 SPI1 ->CR1 = ((SPI1->CR1)&(~SPI_CR1_BR))|(SPI_CR1_BR_1);//0 o 1
	 SPI1 ->CR1 = ((SPI1->CR1)&(~SPI_CR1_BR))|(SPI_CR1_BR_2);//1 o 2// "~" alt gr +4

	 //Activem el SSM i SSI
	 SPI1->CR1 |= SPI_CR1_SSM;
	 SPI1->CR1 |= SPI_CR1_SSI;

	 //De moment el pin PE3 no l'activarem

	 //Ara activem el SPE i MSTR que s'han d'activar els últims.
	 SPI1->CR1 |= SPI_CR1_MSTR;
	 SPI1->CR1 |= SPI_CR1_SPE;
	 */

	/*
	 0x20C7 - Write value 0xC7 into register 0x20 (CTRL_REG1)
	 0xAB00 - Read value from register 0x2B (OUT_Y)

	 0x00 (WRITE) + 0x20 (CTRL_REG1) = 0x20
	 0x80 (READ) + 0x2B (OUT_Y) = 0xAB

	 The high order byte is the Command, and Read/Write direction,
	 the low order byte is the value to Write if sending data to a register.

	 private const byte CTRL_REG1_ADDR=0x20;
	 private const byte OUT_X_ADDR=0x29;
	 private const byte OUT_Y_ADDR =0x2B;
	 private const byte OUT_Z_ADDR=0x2D;

	 */
	// This code is requested in  P3.3 i P3.5
}

/********** PUBLIC FUNCTIONS ALREADY IMPLEMENTED ***************
 Those functions are provided to reduce the codding effort
 for the accelerometer module
 ****************************************************************/

// Read an accelerometer register
//      reg   : Number of the register to read
//      sign  : If true, will read as int8_t (-128 a 127)
//              If false, will read as uint8_t (0 a 255)
//
// In case of error returns numbers greater than 1000
//             1001 : Try to access a reserved register
int32_t readAccel(int32_t reg, int32_t sign) {
	int8_t val;      // Read value
	int8_t *psigned;  // Pointer to signed value

	// Limit the register number to 6 bits 0..63

	reg &= 0x3F;

	// Although it is not mandatory for read,
	// we verify its is not a reserved register
// if (reg<0x0F) return 1001;
	if ((reg < 0x20) && (reg > 0x0F))
		return 1001;
	if ((reg < 0x27) && (reg > 0x23))
		return 1001;
	if ((reg < 0x30) && (reg > 0x2D))
		return 1001;

	// Activates Chip Select
	LIS_CS_PORT->BSRR.H.clear = LIS_CS_BIT;

	// Creates the command
	//  6 low bits: Register to read
	//      bit 6 : 0 Disabe autoincrement
	//      bit 7 : 1 Enable autoincrement
	reg |= BIT(7);

	// Send the command
	SPI1->DR = reg;

	// Small delay before reading the buffer
	DELAY_US(2);

	// Wait to the receiver buffer to fill
	while (!((SPI1->SR) & BIT0))
		;

	// Empty the receiver buffer
	// We don't need the returned information
	val = (SPI1->DR);

	// Write dummy data to obtain a response
	SPI1->DR = 0xFF;

	// Small delay before reading the buffer
	DELAY_US(2);

	// Wait to the receiver buffer to fill
	while (!((SPI1->SR) & BIT0))
		;

	// Recover the response in the reception buffer
	val = (SPI1->DR);

	// Deactivates Chip Select
	LIS_CS_PORT->BSRR.H.set = LIS_CS_BIT;

	// The obtained data will be uint8 (0..255)

	// Return the read value if sign is false
	if (!sign)
		return val;

	// If the data is signed we need to return in two's complement

	// Point to the returned value
	psigned = &val;

	// Return as signed value
	return *psigned;
}



// Converts from integer to string using the indicated radix
// Modified from:
// https://www.fooe.net/trac/llvm-msp430/browser/trunk/mspgcc/msp430-libc/src/stdlib/itoa.c
// Sign is considered for all radix

// Integer to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
char *itoa(int32_t num, char *str, int32_t radix) {
	int32_t sign = 0;   // To remember the sign if negative
	int32_t pos = 0;    // String position
	int32_t i;          // Generic use variable

	//Save sign
	if (num < 0) {
		sign = 1;
		num = -num;
	}

	//Construct a backward string of the number.
	do {
		i = num % radix;
		if (i < 10)
			str[pos] = i + '0';
		else
			str[pos] = i - 10 + 'A';
		pos++;
		num /= radix;
	} while (num > 0);

	//Now add the sign
	if (sign)
		str[pos] = '-';
	else
		pos--;

	// Add the final null
	str[pos + 1] = 0;

	// Pos is now the position of the final digit (before null)

	// Now reverse the string
	i = 0;
	do {
		sign = str[i];
		str[i++] = str[pos];
		str[pos--] = sign;
	} while (i < pos);

	return str;
}
/*
 void visualInclination(void) {

 int32_t x, y, min, max, posx, posy, antx, anty, N;
 uint32_t i;
 N = 100;
 // Min and max are the values expected for 45 and -45 degrees

 min = -39;
 max = 39;

 while (1) {
 x = 0;
 y = 0;
 // We make the N samples average of x and y to avoid noise

 for (i = 0; i < N; i++) {
 x += readAccel(0x29, 1);
 y += readAccel(0x2B, 1);
 }
 x = x / N;
 y = y / N;

 /* We make a data transformation to have a integer value between 0 and 15 being 0
 the value asociated to -45 degrees and 15 the asociated to 45 degrees .
 We always avoid going out the range [0 ,15].

 posx = 15 * (x - min) / (max - min);
 if (posx < 0)
 posx = 0;
 else if (posx > 15)
 posx = 15;
 posy = 15 * (y - min) / (max - min);
 if (posy < 0)
 posy = 0;
 else if (posy > 15)
 posy = 15;

 /* We erase the (*) characters and set them
 in their corresponding position . Then we
 actualize the positions.

 LCD_GotoXY(antx, 0);
 LCD_SendChar(' ');
 LCD_GotoXY(anty, 1);
 LCD_SendChar(' ');
 LCD_GotoXY(posx, 0);
 LCD_SendChar('*');
 LCD_GotoXY(posy, 1);
 LCD_SendChar('*');
 antx = posx;
 anty = posy;
 }
 }*/



