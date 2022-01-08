PROJECT (rpi_c011_link)

ADD_EXECUTABLE (boot boot.c c011.c)
ADD_EXECUTABLE (test_gpio test_gpio.c)
ADD_EXECUTABLE (c011_loopback c011.c c011_loopback.c)

TARGET_LINK_LIBRARIES (boot bcm2835)
TARGET_LINK_LIBRARIES (test_gpio bcm2835)
TARGET_LINK_LIBRARIES (c011_loopback bcm2835)



