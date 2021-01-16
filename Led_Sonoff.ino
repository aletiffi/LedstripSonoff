#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ReadInput.h>
#include <DigiOut.h>
#include <StoreStrings.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>
#include "GlobalVar.h"
#include "HomePage.h"

WiFiClient espClient;
PubSubClient client (espClient);
ESP8266WebServer server(80);
MDNSResponder mdns;
ReadInput Button(SONOFF_BUTTON);
DigiOut Led(SONOFF_LED, OFF_LED);
DigiOut Rele(SONOFF_RELAY, OFF_RELAY);
StoreStrings mem(SETTIGNS_EEPROM_SIZE, STATE_EEPROM_SIZE);
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(T_250MS);

  if (!mem.isReady()) {
    mem.clear();
  }

  Rele.Begin();
  Led.Begin();
  pinMode(SONOFF_LEDSTRIP, OUTPUT);
  FastLED.addLeds<WS2811, SONOFF_LEDSTRIP, BRG>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  pushButton = !Button.State();
  pushButtonPre = pushButton;
  FastLED.clear();
  FastLED.show();

  Serial.println("\n----------------------");
  Serial.println("Application ver. " + String(Version) + " running!");
  LoadLedStripStateFromEeprom();
  if (SwitchState.Val == 1) {
    Rele.On();
    SetLedStrip(Red.Val, Green.Val, Blue.Val, Brightness.Val);
  } else {
    Rele.Off();
  }
  Connection_Manager();
  randomSeed(millis());
}

void loop() {
  if (deviceConnected && mqttConfigured) {
    if (client.connected()) {
      client.loop();
    } else {
      if (millis() - lastMqttCheckConn > T_5S) {
        Serial.print("Connecting Mqtt client... ");
        lastMqttCheckConn = millis();
        // Prova a riconnettere
        if (client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str())) {
          lastMqttCheckConn = 0;
          client.subscribe(MqttSubTopic.Val.c_str());
          Serial.println("Ok");
          PublishState();
        } else {
          Serial.println("Failed");
        }
      }
    }
  }
  server.handleClient();
  mdns.update();

  // Controllo connessione
  if (wifiConfigured && millis() - lastTimeCheckConn > T_5MIN) {
    // Blink di check connessione
    Serial.print("Connection check... ");
    Led.Off();
    delay(T_200MS);
    lastTimeCheckConn = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Connection_Manager();
    } else {
      Serial.println("OK");
      OtaUpdate();
      // Accendo led di conferma connessione alla rete
      Led.On();
    }
  }

  // Controllo presione pulsante multifunzione
  pushButton = !Button.State();

  if (pushButton && !pushButtonPre) {
    Led.Off();
    delay(T_200MS);
    pushButtonTime = millis();
  }
  if (pushButtonPre && !pushButton) {
    PushButtonFunction(pushButtonCount);
    pushButtonCount = 0;
    if (deviceConnected) {
      Led.On();
    }
  }
  pushButtonPre = pushButton;

  if (pushButton) {
    if (millis() - pushButtonTime > (T_1S - T_100MS)) {
      pushButtonTime = millis();
      Led.Blink(T_200MS, 1, T_100MS);
      pushButtonCount ++;
    }
  }

  if (RGB_Efect_Selected != Solid.Effect_Number) {
    if (millis() - lastTimeRgbPlay >= T_250MS) {
      lastTimeRgbPlay = millis();
      RgbEffectPlay();
    }
  }
}

void RgbEffectPlay() {
  if (RGB_Efect_Selected == Random.Effect_Number) {
    switch (RandomEffectStep) {
      case 0:
        // Alza il rosso al max
        Red.Val = RgbChannelManager(Red.Val, Random_Effects[0]->TargetVal, INCREMENT_STEP);
        break;
      case 1:
        // Alza il verde al max
        Green.Val = RgbChannelManager(Green.Val, Random_Effects[1]->TargetVal, INCREMENT_STEP);
        break;
      case 2:
        // Alza il blu al max
        Blue.Val = RgbChannelManager(Blue.Val, Random_Effects[2]->TargetVal, INCREMENT_STEP);
        break;
      case 3:
        // Abbassa il rosso al max
        Red.Val = RgbChannelManager(Red.Val, Random_Effects[3]->TargetVal, INCREMENT_STEP);
        break;
      case 4:
        // Abbassa il verde al max
        Green.Val = RgbChannelManager(Green.Val, Random_Effects[4]->TargetVal, INCREMENT_STEP);
        break;
      case 5:
        // Abbassa il blu al max
        Blue.Val = RgbChannelManager(Blue.Val, Random_Effects[5]->TargetVal, INCREMENT_STEP);
        break;
      case 6:
        // Canale rosso a metà
        Red.Val = RgbChannelManager(Red.Val, Random_Effects[6]->TargetVal, INCREMENT_STEP);
        break;
      case 7:
        // Canale verde a metà
        Green.Val = RgbChannelManager(Green.Val, Random_Effects[7]->TargetVal, INCREMENT_STEP);
        break;
      case 8:
        // Canale blu a metà
        Blue.Val = RgbChannelManager(Blue.Val, Random_Effects[8]->TargetVal, INCREMENT_STEP);
        break;
      default:
        RGB_Efect_Selected = Solid.Effect_Number;
        break;
    }
    SetLedStrip(Red.Val, Green.Val, Blue.Val, Brightness.Val);
  }
}

