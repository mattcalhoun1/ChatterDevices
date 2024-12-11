#ifndef COMMUNICATORGLOBALS_H

#define COMMUNICATORGLOBALS_H

#include "ChatterAll.h"

// radiolib vs rf 95w wrapper for radiohead
//#define RH_USE_RADIOLIB true

#define CHATTERBOX_FIRMWARE_VERSION "1.2.1"
#define STRONG_ENCRYPTION_ENABLED true // false for export

// which control mode in use
#define CONTROL_MODE_GUI true
#define CONTROL_MODE_TEST false
#define FCC_TEST_ENABLED false

#define LOG_DEBUG
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR
#define LOG_ANALYSIS

#define LORA_DEFAULT_FREQUENCY LORA_DEFAULT_FREQUENCY_915
#define LORA_MIN_FREQUENCY LORA_MIN_FREQUENCY_915
#define LORA_MAX_FREQUENCY LORA_MAX_FREQUENCY_915

#define LOG_ENABLED true
#define BACKPACK_THERMAL_ENABLED false
#define MAX_BACKPACKS 2

//#define RH_PLATFORM 1
#define APP_TITLE "ChatterBox"
#define APP_SUBTITLE ">> Chatters Secure Messaging <<"

#define DEFAULT_DEVICE_ALIAS "Com"

#define CHATTER_LICENSING_SITE_PREFIX "https://api.chatters.io/ChatterLicenseGenerator?devid="

/*#define BRIDGE_LORA_ALIAS "Bridge_LoRa"
#define BRIDGE_WIFI_ALIAS "Bridge_Wifi"
#define BRIDGE_CLOUD_ALIAS "Bridge_Cloud"*/

#define BRIDGE_LORA_DEVICE_ID "001"
#define BRIDGE_WIFI_DEVICE_ID "002"
#define BRIDGE_CLOUD_DEVICE_ID "003"
#define BRIDGE_CHANNEL "UART"

#define MAX_CHANNELS 2 // how many can be simultaneously monitored at once
#define CHANNEL_DISPLAY_SIZE 16 // how many chars the channel name + config can occupy for display purposes

// how long for ui ot wait for initial connect
#define CLUSTER_ONBOARD_TIMEOUT 30000

// disable if there are no buttons on the device
#if defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS) // adafruit
#define FACTORY_RESET_PIN 4 // holding after restart to factory reset
//#define BUTTON_ACTION_PIN 4 // unlock screen, trigger backpack, etc
#define VBATPIN A6 // for checking battery level
#endif

#define COMMUNICATOR_MESSAGE_BUFFER_SIZE 128

// wifi not in use, by default
#define WIFI_SSID_MAX_LEN 24
#define WIFI_CRED_MAX_LEN 24

#define CHATTER_LORA_ENABLED // lora enabled on all devices (needed for onboarding)
#define CHATTER_UART_ENABLED // uart enabled on all devices (if user turns on pref)
#define BACKPACK_ENABLED false // backpacks not yet supported, would share same port as uart

#if defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS) // adafruit
// rotary control
//#define ROTARY_ENABLED
#define BUTTON_A_PIN A3  
#define PIN_ROTARY_IN1 A1
#define PIN_ROTARY_IN2 A2

#define PIN_TOUCH_INT 11 // capacative touch interrupt
#define PIN_TOUCH_RS 12 // capacative touch reset (only used by hoysond/rak, not adafruit)

// for hoysond 3.2
//#define TOUCH_CONTROL_RAK // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT_35, ADAFRUIT_28
//#define DISPLAY_TYPE_HOYSOND // DISPLAY_TYPE_ADAFRUIT / DISPLAY_TYPE_HOYSOND

// for adafruit 3.5
#define TOUCH_CONTROL_ADAFRUIT_35 // TOUCH_CONTROL_RAK, TOUCH_CONTROL_ADAFRUIT_35, ADAFRUIT_28
#define DISPLAY_TYPE_ADAFRUIT_35
#endif

// size of full message buffer within ui (combined decrypted packets)
#define GUI_MESSAGE_BUFFER_SIZE 1025
#define GUI_MAX_MESSAGE_LENGTH 1024

// bridging-related settings
// any message sitting in an out queue older thna this is subject to pruning
// adjust this to keep bridges moving
#define MAX_MESSAGE_AGE_SECONDS 90
#define MESSAGE_PRUNE_FREQUENCY 100

// rtc syncing
#define RTC_SYNC_ENABLED false // once the device has booted, whether to keep syncing. it has caused freeze issues on some devices
#define RTC_SYNC_FREQUENCY 1000 // how often to sync rtc clock, the onboard one can drift. 1000 cycles is roughly every 15 min

// Choose SPI or I2C fram chip (SPI is faster supports larger sizes)
#define STORAGE_FRAM_SPI true
//#define STORAGE_FRAM_I2C true

// channel level logs. these logs will be encrypted at the same level as transmissions
#define LORA_CHANNEL_LOG_ENABLED true
#define UDP_CHANNEL_LOG_ENABLED false
#define CAN_CHANNEL_LOG_ENABLED false
#define UART_CHANNEL_LOG_ENABLED false

// Currently only adafruit m4 is supported
#if defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4)|| defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define LORA_RFM9X_CS A4
#define LORA_RFM9X_INT A5
#define LORA_RFM9X_RST A0
#define LORA_RFM9X_BUSY -1 // no busy pin
#define CHATTER_WIFI_ENABLED
#define ESP32_RESETN A3  // was btn a
#define SPIWIFI_SS A1 // was rotary 1
#define SPIWIFI_ACK A2 // was rotary 2 // a.k.a BUSY or READY
#define ESP32_GPIO0   -1  // Not connected
#endif

// wifi spi settings (if used)
#define SPIWIFI SPI

/** Display pins */
#if defined (ARDUINO_SAMD_ZERO) || defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define Display_TFT_RS 5
#define Display_TFT_DC 6
#define Display_TFT_CS 9
#define DISPLAY_TFT_BACKLIGHT 13
#endif

#if defined(DISPLAY_TYPE_ADAFRUIT_35)
#include "Globals_Screen_320_480.h"
#else
#include "Globals_Screen_270_320.h"
#endif

// for thermal cam, need interpolation settings and buffer
#define THERMAL_INTERPOLATE_BUFFER_SIZE 768
#define THERMAL_INTERPOLATE_LEVEL 2 // Each interpolation level (max 2) doubles image size, requiring larger interpolation buffer
#define THERMAL_ENCODE_BUFFER_SIZE 768
#define THERMAL_WIDTH 32
#define THERMAL_HEIGHT 24
#define THERMAL_INTERPOLATED_WIDTH 64
#define THERMAL_INTERPOLATED_HEIGHT 48

#include "Globals_Colors.h"

#endif