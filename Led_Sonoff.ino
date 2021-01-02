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
StoreStrings mem(EEPROM_SIZE);
CRGB leds[NUM_LEDS];
DynamicJsonDocument message(JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 60);

void setup() {
  Serial.begin(115200);
  delay(500);

  Rele.Begin();
  Led.Begin();
  pinMode(SONOFF_LEDSTRIP, OUTPUT);
  FastLED.addLeds<WS2811, SONOFF_LEDSTRIP, BRG>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  pushButton = !Button.State();
  pushButtonPre = pushButton;

  Serial.println("\n----------------------");
  Serial.println("Application ver. " + String(Version) + " running!");
  Connection_Manager();
  randomSeed(millis());
}

void loop() {
  if (deviceConnected) {
    if (client.connected()) {
      client.loop();
    } else {
      client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str());
      client.subscribe(MqttSubTopic.Val.c_str());
      Serial.println("Mqtt client reconnected");
    }
  }
  server.handleClient();

  // Controllo connessione
  if (millis() - lastTimeCheckConn > TIMER_CONNECTION) {
    // Blink di check connessione
    Serial.print("Connection check... ");
    Led.Off();
    delay(PLACING_TIME);
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
    delay(PLACING_TIME);
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
    if (millis() - pushButtonTime > (BLINK_TIME - TIME_FLASH_BLINK)) {
      pushButtonTime = millis();
      Led.Blink(PLACING_TIME, 1, TIME_FLASH_BLINK);
      pushButtonCount ++;
    }
  }

  if (RGB_Efect_Selected != Solid.Effect_Number) {
    if (millis() - lastTimeRgbPlay >= TIMER_RGB_PLAY) {
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
        R = RgbChannelManager(R, Random_Effects[0]->TargetVal, INCREMENT_STEP);
        break;
      case 1:
        // Alza il verde al max
        G = RgbChannelManager(G, Random_Effects[1]->TargetVal, INCREMENT_STEP);
        break;
      case 2:
        // Alza il blu al max
        B = RgbChannelManager(B, Random_Effects[2]->TargetVal, INCREMENT_STEP);
        break;
      case 3:
        // Abbassa il rosso al max
        R = RgbChannelManager(R, Random_Effects[3]->TargetVal, INCREMENT_STEP);
        break;
      case 4:
        // Abbassa il verde al max
        G = RgbChannelManager(G, Random_Effects[4]->TargetVal, INCREMENT_STEP);
        break;
      case 5:
        // Abbassa il blu al max
        B = RgbChannelManager(B, Random_Effects[5]->TargetVal, INCREMENT_STEP);
        break;
      case 6:
        // Canale rosso a metà
        R = RgbChannelManager(R, Random_Effects[6]->TargetVal, INCREMENT_STEP);
        break;
      case 7:
        // Canale verde a metà
        G = RgbChannelManager(G, Random_Effects[7]->TargetVal, INCREMENT_STEP);
        break;
      case 8:
        // Canale blu a metà
        B = RgbChannelManager(B, Random_Effects[8]->TargetVal, INCREMENT_STEP);
        break;
      default:
        RGB_Efect_Selected = Solid.Effect_Number;
        break;
    }
    SetLedStrip(R, G, B, Brightness);
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
  while ((Random_Effects[randSelection]->TargetVal == 0 ) && ((R == 0 && G == 0) || (R == 0 && B == 0) || (B == 0 && G == 0))) {
    randSelection = random(0, RGB_COMBO_NUM);
  }
  return randSelection;
}