byte RgbChannelManager(byte channelVal, byte targetVal, byte incStep) {
  if (channelVal < targetVal) {
    // Aumenta
    if ((targetVal - channelVal) < incStep) {
      return targetVal;
    } else {
      return channelVal += incStep;
    }
  } else if (channelVal > targetVal) {
    // Diminuisce
    if (channelVal < incStep) {
      return targetVal;
    } else {
      return channelVal -= incStep;
    }
  } else {
    // Cambia
    RandomEffectStep = RandomEffectStepSelector();
    return channelVal;
  }
}

byte RandomEffectStepSelector() {
  byte randSelection = random(0, RGB_COMBO_NUM);
  // Controlla che non vengano spenti tutti i canali
  while ((Random_Effects[randSelection]->TargetVal == 0 ) && ((Red.Val == 0 && Green.Val == 0) || (Red.Val == 0 && Blue.Val == 0) || (Blue.Val == 0 && Green.Val == 0))) {
    randSelection = random(0, RGB_COMBO_NUM);
  }
  return randSelection;
}

void PushButtonFunction(int func) {
  Serial.print("Function: " + String(func) + " -> ");
  switch (func) {
    case 1:
      Serial.println("Toggle");
      delay(T_250MS);
      Switch_Toggle();
      break;
    case 2:
      Serial.println("Connection check");
      delay(T_250MS);
      Connection_Manager();
      break;
    case 3:
      Serial.println("Check FW update");
      OtaUpdate();
      break;
    case 4:
      Serial.println("Show ip address");
      ShowIpAddr();
      break;
    case 5:
      Serial.println("Wifi signal power");
      getWifiPower(Ssid.Val);
      break;
    case 6:
      Serial.println("Load settings");
      LoadSettingsFromEeprom();
      break;
    case 7:
      Serial.println("Save settings");
      SaveSettingsInEeprom();
      break;
    case 8:
      Serial.println("EEPROM clean");
      CleanEEPROM();
      break;
    case 9:
      Serial.println("Read all EEPROM");
      mem.print_all();
      break;
    case 10:
      Serial.println("Sonoff restart");
      Restart();
      break;
    default:
      Serial.println("No function!");
      Led.Blink(T_200MS, 5, T_100MS);
      break;
  }
}

void SetLedStrip(byte red, byte green, byte blue, byte brightness) {
  for (byte j = 0; j < NUM_LEDS; j++) {
    leds[j] = CRGB(red, green, blue);
  }
  FastLED.show();
  FastLED.setBrightness(brightness);
  FastLED.show();
  Serial.println("LedStrip updated -> R:" + String(red) + " G:" + String(green) + " B:" + String(blue) + " Brightness:" + String(brightness));
  if (deviceConnected && RGB_Efect_Selected == Solid.Effect_Number) {
    PublishState();
  }
}

void Restart() {
  Serial.println("Restart");
  delay(T_250MS);
  Led.Blink(T_200MS, 5, T_100MS);
  ESP.restart();
}

void Switch_On() {
  if (Rele.State() == OFF_RELAY) {
    Rele.On();
    SwitchState.Val = 1;
    // Riardo per stabilizzazione alimentazione
    delay(T_1S);
  }
  SetLedStrip(Red.Val, Green.Val, Blue.Val, Brightness.Val);
  SaveLedStripStateInEeprom();
}

void Switch_Off() {
  FastLED.clear();
  FastLED.show();
  Rele.Off();
  SwitchState.Val = 0;

  if (deviceConnected) {
    PublishState();
  }
  RGB_Efect_Selected = Solid.Effect_Number;
  SaveLedStripStateInEeprom();
}

