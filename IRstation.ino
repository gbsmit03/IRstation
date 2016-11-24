#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>

const char* ssid     = ""; //Your Networkd SSID
const char* password = ""; //Your Networkd Password

//https://sourceforge.net/p/lirc-remotes/code/ci/master/tree/remotes/lg/6710T00009B.lircd.conf
const unsigned long LG_POWER = 0x20DF10EF;
const unsigned long LG_VOLUMEUP = 0x20DF40BF;
const unsigned long LG_VOLUMEDOWN = 0x20DFC03F;
const unsigned long LG_MUTE = 0x20DF906F; //TODO need to test
const unsigned long LG_CHANNELUP = 0x20DF00FF; //TODO need to test
const unsigned long LG_CHANNELDOWN = 0x20DF807F; //TODO need to test
const unsigned long LG_SOURCE = 0x20DF19E6; //TODO need to test


ESP8266WebServer server(80);
IRsend irsend(4); //an IR emitter led is connected to GPIO pin 4

const int ACTION_LED = 2;
const int POWER_LED = 13;

void handleRoot() {
  digitalWrite(ACTION_LED, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(ACTION_LED, 0);
}

void handleRemote() {
  digitalWrite(ACTION_LED, 1);
  //FIXME there will only be one power code
  String power = server.arg("power");
  String volume = server.arg("volume");
  String channel = server.arg("channel");
  String mute = server.arg("mute");
  String source = server.arg("source");
  String ON = "ON"; //TODO
  String UP = "UP";
  String DOWN = "DOWN";

  if(ON.equalsIgnoreCase(power)) {
    server.send(200, "text/plain", "turning TV on");
    irsend.sendNEC(LG_POWER, 32);
  } else if (ON.equalsIgnoreCase(mute)) {
    server.send(200, "text/plain", "mute");
    irsend.sendNEC(LG_MUTE, 32);
  } else if (ON.equalsIgnoreCase(source)) {
    server.send(200, "text/plain", "source");
    irsend.sendNEC(LG_SOURCE, 32);
  }else if (UP.equalsIgnoreCase(volume)) {
    server.send(200, "text/plain", "volume up");
    irsend.sendNEC(LG_VOLUMEUP, 32);
  } else if (DOWN.equalsIgnoreCase(volume)) {
    server.send(200, "text/plain", "volume down");
    irsend.sendNEC(LG_VOLUMEDOWN, 32);
  } else if (UP.equalsIgnoreCase(channel)) {
    server.send(200, "text/plain", "channel up");
    irsend.sendNEC(LG_CHANNELUP, 32);
  } else if (DOWN.equalsIgnoreCase(channel)) {
    server.send(200, "text/plain", "channel down");
    irsend.sendNEC(LG_CHANNELDOWN, 32);
  } else {
    server.send(200, "text/plain", "unknown command");
  }
  digitalWrite(ACTION_LED, 0);
}

void handleNotFound(){
  digitalWrite(ACTION_LED, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(ACTION_LED, 0);
}

void setup(void){
  pinMode(POWER_LED, OUTPUT);
  pinMode(ACTION_LED, OUTPUT);
  digitalWrite(ACTION_LED, 0);
  digitalWrite(POWER_LED, 0);

  irsend.begin();
  Serial.begin(115200);
  digitalWrite(POWER_LED, 1);
  delay(100);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ACTION_LED, 1);
    delay(500);
    digitalWrite(ACTION_LED, 0);
    Serial.print(".");
  }
  digitalWrite(POWER_LED, 1);
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/remote", handleRemote);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


}

void loop(void){
  server.handleClient();
      delay(1000);
}
