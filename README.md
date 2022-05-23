# ConfigTool

Arduino or ESP8266 libraryy for easy saving and storing config variables.
Also has a handler to edit them via a webserver.

See [this example](https://github.com/Tvde1/ConfigTool/blob/master/examples/ConfigTool/ConfigTool.ino).

```cpp
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
```
It also contains a web interface which can be added like:
```cpp
server.on("/config", configTool.getWebHandler(&server));
```