void Switch_Toggle() {
  if (Rele.State() != OFF_RELAY) {
    Switch_Off();
  } else {
    Switch_On();
  }
}

void PublishState() {
  DynamicJsonDocument doc(JSON_MSG_LENGTH);
  char msg_out[JSON_MSG_LENGTH];

  if (Rele.State() == OFF_RELAY) {
    doc["state"] = OFF_PAYLOAD;
  } else {
    doc["state"] = ON_PAYLOAD;
    doc["brightness"] = Brightness.Val;
    JsonObject color = doc.createNestedObject("color");
    color["r"] = Red.Val;
    color["g"] = Green.Val;
    color["b"] = Blue.Val;
  }
  serializeJson(doc, msg_out);
  if (client.publish(MqttPubTopic.Val.c_str(), msg_out)) {
    Serial.print("Message published: ");
    Serial.println(msg_out);
  } else {
    Serial.println("Message publishing error");
  }
}

void Callback(char *topic, byte *payload, unsigned int length) {
  DynamicJsonDocument doc(JSON_MSG_LENGTH);
  Serial.print("Payload recived: ");
  for (byte i = 0; i < length; i++) {
    Serial.print(char(payload[i]));
  }
  Serial.println("");

  if ((String(topic) == MqttSubTopic.Val) && (length > 5)) {
    const char* stat;
    String state;
    Serial.println("Command/s on payload: ");
    deserializeJson(doc, payload, length);
    JsonObject root = doc.as<JsonObject>();

    for (JsonPair kv : root) {
      String key = kv.key().c_str();

      if (key.equals("state")) {
        stat = doc["state"];
        state = String(stat);
        Serial.println("  - state = " + state);

      } else if (key.equals("brightness")) {
        Brightness.Val = doc["brightness"];
        Serial.println("  - Brightness = " + String(Brightness.Val));

      } else if (key.equals("color")) {
        JsonObject color = doc["color"];
        Red.Val = color["r"];
        Green.Val = color["g"];
        Blue.Val = color["b"];
        Serial.println("  - color = R:" + String(Red.Val) + " G:" + String(Green.Val) + " B:"  + String(Blue.Val));

      } else if (key.equals("effect")) {
        String effect = doc["effect"];
        for (byte i = 0; i < RGB_EFFECTS_NUM; i++) {
          if (effect.equals(RbgEffects[i]->Effect_Name)) {
            RGB_Efect_Selected = RbgEffects[i]->Effect_Number;
            Serial.println("  - effect = " + effect);
          }
        }

      } else {
        Serial.println("  - Key unknown recived = " + key + kv.value().as<char*>());
      }
    }

    if (state.equals(OFF_PAYLOAD)) {
      Switch_Off();
    }
    if (state.equals(ON_PAYLOAD)) {
      Switch_On();
    }
  }
}

void ShowIpAddr() {
  if (deviceConnected) {
    String ip = WiFi.localIP().toString();
    byte numIterations = 0;
    String ipNumStr = "0";
    byte ipNum = 0;

    Serial.println(ip);
    for (int i = 0; i < ip.length(); i++) {
      if (ip.charAt(i) == '.') {
        numIterations++;
      }
      if (numIterations == 3) {
        ipNumStr = ip.substring(i + 1, ip.length());
        ipNum = ipNumStr.toInt();
        break;
      }
    }

    Led.Blink(T_200MS, ipNum, T_200MS);

  } else {

    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(T_200MS, 10, T_100MS);
  }
}

bool getWifiPower(String netName) {
  int netsNumber = WiFi.scanNetworks();
  int minRawLevel = -75;
  int maxRawLevel = -45;
  int minLevel = 0;
  int maxLevel = 5;
  bool result = false;

  if (netsNumber > 0) {
    for (int i = 0; i < netsNumber; ++i) {
      if (String(WiFi.SSID(i)) == netName) {
        result = true;
        int rawPower = WiFi.RSSI(i);
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(rawPower);
        Serial.print(")");
        if (minRawLevel <= rawPower && rawPower <= maxRawLevel) {
          int power = map(rawPower, minRawLevel, maxRawLevel, minLevel, maxLevel);
          switch (power) {
            case 0:
              Serial.println(" Unreacheable");
              break;
            case 1:
              Serial.println(" Very Low");
              break;
            case 2:
              Serial.println(" Low");
              break;
            case 3:
              Serial.println(" Good");
              break;
            case 4:
              Serial.println(" High");
              break;
            case 5:
              Serial.println(" Very High!");
              break;
          }
          Led.Blink(T_200MS, power, T_200MS);
        } else {
          Serial.println(" Signal level out of bounds");
        }
      }
    }
  }
  return result;
}

