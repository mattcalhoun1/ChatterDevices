#ifndef SENTRYGLOBALS_H

#define SENTRYGLOBALS_H

#define LOG_ENABLED true

//#define RH_PLATFORM 1

#define BRIDGE_LORA_ALIAS "Bridge_LoRa"
#define BRIDGE_WIFI_ALIAS "Bridge_Wifi"
#define BRIDGE_CLOUD_ALIAS "Bridge_Cloud"
#define BRIDGE_CHANNEL "UART"

#define FACTORY_RESET_PIN A0

//#define BRIDGE_ID_PIN_LORA A0
#define BRIDGE_ID_PIN_WIFI A1 // not being set identifies this as a lora bridge

#define JOIN_CLUSTER_PIN A2

// any message sitting in an out queue older thna this is subject to pruning
// adjust this to keep bridges moving
#define MAX_MESSAGE_AGE_SECONDS 90
#define MESSAGE_PRUNE_FREQUENCY 100
#define RTC_SYNC_FREQUENCY 1000 // how often to sync rtc clock, the onboard one can drift. 1000 cycles is roughly every 15 min

// Choose SPI or I2C fram chip (SPI is faster supports larger sizes)
#define STORAGE_FRAM_SPI true
//#define STORAGE_FRAM_I2C true

// channel level logs
#define LORA_CHANNEL_LOG_ENABLED true
#define UDP_CHANNEL_LOG_ENABLED false
#define CAN_CHANNEL_LOG_ENABLED false
#define UART_CHANNEL_LOG_ENABLED false

#if defined(ARDUINO_UNOR4_WIFI)
#define LORA_RFM9X_CS 5
#define LORA_RFM9X_INT 3
#define LORA_RFM9X_RST 4
#elif defined(ARDUINO_SAM_DUE)
#define LORA_RFM9X_CS 4
#define LORA_RFM9X_INT 5
#define LORA_RFM9X_RST 6
#elif defined (ARDUINO_SAMD_NANO_33_IOT)
#define LORA_RFM9X_CS 4
#define LORA_RFM9X_INT 3
#define LORA_RFM9X_RST 5
#endif


//#define LORA_RFM9X_FREQ 915.0

// Configure the pins used for the ESP32 connection
//#define SPIWIFI     SPI
#define SPIWIFI_SS    5  // Chip select pin : A5/D20
#define SPIWIFI_ACK   3   // a.k.a BUSY or READY pin : A6/D21
#define ESP32_RESETN  4   // Reset pin : A1 / D16
#define ESP32_GPIO0   -1  // Not connected


//#define THERMAL_INTERPOLATE_BUFFER_SIZE 768
//#define THERMAL_INTERPOLATE_LEVEL 2 // Each interpolation level (max 2) doubles image size, requiring larger interpolation buffer

//#define THERMAL_ENCODE_BUFFER_SIZE 768

//#define THERMAL_WIDTH 32
//#define THERMAL_HEIGHT 24

//#define THERMAL_INTERPOLATED_WIDTH 64
//#define THERMAL_INTERPOLATED_HEIGHT 48

// 1331 pins: sckl (SCL); mosi (SDA); cs(CS); rst (RES); dc (DC)
//#define Display_1331_SCLK 12
//#define Display_1331_MOSI 11
//#define Display_1331_CS   6
//#define Display_1331_RST  1
//#define Display_1331_DC   3

// 7789 pins: sckl (SCL); mosi (SDA); cs(CS); rst (RES); dc (DC)
//#define Display_7789_SCLK 1
//#define Display_7789_MOSI 0
//#define Display_7789_MOSI 0

//#define Display_7789_CS   2
//#define Display_7789_RST  4
//#define Display_7789_DC   3

//#define DISPLAY_1331_STATUS_X 28
//#define DISPLAY_1331_STATUS_Y 54
//#define DISPLAY_1331_STATUS_HEIGHT 10
//#define DISPLAY_1331_STATUS_WIDTH 64

//#define DISPLAY_1306_STATUS_X 28
//#define DISPLAY_1306_STATUS_Y 54
//#define DISPLAY_1306_STATUS_HEIGHT 8
//#define DISPLAY_1306_STATUS_WIDTH 64

