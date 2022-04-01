#include "ConfigTool.h"

struct Config {
	String text = "This is my default text.";
	int number = 100;
};

Config config;
ConfigTool configTool;

void setup() {
	Serial.begin(115200);
	Serial.println();

	configTool.addVariable("Test", &config.text);
	configTool.addVariable("Some Number", &config.number);
	
	configTool.load();

	Serial.println(config.text); // This is my default text.
	Serial.println(config.number); // 100

	config.text = "I can just assign new values like this.";
	config.number = 200;

	Serial.println(config.text); // I can just assign new values like this.
	Serial.println(config.number); // 200

	configTool.save();
	//Values are saved now and will be put back on reset with .load

	configTool.reset();
	//Reset the tool and assign the default values.

	Serial.println(config.text); // This is my default text.
	Serial.println(config.number); // 100

	configTool.save();
}

void loop() {

}