void Connection_Manager() {
  Serial.println("Connection process started");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(T_200MS);
  LoadSettingsFromEeprom();

  WiFi.mode(WIFI_AP_STA);

  if (Ssid.Val != NULL_CHAR && Password.Val != NULL_CHAR) {
    if (getWifiPower(Ssid.Val)) {
      WiFi.begin(Ssid.Val, Password.Val);
      Serial.println("Connecting to: " + String(Ssid.Val));
      byte numCehck = 0;
      while ((WiFi.status() != WL_CONNECTED)) {
        if (numCehck >= 150) {
          lastTimeCheckConn = millis();
          break;
        }
        numCehck++;
        delay(T_200MS);
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    if (mac.equals("")) {
      String macTemp = String(WiFi.macAddress());
      for (byte i = 0; i < macTemp.length(); i++) {
        if (macTemp.charAt(i) != ':') {
          mac += macTemp.charAt(i);
        }
      }
    }
    Serial.println("IP: " + WiFi.localIP().toString() + " Mac: " + String(WiFi.macAddress()));
    client.setServer(MqttServer.Val.c_str(), 1883);
    client.setCallback(Callback);

    // Accendo led per indicare l'avvenuta connessione
    Led.On();
    deviceConnected = true;

  } else {
    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(T_200MS, 10, T_100MS);
    // Lascio spento il led per indicare l'assenza di connessione
    Led.Off();
    deviceConnected = false;
  }

  // Configurazione AP e webServer
  if (Hostname.Val == "" || Hostname.Val.startsWith(" ") || Hostname.Val.length() >= MAX_LENGTH_SETTING) {
    Hostname.Val = DEFAULT_AP_NAME;
  }
  Serial.println("AP Name: " + Hostname.Val);

  WiFi.softAP(Hostname.Val.c_str());
  mdns.begin(Hostname.Val, WiFi.localIP());
  mdns.addService("http", "tcp", 80);
  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/on", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Switch_On();
  });
  server.on("/off", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Switch_Off();
  });
  server.on("/restart", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Restart();
  });
  server.on("/brigthness", HTTP_POST, ChangeBrightness);
  server.on("/effect", HTTP_POST, ChangeEffect);
  server.on("/settings", HTTP_POST, ChangeSettings);
  server.on("/color", HTTP_POST, ChangeColor);
  server.on("/clean", HTTP_GET, CleanEEPROM);
  server.onNotFound(handleNotFound);
  server.begin();

  // Azzero ultimo tempo di connessione
  lastTimeCheckConn = millis();
}

void handleNotFound() {
  String message = "Err 404\nPage Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void ChangeBrightness() {
  DynamicJsonDocument doc(JSON_MSG_LENGTH);
  String data = server.arg("plain");
  Serial.print("Brightness recived: ");
  deserializeJson(doc, data);
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("Brightness")) {
      String brigthness = doc["Brightness"];
      Serial.println(brigthness);
      Brightness.Val = brigthness.toInt();
      SetLedStrip(Red.Val, Green.Val, Blue.Val, Brightness.Val);
    }
  }
}

void ChangeEffect() {
  DynamicJsonDocument doc(JSON_MSG_LENGTH);
  String data = server.arg("plain");
  Serial.print("Effect recived: ");
  deserializeJson(doc, data);
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("effect")) {
      String effect = doc["effect"];
      for (byte i = 0; i < RGB_EFFECTS_NUM; i++) {
        if (effect.equals(RbgEffects[i]->Effect_Name)) {
          RGB_Efect_Selected = RbgEffects[i]->Effect_Number;
          Serial.println(effect);
        }
      }
    }
  }
}

void ChangeColor() {
  DynamicJsonDocument doc(JSON_MSG_LENGTH);
  String data = server.arg("plain");
  Serial.print("Color recived: ");
  deserializeJson(doc, data);
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("color")) {
      String color = doc["color"];
      if (color.length() == 7) {
        Red.Val = HexString2Byte(color.substring(1, 3));
        Green.Val = HexString2Byte(color.substring(3, 5));
        Blue.Val = HexString2Byte(color.substring(5, color.length()));
        Serial.println(color + " -> R: " + String(Red.Val) + " G: " + String(Green.Val) + " B: " + String(Blue.Val));
        SetLedStrip(Red.Val, Green.Val, Blue.Val, Brightness.Val);
      } else {
        Serial.println(color + " -> unable to decode");
      }
    }
  }
}

