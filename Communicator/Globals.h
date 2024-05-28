#ifndef COMMUNICATORGLOBALS_H

#define COMMUNICATORGLOBALS_H

#define LOG_ENABLED true

//#define RH_PLATFORM 1
#define APP_TITLE "ChatterBox"
#define APP_SUBTITLE ">> Chatters Secure Messaging <<"

#define DEFAULT_DEVICE_ALIAS "Com"

#define BRIDGE_LORA_ALIAS "Bridge_LoRa"
#define BRIDGE_WIFI_ALIAS "Bridge_Wifi"
#define BRIDGE_CLOUD_ALIAS "Bridge_Cloud"

#define BRIDGE_LORA_DEVICE_ID "001"
#define BRIDGE_WIFI_DEVICE_ID "002"
#define BRIDGE_CLOUD_DEVICE_ID "003"
#define BRIDGE_CHANNEL "UART"

#define MAX_CHANNELS 2 // how many can be simultaneously monitored at once
#define CHANNEL_DISPLAY_SIZE 16 // how many chars the channel name + config can occupy for display purposes

// how long for ui ot wait for initial connect
#define CLUSTER_ONBOARD_TIMEOUT 30000


// ------ deprecated -------
#define CLUSTER_ADMIN_PIN A2 // if turned on, this is the cluster owner
#define DEVICE_TYPE_PIN_MINI A3
// ----------------------------

// for some reason, this is shorted on the pcb
//#if defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS) // adafruit
//#define FACTORY_RESET_PIN 4
//#endif

#define COMMUNICATOR_MESSAGE_BUFFER_SIZE 128

#define WIFI_SSID_MAX_LEN 24
#define WIFI_CRED_MAX_LEN 24

#define CHATTER_LORA_ENABLED // lora enabled on all devices (needed for onboarding)
#define CHATTER_UART_ENABLED // uart enabled on all devices (if user turns on pref)
#define BACKPACK_ENABLED false // backpacks not yet supported, would share same port as uart

// Button Pins (need interrupts)
// Nano
#if defined (ARDUINO_SAMD_NANO_33_IOT)
// nano does not have enough pins for both touch interrupt/rs and rotary
//#define BUTTON_A_PIN 2  // d2 on nano iot33
//#define PIN_ROTARY_IN1 A1
//#define PIN_ROTARY_IN2 A7
#define PIN_TOUCH_INT 2
#define PIN_TOUCH_RS A1
#define TOUCH_CONTROL_ADAFRUIT // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT
#define DISPLAY_TYPE_ADAFRUIT // DISPLAY_TYPE_ADAFRUIT / DISPLAY_TYPE_HOYSOND
//#define TOUCH_CONTROL_RAK // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT
//#define DISPLAY_TYPE_HOYSOND // DISPLAY_TYPE_ADAFRUIT / DISPLAY_TYPE_HOYSOND

#elif defined (ARDUINO_SAMD_MKRWAN1310) || defined (ARDUINO_SAMD_MKRZERO)
//#define ROTARY_ENABLED
#define BUTTON_A_PIN 5  // d5 on mkr wan 1310
#define PIN_ROTARY_IN1 A1
#define PIN_ROTARY_IN2 A2
#define PIN_TOUCH_INT 0
#define PIN_TOUCH_RS A4
#define TOUCH_CONTROL_RAK // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT
#define DISPLAY_TYPE_HOYSOND // DISPLAY_TYPE_ADAFRUIT / DISPLAY_TYPE_HOYSOND
#elif defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS) // adafruit
//#define ROTARY_ENABLED
#define BUTTON_A_PIN A3  
#define PIN_ROTARY_IN1 A1
#define PIN_ROTARY_IN2 A2
#define PIN_TOUCH_INT 11
#define PIN_TOUCH_RS 12
#define TOUCH_CONTROL_RAK // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT
#define DISPLAY_TYPE_HOYSOND // DISPLAY_TYPE_ADAFRUIT / DISPLAY_TYPE_HOYSOND
#endif

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
#define LORA_CHANNEL_LOG_ENABLED false
#define UDP_CHANNEL_LOG_ENABLED true
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
#define CHATTER_WIFI_ENABLED // will use onboard, instead of pins
#define CHATTER_ONBOARD_WIFI