//#define DISPLAY_7789_STATUS_X 10
//#define DISPLAY_7789_STATUS_Y 300
//#define DISPLAY_7789_STATUS_HEIGHT 20
//#define DISPLAY_7789_STATUS_WIDTH 160

//#define DISPLAY_1306_MAIN_IMAGE_X 20
//#define DISPLAY_1306_MAIN_IMAGE_Y 0
//#define DISPLAY_1306_MAIN_IMAGE_WIDTH 64
//#define DISPLAY_1306_MAIN_IMAGE_HEIGHT 48
//#define DISPLAY_1306_MAIN_IMAGE_SUBTITLE_X 5 // relative to bottom left corner of image
//#define DISPLAY_1306_MAIN_IMAGE_SUBTITLE_Y 5 // relative to bottom left corner of image

//#define DISPLAY_1306_ALT_IMAGE_X 20
//#define DISPLAY_1306_ALT_IMAGE_Y 0
//#define DISPLAY_1306_ALT_IMAGE_WIDTH 64
//#define DISPLAY_1306_ALT_IMAGE_HEIGHT 48
//#define DISPLAY_1306_ALT_IMAGE_SUBTITLE_X 5 // relative to bottom left corner of image
//#define DISPLAY_1306_ALT_IMAGE_SUBTITLE_Y 10 // relative to bottom left corner of image

//#define DISPLAY_1306_MSG_X 5
//#define DISPLAY_1306_MSG_Y 0
//#define DISPLAY_1306_MSG_WIDTH 64
//#define DISPLAY_1306_MSG_HEIGHT 48

//#define DISPLAY_1331_MAIN_IMAGE_X 20
//#define DISPLAY_1331_MAIN_IMAGE_Y 0
//#define DISPLAY_1331_MAIN_IMAGE_WIDTH 64
//#define DISPLAY_1331_MAIN_IMAGE_HEIGHT 48
//#define DISPLAY_1331_MAIN_IMAGE_SUBTITLE_X 5 // relative to bottom left corner of image
//#define DISPLAY_1331_MAIN_IMAGE_SUBTITLE_Y 5 // relative to bottom left corner of image

//#define DISPLAY_1331_ALT_IMAGE_X 20
//#define DISPLAY_1331_ALT_IMAGE_Y 0
//#define DISPLAY_1331_ALT_IMAGE_WIDTH 64
//#define DISPLAY_1331_ALT_IMAGE_HEIGHT 48
//#define DISPLAY_1331_ALT_IMAGE_SUBTITLE_X 25
//#define DISPLAY_1331_ALT_IMAGE_SUBTITLE_Y 50
/*
#define DISPLAY_1331_MSG_X 5
#define DISPLAY_1331_MSG_Y 0
#define DISPLAY_1331_MSG_WIDTH 64
#define DISPLAY_1331_MSG_HEIGHT 48

#define DISPLAY_7789_MAIN_IMAGE_X 35
#define DISPLAY_7789_MAIN_IMAGE_Y 120
#define DISPLAY_7789_MAIN_IMAGE_WIDTH 64
#define DISPLAY_7789_MAIN_IMAGE_HEIGHT 48
#define DISPLAY_7789_MAIN_IMAGE_SUBTITLE_X 55 // relative to bottom left corner of image
#define DISPLAY_7789_MAIN_IMAGE_SUBTITLE_Y 130 // relative to bottom left corner of image

#define DISPLAY_7789_ALT_IMAGE_X 35
#define DISPLAY_7789_ALT_IMAGE_Y 120
#define DISPLAY_7789_ALT_IMAGE_WIDTH 64
#define DISPLAY_7789_ALT_IMAGE_HEIGHT 48
#define DISPLAY_7789_ALT_IMAGE_SUBTITLE_X 55
#define DISPLAY_7789_ALT_IMAGE_SUBTITLE_Y 130

// message location
#define DISPLAY_7789_MSG_X 5
#define DISPLAY_7789_MSG_Y 180
#define DISPLAY_7789_MSG_WIDTH 170
#define DISPLAY_7789_MSG_HEIGHT 69

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define GRAY            0x1111
*/
#endif