void PushButtonFunction(int func) {
  Serial.print("Function: " + String(func) + " -> ");
  switch (func) {
    case 1:
      Serial.println("Toggle");
      delay(500);
      Switch_Toggle();
      break;
    case 2:
      Serial.println("Connection check");
      delay(500);
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
      Serial.println("Load settings");
      LoadSettingsFromEeprom();
      break;
    case 6:
      Serial.println("Save settings");
      SaveSettingsInEeprom();
      break;
    case 7:
      Serial.println("EEPROM clean");
      for (int i = 0; i < EEPROM_SIZE; i++) {
        mem.write(i, String('\0'));
      }
      Restart();
      break;
    case 8:
      Serial.println("Set LedStrip");
      SetLedStrip(R, G, B, Brightness);
      break;
    case 9:
      Serial.println("Wifi signal power");
      getWifiPower(Ssid.Val);
      break;
    case 10:
      Serial.println("RGB Play Toggle");
      if (RGB_Efect_Selected != Solid.Effect_Number) {
        RGB_Efect_Selected = Solid.Effect_Number;
      } else {
        RGB_Efect_Selected = Random.Effect_Number;
      }
      break;
    case 11:
      Serial.println("Sonoff restart");
      Restart();
      break;
    default:
      Serial.println("No function!");
      Led.Blink(PLACING_TIME, 5, TIME_FLASH_BLINK);
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

  if (deviceConnected && (RGB_Efect_Selected == Solid.Effect_Number)) {
    String msg_out;
    JsonObject color = message.createNestedObject("color");
    color["r"] = R;
    color["g"] = G;
    color["b"] = B;
    serializeJson(message, msg_out);
    client.publish(MqttPubTopic.Val.c_str(), msg_out.c_str());
  }
    
  Serial.println("LedStrip updated -> R:" + String(red) + " G:" + String(green) + " B:" + String(blue) + " Brightness:" + String(brightness));
}

void Restart() {
  Serial.println("Restart");
  delay(500);
  Led.Blink(PLACING_TIME, 5, TIME_FLASH_BLINK);
  ESP.restart();
}

void Switch_On() {
  if (Rele.State() == OFF_RELAY) {
    Rele.On();
    
    if (deviceConnected) {
      String msg_out;
      message["state"] = ON_PAYLOAD;
      serializeJson(message, msg_out);
      client.publish(MqttPubTopic.Val.c_str(), msg_out.c_str());
    }
    
    // Riardo per stabilizzazione alimentazione
    delay(1000);
  }
  
  SetLedStrip(R, G, B, Brightness);
}

void Switch_Off() {
  Rele.Off();
  
  if (deviceConnected) {
    String msg_out;
    message["state"] = OFF_PAYLOAD;
    serializeJson(message, msg_out);
    client.publish(MqttPubTopic.Val.c_str(), msg_out.c_str());
  }
  
  RGB_Efect_Selected = Solid.Effect_Number;
}

void Switch_Toggle() {
  if (Rele.State() != OFF_RELAY) {
    Switch_Off();
  } else {
    Switch_On();
  }
}

void Callback(char *topic, byte *payload, unsigned int length) {
  char message_buff[length];
  byte i = 0;

  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  Serial.println("Payload recived: " + String(message_buff));

  if ((String(topic) == MqttSubTopic.Val) && (length > 5)) {
    const char* stat;
    String state;
    Serial.println("Command/s on payload: ");
    deserializeJson(message, payload);
    JsonObject root = message.as<JsonObject>();

    for (JsonPair kv : root) {
      String key = kv.key().c_str();

      if (key.equals("state")) {
        stat = message["state"];
        state = String(stat);
        Serial.println("  - state = " + state);

      } else if (key.equals("brightness")) {
        Brightness = message["brightness"];
        Serial.println("  - Brightness = " + String(Brightness));

      } else if (key.equals("color")) {
        JsonObject color = message["color"];
        R = color["r"];
        G = color["g"];
        B = color["b"];
        Serial.println("  - color = R:" + String(R) + " G:" + String(G) + " B:"  + String(B));

      } else if (key.equals("effect")) {
        const char* eff = message["effect"];
        String effect = String(eff);
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

    Led.Blink(PLACING_TIME, ipNum, PLACING_TIME);

  } else {

    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
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
          Led.Blink(PLACING_TIME, power, PLACING_TIME);
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
  delay(PLACING_TIME);
  LoadSettingsFromEeprom();

  WiFi.mode(WIFI_AP_STA);

  if (Ssid.Val != "" && !Ssid.Val.startsWith(" ")) {
    if (getWifiPower(Ssid.Val)){
      WiFi.begin(Ssid.Val, Password.Val);
      Serial.println("Connecting to: " + String(Ssid.Val));
      byte numCehck = 0;
      while ((WiFi.status() != WL_CONNECTED)) {
        if (numCehck >= 150) {
          lastTimeCheckConn = millis();
          break;
        }
        numCehck++;
        delay(200);
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    client.setServer(MqttServer.Val.c_str(), 1883);
    client.setCallback(Callback);
    client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str());
    client.subscribe(MqttSubTopic.Val.c_str());

    // Accendo led per indicare l'avvenuta connessione
    Led.On();
    deviceConnected = true;

  } else {
    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
    // Lascio spento il led per indicare l'assenza di connessione
    Led.Off();
    deviceConnected = false;
  }

  // Configurazione AP e webServer
  if (Hostname.Val == "" || Hostname.Val.startsWith(" ") || Hostname.Val.length() >= MAX_LENGTH_SETTING) {
    Hostname.Val = DefaultApName;
  }
  Serial.println("AP Name: " + Hostname.Val);

  WiFi.softAP(Hostname.Val.c_str());
  mdns.begin(Hostname.Val, WiFi.localIP());
  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/on", []() {
    server.sendHeader("Location","/");
    server.send(303);
    Switch_On();
  });
  server.on("/off", []() {
    server.sendHeader("Location","/");
    server.send(303);
    Switch_Off();
  });
  server.on("/restart", []() {
    server.sendHeader("Location","/");
    server.send(303);
    Restart();
  });
  server.on("/brigthness", HTTP_POST, ChangeBrigthness);
  server.on("/effect", HTTP_POST, ChangeEffect);
  server.on("/settings", HTTP_POST, ChangeSettings);
  server.on("/color", HTTP_POST, ChangeColor);
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

void ChangeBrigthness() {
  String data = server.arg("plain");
  Serial.print("Brigthness recived: ");
  deserializeJson(message, data);
  JsonObject root = message.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("brigthness")) {
      const char* brigthness = message["brigthness"];
      Serial.println(brigthness);
      Brightness = String(brigthness).toInt();
      SetLedStrip(R, G, B, Brightness);
    }
  }
}

void ChangeEffect() {
  String data = server.arg("plain");
  Serial.print("Effect recived: ");
  deserializeJson(message, data);
  JsonObject root = message.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("effect")) {
      const char* eff = message["effect"];
      String effect = String(eff);
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
  String data = server.arg("plain");
  Serial.print("Color recived: ");
  deserializeJson(message, data);
  JsonObject root = message.as<JsonObject>();
  for (JsonPair kv : root) {
    String key = kv.key().c_str();
    if (key.equals("color")) {
      const char* col = message["color"];
      String color = String(col);
      if (color.length() == 7) {
        R = HexString2Byte(color.substring(1, 3));
        G = HexString2Byte(color.substring(3, 5));
        B = HexString2Byte(color.substring(5, color.length()));
        Serial.println(color + " -> R: " + String(R) + " G: " + String(G) + " B: " + String(B));
        SetLedStrip(R, G, B, Brightness);
      } else {
        Serial.println(color + " -> unable to decode");
      }
    }
  }
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
  int numIteration = 0;
  int startPoint = -1;
  int parNum = 0;
  String valRecived;
  String data = server.arg("plain");

  Serial.println("Parameters recived:");
  Serial.println(data);
  Led.Blink(PLACING_TIME, 3, PLACING_TIME);
  for (int i = 0; i < data.length(); i++) {
    if (data.charAt(i) == '"') {
      if (startPoint == -1) {
        startPoint = i;
      } else {
        valRecived = data.substring(startPoint + 1, i);
        startPoint = -1;
        numIteration++;
        if (numIteration % 2 == 0) {
          Serial.println(valRecived);
          if (valRecived != " " && valRecived != "" && !valRecived.startsWith(" ")) {
            WifiSettings[parNum]->Val = valRecived;
          }
          parNum++;
        } else {
          Serial.print(valRecived + ": ");
        }
      }
    }
  }
  SaveSettingsInEeprom();
  Connection_Manager();
}

void SaveSettingsInEeprom() {
  Serial.println("Saving settings in EEPROM");
  mem.resetWriteCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    mem.write(mem.getLastWrittenByte(), WifiSettings[i]->Val);
  }
  Serial.println("Settings saved!");
}

void LoadSettingsFromEeprom() {
  Serial.println("Loading settings from EEPROM");
  mem.resetReadCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    WifiSettings[i]->Val = mem.read(mem.getLastReadedByte(), MAX_LENGTH_SETTING);
    Serial.println( WifiSettings[i]->Name + ": " +  WifiSettings[i]->Val);
  }
  Serial.println("Settings loaded!");
}

void OtaUpdate() {
  String url = "http://otadrive.com/DeviceApi/GetEsp8266Update?";
  url += "&s=" + String(CHIPID);
  url += MakeFirmwareInfo(ProductKey, Version);

  Serial.println("Get firmware from url:");
  Serial.println(url);

  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, url, Version);
  
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.println("Update faild!");
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
    break;
  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("No new update available");
    Led.Blink(PLACING_TIME, 3, TIME_FLASH_BLINK);
    break;
  case HTTP_UPDATE_OK:
    Serial.println("Update OK");
    Led.Blink(PLACING_TIME, 5, TIME_FLASH_BLINK);
    break;
  default:
    break;
  }
}
