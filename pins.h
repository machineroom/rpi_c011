/*
Definition of C011 <> RPI GPIO pins
bcm2835 lib uses 40 pin J8 connector pin numbering
*/

#include <bcm2835.h>
//      Function    Rpi GPIO pin         BCM2835 GPIO bit
#define D0		    RPI_V2_GPIO_P1_03 // 2
#define D1		    RPI_V2_GPIO_P1_05 // 3
#define D2		    RPI_V2_GPIO_P1_07 // 4
#define D3		    RPI_V2_GPIO_P1_29 // 5
#define D4		    RPI_V2_GPIO_P1_31 // 6
#define D5		    RPI_V2_GPIO_P1_26 // 7
#define D6		    RPI_V2_GPIO_P1_24 // 8
#define D7		    RPI_V2_GPIO_P1_21 // 9

#define RS0		    RPI_V2_GPIO_P1_18 // 24
#define RS1		    RPI_V2_GPIO_P1_19 // 10
#define RESET	    RPI_V2_GPIO_P1_13 // 27
#define CS		    RPI_V2_GPIO_P1_22 // 25
#define RW		    RPI_V2_GPIO_P1_23 // 11

#define IN_INT      RPI_V2_GPIO_P1_32 // 12
#define OUT_INT	    RPI_V2_GPIO_P1_33 // 13

#define INTFC_OE    RPI_V2_GPIO_P1_08 // 14
#define BYTE_DIR    RPI_V2_GPIO_P1_10 // 15
#define OUT_SPARE_1 RPI_V2_GPIO_P1_40 // 21
#define OUT_SPARE_2 RPI_V2_GPIO_P1_35 // 19
#define OUT_SPARE_3 RPI_V2_GPIO_P1_37 // 26
#define OUT_SPARE_4 RPI_V2_GPIO_P1_11 // 17
#define IN_SPARE_1  RPI_V2_GPIO_P1_36 // 16 (TODO schematic bug has this to RPI_V2_GPIO_P1_16 too!)

// direct control of pins on the Transputer interface (J2)
#define ERROR       RPI_V2_GPIO_P1_12 // 18
// BYTE controls the BYTE signal of the HSL cards. HIGH=byte mode, LOW=word mode
#define BYTE        RPI_V2_GPIO_P1_38 // 20

