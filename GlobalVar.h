//#define ENABLE_PRINT 
#ifdef ENABLE_PRINT
#define Sbegin(a)                 (Serial.begin(a))
#define Sprint(a)                 (Serial.print(a))
#define Sprintln(a)               (Serial.println(a))
#define Sprintf(a, b, c)          (Serial.printf(a , b, c))
#else
#define Sbegin(a)
#define Sprint(a)
#define Sprintln(a)
#define Sprintf(a, b, c)
#endif
//----------------------------------------------------------------------------
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
#define INCREMENT_STEP            3
//----------------------------------------------------------------------------
#define T_100MS                   100
#define T_200MS                   200
#define T_250MS                   250
#define T_1S                      1000        // Delay change function
#define T_5S                      5000
#define T_5MIN                    300000      // Connection check every 5 min
//----------------------------------------------------------------------------
#define SETTIGNS_EEPROM_SIZE      230
#define STATE_EEPROM_SIZE         26
//----------------------------------------------------------------------------
#define NUM_WIFI_SETTINGS         9
#define NUM_STATE_SETTINGS        5
#define MAX_LENGTH_SETTING        20
//----------------------------------------------------------------------------
#define ON_PAYLOAD                "ON"
#define OFF_PAYLOAD               "OFF"
//----------------------------------------------------------------------------
#define JSON_MSG_LENGTH           128
//----------------------------------------------------------------------------
#define NULL_CHAR                 '\0'
#define DEFAULT_AP_NAME           "SonoffLedStrip"
//----------------------------------------------------------------------------
#define FW_VERSION                "44.1.1.0"
//----------------------------------------------------------------------------
//---To manage the OTA Drive api key------------------------------------------
#define FAKE_API_KEY              "abc12345-1a1a-12a1-a1ab-a1ab1a1ab123"
#define FW_URL_MKR(k, v)          "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"
//----------------------------------------------------------------------------
bool pushButton                   = false;
bool pushButtonPre                = false;
bool deviceConnected              = false;
bool wifiConfigured               = false;
bool mqttConfigured               = false;
//----------------------------------------------------------------------------
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
String mac                        = "";
//----------------------------------------------------------------------------
struct WifiSetup{
  String Name;
  String Val;
};

WifiSetup Hostname                = {"Hostname", ""};
WifiSetup Ssid                    = {"Ssid", ""};
WifiSetup Password                = {"Password", ""};
WifiSetup MqttSubTopic            = {"MQTT_Sub", ""};
WifiSetup MqttPubTopic            = {"MQTT_Pub", ""};
WifiSetup MqttServer              = {"MQTT_Server", ""};
WifiSetup MqttUser                = {"MQTT_User", ""};
WifiSetup MqttPassword            = {"MQTT_Password", ""};
WifiSetup OtaDriveProductKey      = {"OTA_DRIVE_KEY", ""};
  
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
//----------------------------------------------------------------------------
struct Setting{
  String Name;
  byte Val;
};

Setting SwitchState               = {"SwitchState", 0};
Setting Red                       = {"Red", MAX_RGB_VAL};
Setting Green                     = {"Green", MAX_RGB_VAL};
Setting Blue                      = {"Blue", MAX_RGB_VAL};
Setting Brightness                = {"Brightness", MAX_RGB_VAL / 3};

Setting* StateSettings[NUM_STATE_SETTINGS] = {
  &SwitchState, 
  &Red, 
  &Green, 
  &Blue, 
  &Brightness};
//----------------------------------------------------------------------------
struct RbgRandomEffect{
  String Name;
  byte TargetVal;
};

RbgRandomEffect RedMax            = {"RedMax", MAX_RGB_VAL};
RbgRandomEffect GreenMax          = {"GreenMax", MAX_RGB_VAL};
RbgRandomEffect BlueMax           = {"BlueMax", MAX_RGB_VAL};
RbgRandomEffect RedMin            = {"RedMin", 0};
RbgRandomEffect GreenMin          = {"GreenMin", 0};
RbgRandomEffect BlueMin           = {"BlueMin", 0};
RbgRandomEffect RedHalf           = {"RedHalf", MAX_RGB_VAL / 2};
RbgRandomEffect GreenHalf         = {"GreenHalf", MAX_RGB_VAL / 2};
RbgRandomEffect BlueHalf          = {"BlueHalf", MAX_RGB_VAL / 2};

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
//----------------------------------------------------------------------------
struct RbgEffect{
  String Effect_Name;
  byte Effect_Number;
};

RbgEffect Solid                   = {"Solid", 0};
RbgEffect Random                  = {"Random", 1};

RbgEffect* RbgEffects[RGB_EFFECTS_NUM] = {
  &Solid, 
  &Random};
//----------------------------------------------------------------------------
