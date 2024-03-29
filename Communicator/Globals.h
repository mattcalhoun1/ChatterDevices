#ifndef COMMUNICATORGLOBALS_H

#define COMMUNICATORGLOBALS_H

#define LOG_ENABLED true

//#define RH_PLATFORM 1

#define DEFAULT_DEVICE_ALIAS "Com"

#define BRIDGE_LORA_ALIAS "Bridge_LoRa"
#define BRIDGE_WIFI_ALIAS "Bridge_Wifi"
#define BRIDGE_CLOUD_ALIAS "Bridge_Cloud"

#define BRIDGE_LORA_DEVICE_ID "001"
#define BRIDGE_WIFI_DEVICE_ID "002"
#define BRIDGE_CLOUD_DEVICE_ID "003"
#define BRIDGE_CHANNEL "UART"

#define MAX_CHANNELS 2 // how many can be simultaneously monitored at once

// how long for ui ot wait for initial connect
#define CLUSTER_ONBOARD_TIMEOUT 30000

//#define BRIDGE_ID_PIN_LORA A0
//#define BRIDGE_ID_PIN_WIFI A1

// if using din switches where pin 1 is a0
//#define FACTORY_RESET_PIN A0
//
#define FACTORY_RESET_PIN 2

#define CLUSTER_ADMIN_PIN A2 // if turned on, this is the cluster owner
//#define CLUSTER_SYNC_PIN A3
#define DEVICE_TYPE_PIN_MINI A3

#define COMMUNICATOR_MESSAGE_BUFFER_SIZE 128

#define BUTTON_A_PIN 2  // d2 on nano iot33
#define PIN_ROTARY_IN1 A1
#define PIN_ROTARY_IN2 A7

#define GUI_MESSAGE_BUFFER_SIZE 1025
#define GUI_MAX_MESSAGE_LENGTH 1024

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

// Currently only iot is supported, although r4 wifi has been successfully working before
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


#define THERMAL_INTERPOLATE_BUFFER_SIZE 768
#define THERMAL_INTERPOLATE_LEVEL 2 // Each interpolation level (max 2) doubles image size, requiring larger interpolation buffer

#define THERMAL_ENCODE_BUFFER_SIZE 768

#define THERMAL_WIDTH 32
#define THERMAL_HEIGHT 24

#define THERMAL_INTERPOLATED_WIDTH 64
#define THERMAL_INTERPOLATED_HEIGHT 48

// TFT = 240x320
#define Display_TFT_RS 9
#define Display_TFT_DC 10
#define Display_TFT_CS 8

// 7789 pins: sckl (SCL); mosi (SDA); cs(CS); rst (RES); dc (DC)
#define Display_7789_SCLK 8
#define Display_7789_MOSI 9
#define Display_7789_CS   6 // nano
#define Display_7789_RST  7
#define Display_7789_DC   10

#define DISPLAY_7789_WIDTH 170
#define DISPLAY_7789_HEIGHT 320

#define DISPLAY_7789_STATUS_X 10
#define DISPLAY_7789_STATUS_Y 309
#define DISPLAY_7789_STATUS_HEIGHT 20
#define DISPLAY_7789_STATUS_WIDTH 160

#define DISPLAY_TFT_STATUS_X 10
#define DISPLAY_TFT_STATUS_Y 309
#define DISPLAY_TFT_STATUS_HEIGHT 25
#define DISPLAY_TFT_STATUS_WIDTH 260

#define DISPLAY_TFT_ALERT_X 10
#define DISPLAY_TFT_ALERT_Y 150
#define DISPLAY_TFT_ALERT_HEIGHT 40

#define DISPLAY_7789_MENU_X 5
#define DISPLAY_7789_MENU_Y 120
#define DISPLAY_7789_MENU_WIDTH 180
#define DISPLAY_7789_MENU_HEIGHT 96
#define DISPLAY_7789_MENU_LINE_HEIGHT 13
#define DISPLAY_7789_MENU_MAX_ITEM_LENGTH 12
#define DISPLAY_7789_MENU_INDENT 3 // pixels to indent menu items

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

#define DISPLAY_7789_TITLE_X 40
#define DISPLAY_7789_TITLE_Y 1
#define DISPLAY_7789_TITLE_WIDTH 170
#define DISPLAY_7789_TITLE_HEIGHT 14

