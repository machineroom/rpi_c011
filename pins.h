/*
Definition of C011 <> RPI GPIO pins
bcm2835 lib uses 40 pin J8 connector pin numbering
*/

#include <bcm2835.h>
//				GPIO pin		           		| GPIO bit		| 3V3/5V cnv (chip/pin)		| C011 mode2 pin
#define D0		RPI_V2_GPIO_P1_03	//BLACK		| 2				| 2/A1						| 24
#define D1		RPI_V2_GPIO_P1_05	//BROWN 	| 3				| 2/A2						| 23
#define D2		RPI_V2_GPIO_P1_07	//RED		| 4				| 2/A3						| 22
#define D3		RPI_V2_GPIO_P1_29	//ORANGE	| 5				| 2/A4						| 8
#define D4		RPI_V2_GPIO_P1_31	//YELLOW	| 6				| 2/A5						| 20
#define D5		RPI_V2_GPIO_P1_26	//GREEN		| 7				| 2/A6						| 10
#define D6		RPI_V2_GPIO_P1_24	//BLUE		| 8				| 2/A7						| 18
#define D7		RPI_V2_GPIO_P1_21	//VIOLET	| 9				| 2/A8						| 12

#define RS0		RPI_V2_GPIO_P1_18	//GREY		| 24			| 1/A1						| 5
#define RS1		RPI_V2_GPIO_P1_19	//WHITE		| 10			| 1/A2						| 6
#define RESET	RPI_V2_GPIO_P1_13	//YELLOW	| 27			| 1/A3						| 13
#define CS		RPI_V2_GPIO_P1_22	//GREEN		| 25			| 1/A4						| 25
#define RW		RPI_V2_GPIO_P1_23	//BLUE		| 11			| 1/A5						| 3

#define IN_INT	RPI_V2_GPIO_P1_32	//BROWN		| 12			| 1/A6						| 26
#define OUT_INT	RPI_V2_GPIO_P1_33	//WHITE		| 13			| 1/A7						| 4

// BYTE controls the BYTE signal of the HSL cards. HIGH=byte mode, LOW=word mode
#define BYTE    RPI_V2_GPIO_P1_38	//BLUE		| 20			| 1/A8						| to system controller


//									BLACK		GND				| */GND						| 14,11,16
//									ORANGE		3V3				| */VA,*/OE
//									RED			5V				| */VB						| 28
