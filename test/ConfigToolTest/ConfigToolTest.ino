/**
 * Test functionality of ConfigTool.
 * 
 * TAKE CARE !!! This DELETES exisintg "/config.json" files !!!
 * 
 */


#include <SPIFFS.h>
#include <ConfigTool.h>

String config_String_1 = "Default";
String config_String_2 = "Test";
int    config_int_1    = 100;
int    config_int_2    = 200;
bool   config_bool_F   = false;
bool   config_bool_T   = true;

ConfigTool configTool;

bool dumpConfig(bool check){

  File file = SPIFFS.open("/config.json");
  if(!file){
    Serial.println("reading - config failed");
    return false;
  }

  Serial.println("reading - dumpfile:");
  while(file.available()){
    Serial.write(file.read());
  }
  Serial.println();
  // if requested, print the comparison for "zeroVariables":
  if (check) Serial.println("{\"String1\":\"-1-\",\"String2\":\"-2-\",\"bool__F\":false,\"bool__T\":false,\"int___1\":0,\"int___2\":0}");
  
  return true;
}

void initVariables() {
  configTool.addVariable("String1", &config_String_1);
  configTool.addVariable("String2", &config_String_2);
  configTool.addVariable("int___1", &config_int_1);
  configTool.addVariable("int___2", &config_int_2);
  configTool.addVariable("bool__F", &config_bool_F);
  configTool.addVariable("bool__T", &config_bool_T);
}

void zeroVariables() {
  config_String_1 = "-1-";
  config_String_2 = "-2-";
  config_int_1    = 0;
  config_int_2    = 0;
  config_bool_F   = false;
  config_bool_T   = false;
}

void checkDefaultValues() {
  Serial.printf("String1 OK=%d\n", (config_String_1 == "Default"));
  Serial.printf("String2 OK=%d\n", (config_String_2 == "Test"));
  Serial.printf("int1    OK=%d\n", (config_int_1    == 100));
  Serial.printf("int2    OK=%d\n", (config_int_2    == 200));
  Serial.printf("bool_F  OK=%d\n", (config_bool_F   == false));
  Serial.printf("bool_T  OK=%d\n", (config_bool_T   == true));
}

void setup(){
  Serial.begin(115200);
  Serial.println("Setup begin");
  
  if(!SPIFFS.begin(false)){
    Serial.println("setup - SPIFFS begin failed");
    return;
  }

  Serial.println("setup - dump at start");
  dumpConfig(true);

  Serial.println("setup - init variables to default and save");
  initVariables();
  configTool.save();
  dumpConfig(false);

  Serial.println("setup - manipulate variables and load again");
  zeroVariables();
  configTool.load();
  checkDefaultValues();

  Serial.println("setup - manipulate variables and save");
  zeroVariables();
  configTool.save();
  dumpConfig(true);

  Serial.println("setup - reset stored values");
  configTool.reset();
  dumpConfig(false);
  checkDefaultValues();

  Serial.println("setup - again manipulate variables and save");
  zeroVariables();
  configTool.save();
  dumpConfig(true);

  Serial.println( "Setup end" );
  Serial.println( "Test complete" );
}

void loop(){
  // NOP
}
