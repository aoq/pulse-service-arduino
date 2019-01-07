#include <Arduino.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BLEGatt.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "bluefruit_config.h"

// Should set this flag if you need debug messages on the console.
#define NEED_CONSOLE_OUTPUT 1

#if NEED_CONSOLE_OUTPUT
#define DEBUG(...) { Serial.println(__VA_ARGS__); }
#else
#define DEBUG(...)
#endif // NEED_CONSOLE_OUTPUT



#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.7.0"
#define MODE_LED_BEHAVIOUR          "MODE"

// Note that UTF-8 code points should be used to show Japanese characters.
const char kDeviceName[] = "Pulse";
const size_t kMaxDeviceNameLength = 24;

const uint16_t kServiceShortUuid = 0x0001;
const uint16_t kPulseStateShortUuid = 0x0002;

// DB940001-02D4-4B52-BEEC-F3DEBED8577D
uint8_t kServiceUuid[16] =
{
    0xDB, 0x94, (uint8_t)(kServiceShortUuid >> 8), (uint8_t)(kServiceShortUuid & 0xFF),
    0x02, 0xD4, 0x4B, 0x52,
    0xBE, 0xEC, 0xF3, 0xDE,
    0xBE, 0xD8, 0x57, 0x70
};

uint8_t kPulseStateUuid[16] =
{
    0xDB, 0x94, (uint8_t)(kPulseStateShortUuid >> 8), (uint8_t)(kPulseStateShortUuid & 0xFF),
    0x02, 0xD4, 0x4B, 0x52,
    0xBE, 0xEC, 0xF3, 0xDE,
    0xBE, 0xD8, 0x57, 0x70
};

// The analog pin number connected to the Pulse Sensor wire.
int pulseSensorPin = 0;
// The LED pin number to blink along with heartbeats.
int heartbeatLedPin = 13;

// The incoming raw signal value [0-1024].
int signal;
// The threshold used to detect heartbeats.
int signalThreshold = 550;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEGatt gatt(ble);

int32_t serviceId;
int32_t characteristicId;



void error(const __FlashStringHelper* err)
{
    DEBUG(err);
    while (1);
}



void onConnected()
{
    DEBUG(F("Connected"));
}

void onDisconnected()
{
    DEBUG(F("Disconnected"));
}


void setup()
{
#if NEED_CONSOLE_OUTPUT
    Serial.begin(9600);
    // Wait for serial port to connect.
    while (!Serial);
#endif // NEED_CONSOLE_OUTPUT

    pinMode(heartbeatLedPin, OUTPUT);

    if (!ble.begin(VERBOSE_MODE))
    {
        error(F("Failed to initialize BLE"));
    }

    if (FACTORYRESET_ENABLE)
    {
        if (!ble.factoryReset())
        {
            error(F("Couldn't factory reset"));
        }
    }

    if (!ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION))
    {
        error(F("Callback requires the firmware version 0.7.0"));
    }

    // Create the device name from the device Bluetooth address.
    ble.println(F("AT+BLEGETADDR"));
    ble.readline();
    String address = ble.buffer;
    // Create the device name from the address.
    char deviceNameCommand[kMaxDeviceNameLength];
    sprintf(deviceNameCommand, "AT+GAPDEVNAME=%s-%02X%02X%02X",
        kDeviceName, address.charAt(0), address.charAt(1), address.charAt(2));
    if (!ble.sendCommandCheckOK(deviceNameCommand))
    {
        error(F("Couldn't set the device name"));
    }

    serviceId = gatt.addService(kServiceUuid);
    if (serviceId == 0)
    {
        error(F("Couldn't add service"));
    }

    characteristicId = gatt.addCharacteristic(kPulseStateUuid,
                                              GATT_CHARS_PROPERTIES_NOTIFY,
                                              1,
                                              1,
                                              BLE_DATATYPE_BYTEARRAY);
    if (characteristicId == 0)
    {
        error(F("Couldn't add characteristic"));
    }

    ble.reset();
    // Disable command echo.
    ble.echo(false);
    // Print device information.
    ble.info();
    ble.verbose(false);

    ble.setConnectCallback(onConnected);
    ble.setDisconnectCallback(onDisconnected);
}

void loop()
{
    signal = analogRead(pulseSensorPin);
#if NEED_CONSOLE_OUTPUT
    Serial.println(signal);
#endif // NEED_CONSOLE_OUTPUT

    uint8_t value[1] = { 0x00 };
    if(signal > signalThreshold)
    {
        value[0] = 0x01;
        gatt.setChar(characteristicId, value, 1);
        digitalWrite(heartbeatLedPin, HIGH);
    }
    else
    {
        value[0] = 0x00;
        gatt.setChar(characteristicId, value, 1);
        digitalWrite(heartbeatLedPin, LOW);
    }

    // Update event callbacks.
    ble.update(200);
    delay(10);
}