#define LORA_RFM9X_CS 4
#define LORA_RFM9X_INT 3
#define LORA_RFM9X_RST 5
#elif defined (ARDUINO_SAMD_MKRWAN1310)
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/src/LoRa.h
#define LORA_RFM9X_CS LORA_IRQ_DUMB
#define LORA_RFM9X_INT LORA_IRQ
#define LORA_RFM9X_RST -1
#elif defined (ARDUINO_SEEED_XIAO_M0)
#define LORA_RFM9X_CS 0
#define LORA_RFM9X_INT 1
#define LORA_RFM9X_RST 2

//#elif defined (ARDUINO_SAMD_ZERO)
//sparkfun
//#define LORA_RFM9X_CS 12
//#define LORA_RFM9X_INT 6
//#define LORA_RFM9X_RST -1

//#define CHATTER_WIFI_ENABLED
//#define ESP32_RESETN A3  // was btn a
//#define SPIWIFI_SS A1 // was rotary 1
//#define SPIWIFI_ACK A2 // was rotary 2 // a.k.a BUSY or READY
//#define ESP32_GPIO0   -1  // Not connected

/*adafruit*/
#elif defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4)|| defined(ADAFRUIT_FEATHER_M4_EXPRESS)
// for onboard rfm95x, which doesnt work because it interferes with fram chip
//#define LORA_RFM9X_CS 8
//#define LORA_RFM9X_INT 3
//#define LORA_RFM9X_RST 4

#define LORA_RFM9X_CS A4
#define LORA_RFM9X_INT A5
#define LORA_RFM9X_RST A0


#define CHATTER_WIFI_ENABLED
#define ESP32_RESETN A3  // was btn a
#define SPIWIFI_SS A1 // was rotary 1
#define SPIWIFI_ACK A2 // was rotary 2 // a.k.a BUSY or READY
#define ESP32_GPIO0   -1  // Not connected

//mkrzero
#elif defined (ARDUINO_SAMD_MKRZERO)
#define LORA_RFM9X_CS 20 // (A5)
#define LORA_RFM9X_INT 4
#define LORA_RFM9X_RST 21 // (A6)

#define CHATTER_WIFI_ENABLED
#define ESP32_RESETN 5  // was btn a
#define SPIWIFI_SS A1 // was rotary 1
#define SPIWIFI_ACK A2 // was rotary 2 // a.k.a BUSY or READY
#define ESP32_GPIO0   -1  // Not connected

#endif

// wifi spi settings (if used)
// Configure the pins used for the ESP32 connection
#define SPIWIFI     SPI
//#define SPIWIFI_SS    5  // Chip select pin : A5/D20
//#define SPIWIFI_ACK   3   // a.k.a BUSY or READY pin : A6/D21
//#define ESP32_RESETN  4   // Reset pin : A1 / D16
//#define ESP32_GPIO0   -1  // Not connected

/** Display pins */
#if defined (ARDUINO_SAMD_NANO_33_IOT)
#define Display_TFT_RS 9
#define Display_TFT_DC 10
#define Display_TFT_CS 8
#define DISPLAY_TFT_BACKLIGHT A3
#elif defined (ARDUINO_SAMD_MKRWAN1310) || defined (ARDUINO_SAMD_MKRZERO)
#define Display_TFT_RS 3
#define Display_TFT_DC 1
#define Display_TFT_CS 2
#define DISPLAY_TFT_BACKLIGHT A3
#elif defined (ARDUINO_SEEED_XIAO_M0)
#define Display_TFT_RS 0 // not used
#define Display_TFT_DC 3
#define Display_TFT_CS 6
#define DISPLAY_TFT_BACKLIGHT -1
// adafruit

#elif defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define Display_TFT_RS 5
#define Display_TFT_DC 6
#define Display_TFT_CS 9
#define DISPLAY_TFT_BACKLIGHT 13

// sparkfun rf pro
/*#elif defined (ARDUINO_SAMD_ZERO)
#define Display_TFT_RS 3
#define Display_TFT_DC 1
#define Display_TFT_CS 2
#define DISPLAY_TFT_BACKLIGHT A3
/** end Display pins */
#endif

#define THERMAL_INTERPOLATE_BUFFER_SIZE 768
#define THERMAL_INTERPOLATE_LEVEL 2 // Each interpolation level (max 2) doubles image size, requiring larger interpolation buffer

