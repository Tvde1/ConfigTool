/*
	Name:	ConfigTool.cpp
	Author:	Tvde1
*/

#include "ConfigTool.h"
#include <FS.h>
#if defined(ARDUINO_ARCH_ESP32) //ESP32
	#include <SPIFFS.h>
#endif

void ConfigTool::load() {
#if defined(ARDUINO_ARCH_ESP32) //ESP32
	if (!SPIFFS.begin(true)) {
#else
	if (!SPIFFS.begin()) {
#endif
		Serial.println("SPIFFS Failed");
	}
	File f = SPIFFS.open("/config.json", "r");
	if (!f) {
		return;
	}

	DynamicJsonDocument root(ConfigSize);
	deserializeJson(root, f.readStringUntil('\n'));
	for (auto item : variables_) {
		item.second->deserialize(&root);
	}

	f.close();
}

void ConfigTool::save() {
	DynamicJsonDocument root(ConfigSize);

	for (auto item : variables_) {
		item.second->serialize(&root);
	}
	
	SPIFFS.begin();
	File f = SPIFFS.open("/config.json", "w");
	serializeJson(root, f);
	f.close();
}

void ConfigTool::reset() {
	SPIFFS.begin();
	SPIFFS.remove("/config.json");

	for (auto item : variables_) {
		item.second->reset();
	}
}

String ConfigTool::createInput(BaseVar *item, String type) {
	String result = "<input name=\"" + item->name + "\" type=\"" +type+ "\" value=\"";

	if (!item->hideValueInWeb) {
		result += item->toString();
	}
	result += "\" />";

	return result;
}

String ConfigTool::createBoolSelector(ConfigVar<bool> *item, String label, String display, bool checked) {

	String result = "<input name=\"" + item->name + "\" type=\"radio\" id=\"_" +label+ "_\" value=\"" +label+ "\" ";

	if (checked) {
		result += "checked ";
	}
	result += "><label for=\"_" +label+ "_\"> " +display+ "</label> ";

	return result;
}

String ConfigTool::createWebPage(bool updated) {
	static const String beginHtml = 
		"<html>"
		"<head>"
			"<title>Config Tool</title>"
		"</head>"
		"<body>"
			"<h1>ConfigTool Web Interface</h1>"
			"<p>Edit the config variables here and click save.</p>";

	static const String savedAlert = 
			"<p>The config has been saved.</p>";

	static const String continueHtml = 
			"<form method=\"POST\" action=\"config\">"
				"<table>";
	
	static const String endHtml = 
				"</table>"
				"<p>"
					"<button type=\"submit\">Save</button><br/><br/>"
					"<button type=\"submit\" name=\"reset\">Reset</button> be careful!"
				"</p>"
			"</form>"
		"</body>"
		"</html>";

	String result = beginHtml;

	if (updated) {
		result += savedAlert;
	}
	result += continueHtml;

	for (auto item : variables_) {
		
		if (item.second->hideInWeb) continue;
		
		result += 
			"<tr>"
				"<td><label>" + item.first + "</label></td>"
				"<td>";
		
		switch (item.second->varType()) {
			
			case VT_TEXT:
				result += createInput(item.second, "text");
				//<input name=\"" + item.first + "\" type=\"text\" value=\"";
				//if (!item.second->hideValueInWeb) result += item.second->toString();
				//result += "\" />";
				break;
			
			case VT_NUM:
				result += createInput(item.second, "number");
				//result += "<input name=\"" + item.first + "\" type=\"number\" value=\"";
				//if (!item.second->hideValueInWeb) result += item.second->toString();
				//result += "\" />";
				break;
			
			case VT_BOOL:
				bool val = *(((ConfigVar<bool>*)item.second)->pointer);
				result += "<fieldset>";
				result += createBoolSelector((ConfigVar<bool>*)item.second, "true",  "on",  val  && !item.second->hideValueInWeb);
				result += createBoolSelector((ConfigVar<bool>*)item.second, "false", "off", !val && !item.second->hideValueInWeb);
//				result += createBoolTag(item.first, "true", "on", val && !item.second->hideValueInWeb);
//				result += createBoolTag(item.first, "false", "off", !val && !item.second->hideValueInWeb);
				result += "</fieldset>";
				break;
		
		}
				
		result +=
				"</td>"
			"</tr>";
	}

	result += endHtml;

	//Serial.println(result);

	return result;
}

std::function<void()> ConfigTool::getWebHandler(WebServer* server) {
	return [this, server]() {
		bool updated = false;

		if (/*server->args() == 1 &&*/ server->hasArg("reset")) { //} && server->arg("reset") == "true") {
			Serial.println("Reset is true.");
			for (auto item : variables_) {
				item.second->reset();
			}
			updated = true;
		}
		else {
			for (int i = 0; i < server->args(); i++) {
				String name = server->argName(i);
				
				auto item = variables_.find(name);
				if (item == variables_.end()) {
					continue;
				}
				String val = server->arg(name);
				
				Serial.println(name + "=" + val);
				
				// skip empty input for values that are not displayed
				if (item->second->hideValueInWeb && val == "") {
					continue;
				}
					
				updated = true;
				item->second->fromString(val);
			}
		}
		save();

		String html = createWebPage(updated);
		server->send(200, "text/html", html);
		return;
	};
}