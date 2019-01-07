#ifndef BLUEFRUIT_CONFIG_H_
#define BLUEFRUIT_CONFIG_H_

// Size of the read buffer for incoming data.
#define BUFSIZE         128
// If set to 'true' enables debug output.
#define VERBOSE_MODE    true

// Required for software serial.
#define BLUEFRUIT_SWUART_RXD_PIN     9
#define BLUEFRUIT_SWUART_TXD_PIN    10
#define BLUEFRUIT_UART_CTS_PIN      11
// Unused.
#define BLUEFRUIT_UART_RTS_PIN      -1


// This makes it not complain on compilation if there's no Serial1
#ifdef Serial1
#define BLUEFRUIT_HWSERIAL_NAME     Serial1
#endif // Serial1

#define BLUEFRUIT_UART_MODE_PIN     12


#define BLUEFRUIT_SPI_CS             8
#define BLUEFRUIT_SPI_IRQ            7
#define BLUEFRUIT_SPI_RST            6
#define BLUEFRUIT_SPI_SCK           13
#define BLUEFRUIT_SPI_MISO          12
#define BLUEFRUIT_SPI_MOSI          11

#endif // BLUEFRUIT_CONFIG_H_