#define THERMAL_ENCODE_BUFFER_SIZE 768

#define THERMAL_WIDTH 32
#define THERMAL_HEIGHT 24

#define THERMAL_INTERPOLATED_WIDTH 64
#define THERMAL_INTERPOLATED_HEIGHT 48

#define DISPLAY_TFT_STATUS_X 10
#define DISPLAY_TFT_STATUS_Y 318
#define DISPLAY_TFT_STATUS_HEIGHT 25
#define DISPLAY_TFT_STATUS_WIDTH 100

#define DISPLAY_TFT_CACHE_STATUS_X 130
#define DISPLAY_TFT_CACHE_STATUS_Y 312
#define DISPLAY_TFT_CACHE_STATUS_HEIGHT 7
#define DISPLAY_TFT_CACHE_STATUS_WIDTH 80

#define DISPLAY_TFT_ALERT_X 10
#define DISPLAY_TFT_ALERT_Y 150
#define DISPLAY_TFT_ALERT_HEIGHT 40

#define DISPLAY_TFT_LS_ALERT_X 10
#define DISPLAY_TFT_LS_ALERT_Y 150
#define DISPLAY_TFT_LS_ALERT_HEIGHT 40

#define DISPLAY_TFT_MENU_X 25
#define DISPLAY_TFT_MENU_Y 80
#define DISPLAY_TFT_MENU_WIDTH 180
#define DISPLAY_TFT_MENU_HEIGHT 215
#define DISPLAY_TFT_MENU_LINE_HEIGHT 36
#define DISPLAY_TFT_MENU_MAX_ITEM_LENGTH 12
#define DISPLAY_TFT_MENU_INDENT 3 // pixels to indent menu items

#define DISPLAY_TFT_MENU_SCROLL_UP_X 220
#define DISPLAY_TFT_MENU_SCROLL_UP_Y 110
#define DISPLAY_TFT_MENU_SCROLL_DOWN_X 220
#define DISPLAY_TFT_MENU_SCROLL_DOWN_Y 275
#define DISPLAY_TFT_MENU_SCROLL_RADIUS 6
#define DISPLAY_TFT_MENU_SCROLL_TOUCH_RADIUS 20
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
#define DISPLAY_TFT_MSG_Y 65
#define DISPLAY_TFT_MSG_WIDTH 230
#define DISPLAY_TFT_MSG_AREA_HEIGHT 216
#define DISPLAY_TFT_MSG_HEIGHT 40
#define DISPLAY_TFT_MSG_TITLE_HEIGHT 16
#define DISPLAY_TFT_MSG_MAX_DISPLAY 4

// title/subtitle locations
#define DISPLAY_TFT_TITLE_X 5
#define DISPLAY_TFT_TITLE_Y 17
#define DISPLAY_TFT_TITLE_WIDTH 230
#define DISPLAY_TFT_TITLE_HEIGHT 22

#define DISPLAY_TFT_SUBTITLE_X 15
#define DISPLAY_TFT_SUBTITLE_Y 41
#define DISPLAY_TFT_SUBTITLE_WIDTH 190
#define DISPLAY_TFT_SUBTITLE_HEIGHT 17

#define DISPLAY_TFT_DASHBOARD_X 0
#define DISPLAY_TFT_DASHBOARD_Y 314
#define DISPLAY_TFT_DASHBOARD_HEIGHT 10
#define DISPLAY_TFT_DASHBOARD_WIDTH 240

// portrait
#define DISPLAY_TFT_KEYBOARD_X 5
#define DISPLAY_TFT_KEYBOARD_Y 195
#define DISPLAY_TFT_KEYBOARD_HEIGHT 120
#define DISPLAY_TFT_KEYBOARD_WIDTH 230

#define DISPLAY_TFT_MODAL_TITLE_X 5
#define DISPLAY_TFT_MODAL_TITLE_Y 22
#define DISPLAY_TFT_MODAL_TITLE_HEIGHT 25
#define DISPLAY_TFT_MODAL_SUBTITLE_X 5
#define DISPLAY_TFT_MODAL_SUBTITLE_Y 34
#define DISPLAY_TFT_MODAL_SUBTITLE_HEIGHT 12
#define DISPLAY_TFT_MODAL_INPUT_X 1
#define DISPLAY_TFT_MODAL_INPUT_Y 60
#define DISPLAY_TFT_MODAL_INPUT_HEIGHT 120
#define DISPLAY_TFT_MODAL_INPUT_WIDTH 270

