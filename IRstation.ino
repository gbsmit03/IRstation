#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>

const char* ssid     = ""; //Your Networkd SSID
const char* password = ""; //Your Networkd Password

const int ACTION_LED = 2;
const int POWER_LED = 13;
const int ON = 1;
const int OFF = 0;

ESP8266WebServer server(80);
IRsend irsend(4); //an IR emitter led is connected to GPIO pin 4

void handleRoot() {
  digitalWrite(ACTION_LED, ON);
  server.send(200, "text/plain", "hello from IRstation!");
  digitalWrite(ACTION_LED, OFF);
}

void handleRemote() {
  digitalWrite(ACTION_LED, 1);

  String codeStr = server.arg("code");
  char * temp;
  Serial.print("codeStr: " + codeStr + "\n");

  long codeLong = strtol(codeStr.c_str(), &temp, 0);

  if (*temp == '\0') {
    //TODO need to support wouth bit length and send types.
    irsend.sendNEC(codeLong, 32);
    server.send(204, "text/plain", "");
  } else {
    server.send(400, "text/plain", "remote code parameter is not a valid number");
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
  digitalWrite(ACTION_LED, OFF);
  digitalWrite(POWER_LED, OFF);

  irsend.begin();
  Serial.begin(115200);
  digitalWrite(POWER_LED, OFF);
  delay(100);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ACTION_LED, ON);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(ACTION_LED, OFF);
  digitalWrite(POWER_LED, ON);

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
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void loop(void){
  server.handleClient();
      delay(250);
}
