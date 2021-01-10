#define SONOFF_BUTTON             0
#define SONOFF_RELAY              12
#define SONOFF_LED                13
#define SONOFF_LEDSTRIP           14
//----------------------------------------------------------------------------
#define OFF_RELAY                 LOW
#define OFF_LED                   HIGH
//----------------------------------------------------------------------------
#define NUM_LEDS                  33
#define MAX_RGB_VAL               255
#define RGB_COMBO_NUM             9
#define RGB_EFFECTS_NUM           2
//----------------------------------------------------------------------------
#define BLINK_TIME                1000        // Delay cambio programma
#define TIME_FLASH_BLINK          100
#define PLACING_TIME              200
#define TIMER_CONNECTION          300000      // Controllo connessione ogni 5 min
#define MQTT_CONNECTION           5000
#define TIMER_RGB_PLAY            250
#define INCREMENT_STEP            3
//----------------------------------------------------------------------------
#define EEPROM_SIZE               256
//----------------------------------------------------------------------------
#define NUM_WIFI_SETTINGS         9
#define MAX_LENGTH_SETTING        16
//----------------------------------------------------------------------------
#define ON_PAYLOAD                "ON"
#define OFF_PAYLOAD               "OFF"
//----------------------------------------------------------------------------
#define JSON_MSG_LENGTH           32
//----------------------------------------------------------------------------
#define Version                   "32.1.0.0"
//----------------------------------------------------------------------------
//---Per upload file .bin in OTA Drive non toccare----------------------------
#define ProductKey                "abc12345-1a1a-12a1-a1ab-a1ab1a1ab123"
#define MakeFirmwareInfo(k, v)    "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"
//----------------------------------------------------------------------------
bool pushButton                   = false;
bool pushButtonPre                = false;
bool deviceConnected              = false;
//----------------------------------------------------------------------------
byte R                            = MAX_RGB_VAL;
byte G                            = MAX_RGB_VAL;
byte B                            = MAX_RGB_VAL;
byte Brightness                   = MAX_RGB_VAL / 3;
byte RGB_Efect_Selected           = 0;
byte RandomEffectStep             = 0;
//----------------------------------------------------------------------------
int pushButtonCount               = 0;
//----------------------------------------------------------------------------
unsigned long pushButtonTime      = 0;
unsigned long lastTimeCheckConn   = 0;
unsigned long lastMqttCheckConn   = 0;
unsigned long lastTimeRgbPlay     = 0;
//----------------------------------------------------------------------------
String DefaultApName              = "Sonoff AP";
String mac                        = "";
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
WifiSetup OtaDriveProductKey = {"OtaDriveProductKey", ""};

WifiSetup* WifiSettings[NUM_WIFI_SETTINGS] = {
  &Hostname, 
  &Ssid, 
  &Password, 
  &MqttSubTopic, 
  &MqttPubTopic,
  &MqttServer, 
  &MqttUser, 
  &MqttPassword,
  &OtaDriveProductKey};

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