#define DISPLAY_TFT_TICKER_X 231
#define DISPLAY_TFT_TICKER_Y 315
#define DISPLAY_TFT_TICKER_SIZE 3

#define DISPLAY_BUTTON_AREA_X 20
#define DISPLAY_BUTTON_AREA_Y 278
#define DISPLAY_BUTTON_HEIGHT 18
#define DISPLAY_BUTTON_WIDTH 60
#define DISPLAY_BUTTON_HORIZONTAL_OFFSET 75 // space between buttons

// landscape
#define DISPLAY_TFT_LS_KEYBOARD_X 5
#define DISPLAY_TFT_LS_KEYBOARD_Y 100
#define DISPLAY_TFT_LS_KEYBOARD_HEIGHT 140
#define DISPLAY_TFT_LS_KEYBOARD_WIDTH 312

#define DISPLAY_TFT_LS_MODAL_TITLE_X 5
#define DISPLAY_TFT_LS_MODAL_TITLE_Y 22
#define DISPLAY_TFT_LS_MODAL_TITLE_HEIGHT 25
#define DISPLAY_TFT_LS_MODAL_SUBTITLE_X 5
#define DISPLAY_TFT_LS_MODAL_SUBTITLE_Y 34
#define DISPLAY_TFT_LS_MODAL_SUBTITLE_HEIGHT 12
#define DISPLAY_TFT_LS_MODAL_INPUT_X 1
#define DISPLAY_TFT_LS_MODAL_INPUT_Y 60
#define DISPLAY_TFT_LS_MODAL_INPUT_HEIGHT 40
#define DISPLAY_TFT_LS_MODAL_INPUT_WIDTH 320

#define DISPLAY_TFT_HEIGHT 320
#define DISPLAY_TFT_WIDTH 240

#define DISPLAY_TFT_SPINNER_X 220
#define DISPLAY_TFT_SPINNER_Y 18
#define DISPLAY_TFT_SPINNER_RADIUS 15

#define DISPLAY_TFT_PROGRESS_BAR_X 50
#define DISPLAY_TFT_PROGRESS_BAR_Y 220
#define DISPLAY_TFT_PROGRESS_BAR_WIDTH 140
#define DISPLAY_TFT_PROGRESS_BAR_HEIGHT 20

#define DISPLAY_TFT_MAIN_SCROLL_UP_X 220
#define DISPLAY_TFT_MAIN_SCROLL_UP_Y 35
#define DISPLAY_TFT_MAIN_SCROLL_DOWN_X 220
#define DISPLAY_TFT_MAIN_SCROLL_DOWN_Y 260
#define DISPLAY_TFT_MAIN_SCROLL_RADIUS 6
#define DISPLAY_TFT_MAIN_SCROLL_TOUCH_RADIUS 20

#define DISPLAY_TFT_LOCK_BUTTON_X 1
#define DISPLAY_TFT_LOCK_BUTTON_Y 1
#define DISPLAY_TFT_LOCK_BUTTON_SIZE 20

#define DISPLAY_TFT_FLIP_BUTTON_X 219
#define DISPLAY_TFT_FLIP_BUTTON_Y 1
#define DISPLAY_TFT_FLIP_BUTTON_SIZE 20


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

#define DARKBLUE 0x0B16
#define LIGHTBLUE 0x451B
#define BEIGE 0xF779

#define DARKGREEN 0x12AA// 0x43AD//0xA613 // rgb(161, 195, 152)
#define LIGHTGREEN 0xC758 // rgb(198, 235, 197)
#define DARKRED 0xA208 // rgb(163, 67, 67)

#define LIGHTGREY 0xC618   ///< 198, 195, 198

#define BRIGHTGREEN 0x43AD // rgb(67, 118, 108)
#define DARKGRAY 0x7BEF    ///< 123, 125, 123
#define BRIGHTYELLOW 0xfcc0 // rgb(255, 152, 0)
#define BROWN 0xD343
#define ORANGE 0xFA20

// tvt colors
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198

#endif