#define DISPLAY_7789_ALERT_X 10
#define DISPLAY_7789_ALERT_Y 150
#define DISPLAY_7789_ALERT_HEIGHT 30

#define DISPLAY_7789_SUBTITLE_X 25
#define DISPLAY_7789_SUBTITLE_Y 21
#define DISPLAY_7789_SUBTITLE_WIDTH 170
#define DISPLAY_7789_SUBTITLE_HEIGHT 21

#define DISPLAY_7789_DASHBOARD_Y 287
#define DISPLAY_7789_DASHBOARD_HEIGHT 18

#define DISPLAY_TFT_MENU_X 15
#define DISPLAY_TFT_MENU_Y 100
#define DISPLAY_TFT_MENU_WIDTH 180
#define DISPLAY_TFT_MENU_HEIGHT 165
#define DISPLAY_TFT_MENU_LINE_HEIGHT 26
#define DISPLAY_TFT_MENU_MAX_ITEM_LENGTH 12
#define DISPLAY_TFT_MENU_INDENT 3 // pixels to indent menu items

#define DISPLAY_TFT_MAIN_IMAGE_X 35
#define DISPLAY_TFT_MAIN_IMAGE_Y 120
#define DISPLAY_TFT_MAIN_IMAGE_WIDTH 64
#define DISPLAY_TFT_MAIN_IMAGE_HEIGHT 48
#define DISPLAY_TFT_MAIN_IMAGE_SUBTITLE_X 55 // relative to bottom left corner of image
#define DISPLAY_TFT_MAIN_IMAGE_SUBTITLE_Y 130 // relative to bottom left corner of image

#define DISPLAY_TFT_ALT_IMAGE_X 35
#define DISPLAY_TFT_ALT_IMAGE_Y 120
#define DISPLAY_TFT_ALT_IMAGE_WIDTH 64
#define DISPLAY_TFT_ALT_IMAGE_HEIGHT 48
#define DISPLAY_TFT_ALT_IMAGE_SUBTITLE_X 55
#define DISPLAY_TFT_ALT_IMAGE_SUBTITLE_Y 130

// message location
#define DISPLAY_TFT_MSG_X 5
#define DISPLAY_TFT_MSG_Y 180
#define DISPLAY_TFT_MSG_WIDTH 170
#define DISPLAY_TFT_MSG_HEIGHT 69

// title/subtitle locations
#define DISPLAY_TFT_TITLE_X 60
#define DISPLAY_TFT_TITLE_Y 15
#define DISPLAY_TFT_TITLE_WIDTH 170
#define DISPLAY_TFT_TITLE_HEIGHT 21

#define DISPLAY_TFT_SUBTITLE_X 50
#define DISPLAY_TFT_SUBTITLE_Y 37
#define DISPLAY_TFT_SUBTITLE_WIDTH 170
#define DISPLAY_TFT_SUBTITLE_HEIGHT 21

#define DISPLAY_TFT_DASHBOARD_Y 287
#define DISPLAY_TFT_DASHBOARD_HEIGHT 18

#define DISPLAY_7789_SPINNER_X 200
#define DISPLAY_7789_SPINNER_Y 35
#define DISPLAY_7789_SPINNER_RADIUS 15

#define DISPLAY_TFT_KEYBOARD_X 5
#define DISPLAY_TFT_KEYBOARD_Y 195
#define DISPLAY_TFT_KEYBOARD_HEIGHT 120
#define DISPLAY_TFT_KEYBOARD_WIDTH 230

#define DISPLAY_TFT_MODAL_TITLE_X 30
#define DISPLAY_TFT_MODAL_TITLE_Y 32
#define DISPLAY_TFT_MODAL_TITLE_HEIGHT 45
#define DISPLAY_TFT_MODAL_INPUT_X 0
#define DISPLAY_TFT_MODAL_INPUT_Y 60
#define DISPLAY_TFT_MODAL_INPUT_HEIGHT 120
#define DISPLAY_TFT_MODAL_INPUT_WIDTH 270


#define DISPLAY_TFT_HEIGHT 320
#define DISPLAY_TFT_WIDTH 270

#define DISPLAY_TFT_SPINNER_X 220
#define DISPLAY_TFT_SPINNER_Y 18
#define DISPLAY_TFT_SPINNER_RADIUS 15

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


#endif