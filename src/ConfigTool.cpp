/*
	Name:	ConfigTool.cpp
	Author:	Tvde1
*/

#include "ConfigTool.h"
#include <FS.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <map>

void ConfigTool::load() {
	if (!SPIFFS.begin(true)) {
		Serial.println("SPIFFS Failed");
	}
	File f = SPIFFS.open("/config.json", "r");
	if (!f) {
		return;
	}

	DynamicJsonDocument root(ConfigSize);
	deserializeJson(root, f.readStringUntil('\n'));
	for (auto item : variables_) {
		item.second->deserialize(root);
	}

	f.close();
}

void ConfigTool::save() {
	DynamicJsonDocument root(ConfigSize);

	for (auto item : variables_) {
		item.second->serialize(root);
	}

	SPIFFS.begin();
	File f = SPIFFS.open("/config.json", "w");
	serializeJson(root, f);
}

void ConfigTool::reset() {
	SPIFFS.begin(true);
	SPIFFS.remove("/config.json");

	for (auto item : variables_) {
		item.second->reset();
	}
}