void CleanEEPROM() {
  Led.Blink(T_200MS, 3, T_200MS);
  mem.clear();
  mem.print_all();
  Led.Blink(T_200MS, 3, T_200MS);
  Restart();
}

byte HexString2Byte(String val) {
  byte res = 1;
  if (val.length() <= 2 && val.length() > 0) {
    for (byte k = 0; k < 2; k++) {
      if (isHexadecimalDigit(val.charAt(k))) {
        if (isAlpha(val.charAt(k))) {
          res *= char2byte(val.charAt(k));
        } else {
          res *= byte(val.charAt(k));
        }
      } else {
        return 0;
      }
    }
    return res;
  }
  return 0;
}

byte char2byte (char c) {
  if (c == 'a' || c == 'A') {
    return 10;
  } else if (c == 'b' || c == 'B') {
    return 11;
  }  else if (c == 'c' || c == 'C') {
    return 12;
  }  else if (c == 'd' || c == 'D') {
    return 13;
  }  else if (c == 'e' || c == 'E') {
    return 14;
  }  else if (c == 'f' || c == 'F') {
    return 15;
  } else {
    return 1;
  }
}

void ChangeSettings() {
  DynamicJsonDocument doc(512);
  String data = server.arg("plain");
  Serial.println("Parameters recived:");
  Serial.println(data);
  deserializeJson(doc, data);
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
      if (key.equals(WifiSettings[i]->Name)) {
        String temp = doc[key];
        if (temp != " " && temp != "" && !temp.startsWith(" ")) {
          WifiSettings[i]->Val = temp;
        }
      }
    }
  }

  SaveSettingsInEeprom();
  Connection_Manager();
}

void SaveSettingsInEeprom() {
  mem.resetWriteCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    mem.write(mem.getLastWrittenByte(), WifiSettings[i]->Val);
  }
}

void LoadSettingsFromEeprom() {
  Serial.println("Loading settings from EEPROM... ");
  mem.resetReadCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    WifiSettings[i]->Val = mem.read(mem.getLastReadedByte());
    Serial.println( WifiSettings[i]->Name + ": " +  WifiSettings[i]->Val);
  }

  if ( MqttSubTopic.Val != NULL_CHAR && MqttPubTopic.Val != NULL_CHAR && MqttServer.Val != NULL_CHAR &&
       MqttUser.Val != NULL_CHAR && MqttPassword.Val != NULL_CHAR) {
    mqttConfigured = true;
  } else {
    mqttConfigured = false;
  }

  if (Ssid.Val != NULL_CHAR && Password.Val != NULL_CHAR) {
    wifiConfigured = true;
  } else {
    wifiConfigured = false;
  }
}

void SaveLedStripStateInEeprom() {
  mem.resetWriteCounter2();
  for (byte i = 0; i < NUM_STATE_SETTINGS; i++) {
    mem.write_pt2(mem.getLastWrittenByte2(), String(StateSettings[i]->Val));
  }
}

void LoadLedStripStateFromEeprom() {
  Serial.println("Loading last state from EEPROM");
  mem.resetReadCounter2();
  for (byte i = 0; i < NUM_STATE_SETTINGS; i++) {
    StateSettings[i]->Val = mem.read_pt2(mem.getLastReadedByte2()).toInt();
    Serial.print( StateSettings[i]->Name + ": " +  StateSettings[i]->Val + " ");
  }
  Serial.println("");
}

void OtaUpdate() {
  //----------------------------------------------------------------------------
  //---Per upload file .bin in OTA Drive non toccare----------------------------
  String fake_url = "http://otadrive.com/DeviceApi/GetEsp8266Update?";
  fake_url += "&s=" + mac;
  fake_url += MakeFirmwareInfo(ProductKey, Version);
  //----------------------------------------------------------------------------

  String url = "http://otadrive.com/DeviceApi/GetEsp8266Update?";
  url += "&s=" + mac + "&_FirmwareInfo&k=" + OtaDriveProductKey.Val + "&v=" + Version + "&FirmwareInfo_&";

  Serial.println("Get firmware from url:");
  Serial.println(url);

  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, url, Version);
  switch (ret)
  {
    case HTTP_UPDATE_FAILED:
      Serial.printf("Update Faild, Error: (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No new update available");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update OK");
      break;
    default:
      Serial.println(ret);
      break;
  }
}
