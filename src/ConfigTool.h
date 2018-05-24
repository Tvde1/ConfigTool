/*
	Name:	ConfigTool.h
	Author:	Tvde1
*/

#include <Arduino.h>;
#include <map>;
#include "ArduinoJson.h"
#include "ESP8266WebServer.h";
#include <string>;

#ifndef _ConfigTool_h
#define _ConfigTool_h

struct BaseVar {
	String name;
	virtual void serialize(JsonObject*) = 0;
	virtual void deserialize(JsonObject*) = 0;
	virtual void reset() = 0;
	virtual String toString() = 0;
	virtual void fromString(String) = 0;
};

template <typename T>
struct ConfigVar : BaseVar {
	ConfigVar(String n, T* p) {};

	void deserialize(JsonObject *json) {};

	void serialize(JsonObject* json) {};

	void reset() {};

	String toString() { return "";  };

	void fromString(String) {};
};

template <>
struct ConfigVar<String> : BaseVar {
	String* pointer;
	String defaultValue;
	ConfigVar(String n, String* p) {
		name = n;
		pointer = p;
		defaultValue = *p;
	};

	void deserialize(JsonObject *json) {
		if (json->containsKey(name) && json->is<char*>(name)) {

			*pointer = String{ json->get<char *>(name) };
			json->remove(name);
		}
	}

	void serialize(JsonObject* json) {
		json->set(name, *pointer);
	}

	void reset() {
		*pointer = defaultValue;
	}

	String toString() {
		return *pointer;
	}

	void fromString(String value) {
		*pointer = value;
	}
};

template <>
struct ConfigVar<bool> : BaseVar {
	bool* pointer;
	bool defaultValue;
	ConfigVar(String n, bool* p) {
		name = n;
		pointer = p;
		defaultValue = *p;
	};

	void deserialize(JsonObject *json) {
		if (json->containsKey(name) && json->is<bool>(name)) {

			*pointer = json->get<bool>(name);
			json->remove(name);
		}
	}

	void serialize(JsonObject* json) {
		json->set(name, *pointer);
	}

	void reset() {
		*pointer = defaultValue;
	}

	String toString() {
		return *pointer ? "true" : "false";
	}

	void fromString(String value) {
		*pointer = value == "true";
	}
};

template <>
struct ConfigVar<int> : BaseVar {
	int* pointer;
	int defaultValue;
	ConfigVar(String n, int* p) {
		name = n;
		pointer = p;
		defaultValue = *p;
	};

	void deserialize(JsonObject *json) {
		if (json->containsKey(name) && json->is<int>(name)) {
			*pointer = json->get<int>(name);
			json->remove(name);
		}
	}

	void serialize(JsonObject* json) {
		json->set(name, *pointer);
	}

	void reset() {
		*pointer = defaultValue;
	}

	String toString() {
		return String(*pointer);
	}

	void fromString(String value) {
		*pointer = value.toInt();
	}
};

struct ConfigTool {
public:
	template <typename T>
	void addVariable(String name, T* pointer) {
		variables_[name] = (new ConfigVar<T>(name, pointer));
	};
	void load();
	void save();
	std::function<void()> getWebHandler(ESP8266WebServer*);
	void reset();
private:
	std::map<String, BaseVar*> variables_;
	String createWebPage(bool);
};

#endif
