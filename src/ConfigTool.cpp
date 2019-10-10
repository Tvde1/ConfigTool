/*
	Name:	ConfigTool.cpp
	Author:	Tvde1
*/

#include "ConfigTool.h"
#include <FS.h>
#include <SPIFFS.h>

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
		item.second->deserialize(&root);
	}

	f.close();
}

void ConfigTool::save() {
	DynamicJsonDocument root(ConfigSize);

	for (auto item : variables_) {
		item.second->serialize(&root);
	}
	
	SPIFFS.begin(true);
	File f = SPIFFS.open("/config.json", "w");
	serializeJson(root, f);
	f.close();
}

void ConfigTool::reset() {
	SPIFFS.begin(true);
	SPIFFS.remove("/config.json");

	for (auto item : variables_) {
		item.second->reset();
	}
}

String ConfigTool::createWebPage(bool updated) {
	const String beginHtml = "<html><head>"
		"<title>Config Tool</title>"
		"</head><body>"
		"<h1>ConfigTool Web Interface</h1>"
		"<p>Edit the config variables here and click save.</p>"
		"<form method=\"POST\" action=\"config\"><table>";
	
	const String endHtml = "</table>"
		"<button type=\"submit\">Save</button>"
		"<button type=\"submit\" name=\"reset\">Reset</button>"
		"</form></body>"
		"</script></html>";

	String result = beginHtml;

	//if (updated) {
	//	result += savedAlert;
	//}

	//result += continueHtml;

	for (auto item : variables_) {
		result += "<tr>"
			"<td><label>" + item.first + "</label></td>"
			"<td><input name=\"" + item.first + "\" type=\"text\" value=\"" + item.second->toString() + "\" /></td>"
			"</tr>";
	}

	result += endHtml;

	Serial.println(result);

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
			Serial.write("Args hit: ");
			for (int i = 0; i < server->args(); i++) {
				String name = server->argName(i);
				
				if (name == "reset") Serial.print("....oops....");
				
				auto item = variables_.find(name);
				Serial.print(name + " | ");
				if (item == variables_.end()) {
					continue;
				}
				updated = true;
				item->second->fromString(server->arg(name));
			}
			Serial.println();
		}
		save();

		String html = createWebPage(updated);
		server->send(200, "text/html", html);
		return;
	};
}