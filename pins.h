/*
Definition of C011 <> RPI GPIO pins
bcm2835 lib uses 40 pin J8 connector pin numbering
*/

#include <bcm2835.h>

#define D0 RPI_V2_GPIO_P1_03    //BLACK
#define D1 RPI_V2_GPIO_P1_05    //BROWN
#define D2 RPI_V2_GPIO_P1_07    //RED
#define D3 RPI_V2_GPIO_P1_11    //ORANGE
#define D4 RPI_V2_GPIO_P1_12    //YELLOW
#define D5 RPI_V2_GPIO_P1_13    //GREEN
#define D6 RPI_V2_GPIO_P1_15    //BLUE
#define D7 RPI_V2_GPIO_P1_16    //VIOLET

#define RS0 RPI_V2_GPIO_P1_18   //GREY
#define RS1 RPI_V2_GPIO_P1_19   //WHITE
#define RESET RPI_V2_GPIO_P1_21 //YELLOW
#define CS RPI_V2_GPIO_P1_22    //GREEN
#define RW RPI_V2_GPIO_P1_23    //BLUE


