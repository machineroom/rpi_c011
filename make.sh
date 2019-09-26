gcc test_gpio.c -lbcm2835 -o test_gpio
gcc c011_loopback.c c011.c -lbcm2835 -o c011_loopback
gcc boot.c c011.c -lbcm2835 -o boot 
