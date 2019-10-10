/**
 * WebServer for ConfigTool.
 */

#include <ConfigTool.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "<ssid>";
const char* password = "<pwd>";

String config_String_1 = "Default";
String config_String_2 = "Test";
int    config_int_1    = 100;
int    config_int_2    = 200;
bool   config_bool_F   = false;
bool   config_bool_T   = true;

ConfigTool configTool;
WebServer server(80);

void initVariables() {
  configTool.addVariable("String1", &config_String_1);
  configTool.addVariable("String2", &config_String_2);
  configTool.addVariable("int___1", &config_int_1);
  configTool.addVariable("int___2", &config_int_2);
  configTool.addVariable("bool__F", &config_bool_F);
  configTool.addVariable("bool__T", &config_bool_T);
}


void handleRoot() {
  server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
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

void setup(){
  Serial.begin(115200);
  Serial.println("Setup begin");
  
  initVariables();
  configTool.load();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/config", configTool.getWebHandler(&server));

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}