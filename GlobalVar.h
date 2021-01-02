/*
GPIO03 and GPIO02 are pulled high momentarily after boot. 
Any connected device may "blink" when the Sonoff is powering up.

Sonoff r1
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO14

Sonoff r2 (ESP8285 SoC with 1MB flash)
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO02 no pullup, labeled as IO2

Sonoff r3 (ESP8285)
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO09
    GPIO10
    GPIO16  
*/

#define SONOFF_BUTTON           0
#define SONOFF_RELAY            12
#define SONOFF_LED              13
#define SONOFF_LEDSTRIP         14
//----------------------------------------------------------------------------
#define OFF_RELAY               LOW
#define OFF_LED                 HIGH
//----------------------------------------------------------------------------
#define NUM_LEDS                33
#define MAX_RGB_VAL             255
#define RGB_COMBO_NUM           9
#define RGB_EFFECTS_NUM         2
//----------------------------------------------------------------------------
#define BLINK_TIME              1000        // Delay cambio programma
#define TIME_FLASH_BLINK        100
#define PLACING_TIME            200
#define TIMER_CONNECTION        300000      // Controllo connessione ogni 5 min
#define TIMER_RGB_PLAY          250
#define INCREMENT_STEP          3
//----------------------------------------------------------------------------
#define EEPROM_SIZE             256
//----------------------------------------------------------------------------
#define NUM_WIFI_SETTINGS       8
#define MAX_LENGTH_SETTING      16
//----------------------------------------------------------------------------
#define ON_PAYLOAD              "ON"
#define OFF_PAYLOAD             "OFF"
//----------------------------------------------------------------------------
#define ProductKey              "2e8f823f-ed0c-4732-966a-16128b58340c"
#define Version                 "25.0.0.0"
#define MakeFirmwareInfo(k, v)  "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"
//----------------------------------------------------------------------------
bool pushButton                 = false;
bool pushButtonPre              = false;
bool deviceConnected            = false;
//----------------------------------------------------------------------------
byte R                          = MAX_RGB_VAL;
byte G                          = MAX_RGB_VAL;
byte B                          = MAX_RGB_VAL;
byte Brightness                 = MAX_RGB_VAL / 3;
byte RGB_Efect_Selected         = 0;
byte RandomEffectStep           = 0;
//----------------------------------------------------------------------------
int pushButtonCount             = 0;
//----------------------------------------------------------------------------
unsigned long pushButtonTime    = 0;
unsigned long lastTimeCheckConn = 0;
unsigned long lastTimeRgbPlay   = 0;
//----------------------------------------------------------------------------
String DefaultApName            = "Sonoff AP";
//----------------------------------------------------------------------------

struct WifiSetup{
  String Name;
  String Val;
};

WifiSetup Hostname           = {"Hostname", ""};
WifiSetup Ssid               = {"Ssid", ""};
WifiSetup Password           = {"Password", ""};
WifiSetup MqttSubTopic       = {"MqttSubTopic", ""};
WifiSetup MqttPubTopic       = {"MqttPubTopic", ""};
WifiSetup MqttServer         = {"MqttServer", ""};
WifiSetup MqttUser           = {"MqttUser", ""};
WifiSetup MqttPassword       = {"MqttPassword", ""};

WifiSetup* WifiSettings[NUM_WIFI_SETTINGS] = {
  &Hostname, 
  &Ssid, 
  &Password, 
  &MqttSubTopic, 
  &MqttPubTopic,
  &MqttServer, 
  &MqttUser, 
  &MqttPassword};

struct RbgRandomEffect{
  String Name;
  byte TargetVal;
};

RbgRandomEffect RedMax         = {"RedMax", MAX_RGB_VAL};
RbgRandomEffect GreenMax       = {"GreenMax", MAX_RGB_VAL};
RbgRandomEffect BlueMax        = {"BlueMax", MAX_RGB_VAL};
RbgRandomEffect RedMin         = {"RedMin", 0};
RbgRandomEffect GreenMin       = {"GreenMin", 0};
RbgRandomEffect BlueMin        = {"BlueMin", 0};
RbgRandomEffect RedHalf        = {"RedHalf", MAX_RGB_VAL / 2};
RbgRandomEffect GreenHalf      = {"GreenHalf", MAX_RGB_VAL / 2};
RbgRandomEffect BlueHalf       = {"BlueHalf", MAX_RGB_VAL / 2};

RbgRandomEffect* Random_Effects[RGB_COMBO_NUM] = {
  &RedMax, 
  &GreenMax, 
  &BlueMax, 
  &RedMin, 
  &GreenMin, 
  &BlueMin, 
  &RedHalf, 
  &GreenHalf, 
  &BlueHalf};

struct RbgEffect{
  String Effect_Name;
  byte Effect_Number;
};

RbgEffect Solid               = {"Solid", 0};
RbgEffect Random              = {"Random", 1};

RbgEffect* RbgEffects[RGB_EFFECTS_NUM] = {
  &Solid, 
  &Random};
