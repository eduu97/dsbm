void initAccel(void) {
//Habilitar rellotge del perifèric corresponent, en aquest cas
l'SPI1
RCC -> APB2ENR |= (1 << 12);
SPI1 -> CR1 &= 0xFC80; // "1111 1100 1000 0000"
SPI1 -> CR1 |= 0x035F; // "0000 0011 0101 1111"

writeAccel(0x20, 0x47);
// Prova de lectura del valor dels eixos de l'acceleròmetre
int x, y;
char string;
x = readAccel(0x29, 0);
itoa(x, &string, 16);
LCD_SendString(&string);
y = readAccel(0x2B, 1);
}


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
void initAccel(void)
 {

	RCC->APB2ENR = (RCC->APB2ENR)|(RCC_APB2ENR_SPI1EN);		//2^12 bit SPI1 EN (constant defined)

	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_CPOL;		//Inactive line for high level
	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_CPHA;		//Data is read with the second flank of the clock

	SPI1->CR1 = (SPI1->CR1)&(~SPI_CR1_DFF);		//8 bits format

	SPI1->CR1 = ((SPI1->CR1)&(~ SPI_CR1_BR))|SPI_CR1_BR_0|SPI_CR1_BR_1;	//n = 3 -> f/16 = 84/16 = 5.25MHz

	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_SSM;		//nCs input mode
	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_SSI;		//Software management -> '1'


	writeAccel(0x20,0x47);	//PD = 1, X,Y,Z = 1

	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_SPE;		//Peripheral enabled
	SPI1->CR1 = (SPI1->CR1)|SPI_CR1_MSTR;		//Controller is Master


	DELAY_US(3000);


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

int32_t readAccel(int32_t reg,int32_t sign) {

	uint8_t  val;      // Read value
	int8_t *psigned;  // Pointer to signed value

	 // Limit the register number to 6 bits 0..63
	 reg&=0x3B;		//Depends on the accelerometer version

	 // Although it is not mandatory for read,
	 // we verify its is not a reserved register
	 if (reg<0x0F) return 1001;
	 if ((reg<0x20)&&(reg>0x0F)) return 1001;
	 if ((reg<0x27)&&(reg>0x23)) return 1001;
	 if ((reg<0x30)&&(reg>0x2D)) return 1001;

	 // Activates Chip Select
	 LIS_CS_PORT->BSRR.H.clear=LIS_CS_BIT;

	 // Creates the command
	 //  6 low bits: Register to read
	 //      bit 6 : 0 Disabe autoincrement
	 //      bit 7 : 1 Enable autoincrement
	 reg|=BIT(7);

	 // Send the command
	 SPI1->DR=reg;

	 // Small delay before reading the buffer
	 DELAY_US(2);

	 // Wait to the receiver buffer to fill
	 while (!((SPI1->SR) & BIT0));

	 // Empty the receiver buffer
	 // We don't need the returned information
	 val=(SPI1->DR);

	 // Write dummy data to obtain a response
	 SPI1->DR=0xFF;

	 // Small delay before reading the buffer
	 DELAY_US(2);

	 // Wait to the receiver buffer to fill
	 while (!((SPI1->SR) & BIT0));

	 // Recover the response in the reception buffer
	 val=(SPI1->DR);

	 // Deactivates Chip Select
	 LIS_CS_PORT->BSRR.H.set=LIS_CS_BIT;

	 // The obtained data will be uint8 (0..255)

	 // Return the read value if sign is false
	 if (!sign) return val;

	 // If the data is signed we need to return in two's complement

	 // Point to the returned value
	 psigned=&val;

	 // Return as signed value
	 return *psigned;
 }


int32_t writeAccel(int32_t reg,int32_t data){

	 // Limit the register number to 6 bits 0..63
	 reg&=0x3B;

	 // Although it is not mandatory for read,
	 // we verify its is not a reserved register
	 if (reg<0x0F) return 1001;
	 if ((reg<0x20)&&(reg>0x0F)) return 1001;
	 if ((reg<0x27)&&(reg>0x23)) return 1001;
	 if ((reg<0x30)&&(reg>0x2D)) return 1001;

	 LIS_CS_PORT->BSRR.H.clear=LIS_CS_BIT;

	 reg&=(!BIT(7));

   	 SPI1->DR=reg;

	 while (!((SPI1->SR) & BIT0));

	  // Empty the receiver buffer
	  // We don't need the returned information
	  val=(SPI1->DR);

	  SPI1->DR=data;		//PD = 1, X,Y,Z = 1

	  // Small delay before reading the buffer
	  DELAY_US(2);

	  // Wait to the receiver buffer to fill
	  while (!((SPI1->SR) & BIT0));

	  // Recover the response in the reception buffer
	  val=(SPI1->DR);

	  // Deactivates Chip Select
	  LIS_CS_PORT->BSRR.H.set=LIS_CS_BIT;

	  return 1;
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
char *itoa(int32_t num, char *str, int32_t radix){
	 int32_t sign = 0;   // To remember the sign if negative
	 int32_t pos = 0;    // String position
	 int32_t i;          // Generic use variable

	 //Save sign
	 if (num < 0){
		   sign = 1;
		   num = -num;
	 }
	 //Construct a backward string of the number.
	 do {
		i = num % radix;
		if (i < 10){
			 str[pos] = i + '0';
		}else{
			 str[pos] = i - 10 + 'A';
		}
		pos++;
		num /= radix;
	 }while (num > 0);

	 //Now add the sign
	 if (sign){
		   str[pos] = '-';
	 }else{
		   pos--;
	 }

	 // Add the final null
	  str[pos+1]=0;

	 // Pos is now the position of the final digit (before null)

	 // Now reverse the string
	 i=0;
	 do{
		  sign=str[i];
		  str[i++]=str[pos];
		  str[pos--]=sign;
	  }while(i<pos);

	 return str;
 }

