/*
	Name:	ConfigTool.h
	Author:	Tvde1
*/
#ifndef _ConfigTool_h
#define _ConfigTool_h

#include <map>
#include <ArduinoJson.h>

#if defined(ARDUINO_ARCH_ESP8266) //ESP8266
    #include <ESP8266WebServer.h>
	#define WebServer ESP8266WebServer
#elif defined(ARDUINO_ARCH_ESP32) //ESP32
    #include <SPIFFS.h>
    #include <WebServer.h>
#else
	#error "unsupported architecture - intended for ESP32 and ESP8266"
#endif

#define VT_TEXT 1
#define VT_BOOL 2
#define VT_NUM  3

struct BaseVar {
	String name;
	bool hideInWeb = false;
	bool hideValueInWeb = false;
	
	virtual void serialize(JsonDocument*) = 0;
	virtual void deserialize(JsonDocument*) = 0;
	virtual void reset() = 0;
	virtual String toString() = 0;
	virtual void fromString(String) = 0;
	
	virtual int varType() = 0;
};

template <typename T>
struct ConfigVar : BaseVar {
	ConfigVar(String n, T* p) {};

	void deserialize(JsonDocument* json) {};
	void serialize(JsonDocument* json) {};
	void reset() {};
	String toString() { return "";  };
	void fromString(String) {};

	int varType() {return -1;};
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

	void deserialize(JsonDocument* json) {
		*pointer = String{ (*json)[name] | defaultValue };
	}

	void serialize(JsonDocument* json) {
		(*json)[name] = *pointer;
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

	int varType() {
		return VT_TEXT;
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

	void deserialize(JsonDocument* json) {
		if (!(*json)[name].isNull()) {
			*pointer = (*json)[name];
		}
	}

	void serialize(JsonDocument* json) {
		(*json)[name] =  *pointer;
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

	int varType() {
		return VT_BOOL;
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

	void deserialize(JsonDocument* json) {
		if (!(*json)[name].isNull()) {
			*pointer = (*json)[name];
		}
	}

	void serialize(JsonDocument* json) {
		(*json)[name] = *pointer;
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

	int varType() {
		return VT_NUM;
	}
};

struct ConfigTool {
public:
	int ConfigSize = 1024;

	template <typename T>
	void addVariable(String name, T* pointer, bool hide = false, bool hideVal = false) {
		ConfigVar<T>* var = new ConfigVar<T>(name, pointer);
		var->hideInWeb = hide;
		var->hideValueInWeb = hideVal;
		variables_[name] = var;
	};

	void load();
	void save();
	void reset();

	std::function<void()> getWebHandler(WebServer*);
	
private:
	std::map<String, BaseVar*> variables_;
	String createInput(BaseVar*, String);
	String createBoolSelector(ConfigVar<bool>*, String, String, bool);
	String createWebPage(bool);
};

#endif
