/*
	Name:	ConfigTool.cpp
	Author:	Tvde1
*/

#include "ConfigTool.h"
#include "FS.h"
#include "ArduinoJson.h"
#include "ESP8266WebServer.h"
#include <map>

void ConfigTool::load() {
	SPIFFS.begin();
	File f = SPIFFS.open("/config.json", "r");
	if (!f) {
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(f.readStringUntil('\n'));

	for (auto item : variables_) {
		item.second->deserialize(&root);
	}

	f.close();
}

void ConfigTool::save() {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	for (auto item : variables_) {
		item.second->serialize(&root);
	}

	SPIFFS.begin();
	File f = SPIFFS.open("/config.json", "w");

	String output = "";
	root.printTo(f);

	f.close();
}

String ConfigTool::createWebPage(bool updated) {
	const String beginHtml = "<html><head><title>Config Tool</title><link rel=\"stylesheet\"href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/css/bootstrap.min.css\"integrity=\"sha384-9gVQ4dYFwwWSjIDZnLEWnxCjeSWFphJiwGPXr1jddIhOegiu1FwO5qRGvFXOdJZ4\"crossorigin=\"anonymous\"><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\"crossorigin=\"anonymous\"></script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.0/umd/popper.min.js\"integrity=\"sha384-cs/chFZiN24E4KMATLdqdvsezGxaGsi4hLGOzlXwp5UZB1LY//20VyM2taTB4QvJ\"crossorigin=\"anonymous\"></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/js/bootstrap.min.js\"integrity=\"sha384-uefMccjFJAIv6A+rW+L4AHf99KvxDjWSu1z9VI8SKNVmz4sk7buKt/6v9KI65qnm\"crossorigin=\"anonymous\"></script></head><body><div class=\"container\"><div class=\"jumbotron\"style=\"width:100%\"><h1>ConfigTool Web Interface</h1><p>Edit the config variables here and click save.</p></div>";
	const String continueHtml = "<form method=\"POST\" action=\"\">";
	const String savedAlert = "<div class=\"alert alert-success\" role=\"alert\"><button type=\"button\" class=\"close\" data-dismiss=\"alert\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>The config has been saved.</div>";

	const String endHtml = "<div class=\"form-group row\"><div class=\"col-sm-1\"><button class=\"btn btn-primary\" type=\"submit\">Save</button></div><div class=\"col-sm-1 offset-sm-0\"><button type=\"button\" class=\"btn btn-danger\" onclick=\"reset()\">Reset</button></div></div></form></div></body><script>function reset(){var url=window.location.href;if(url.indexOf('?')>0){url=url.substring(0,url.indexOf('?'));}url+='?reset=true';window.location.replace(url);}</script></html>";

	String result = beginHtml;

	if (updated) {
		result += savedAlert;
	}

	result += continueHtml;

	for (auto item : variables_) {
		result += "<div class=\"form-group row\"><div class=\"col-2\"><label>" + item.first + "</label></div><div class=\"col-10\"><input name=\"" + item.first + "\" class=\"form-control\" type=\"text\" value=\"" + item.second->toString() + "\" /></div></div>";
	}

	result += endHtml;

	return result;
}

std::function<void()> ConfigTool::getWebHandler(ESP8266WebServer* server) {
	return [this, server]() {
		bool updated = false;

		if (server->args() == 1 && server->hasArg("reset") && server->arg("reset") == "true") {
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

void ConfigTool::reset() {
	SPIFFS.begin();
	SPIFFS.remove("/config.json");

	for (auto item : variables_) {
		item.second->reset();
	}
}
