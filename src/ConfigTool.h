/*
	Name:	ConfigTool.h
	Author:	Tvde1
*/

#include <Arduino.h>
#include <list>
#include "ArduinoJson.h"

#ifndef _ConfigTool_h
#define _ConfigTool_h

struct BaseVar {
	String name;
	virtual void serialize(JsonObject*) = 0;
	virtual void deserialize(JsonObject*) = 0;
	virtual void reset() = 0;
};

template <typename T>
struct ConfigVar : BaseVar {
	T* pointer;
	T defaultValue;
	ConfigVar(String n, T* p) {
		name = n;
		pointer = p;
		defaultValue = *p;
	};

	void deserialize(JsonObject *json) {
		if (json->containsKey(name) && json->is<T>(name)) {
			*pointer = json->get<T>(name);
			json->remove(name);
		}
	}

	void serialize(JsonObject* json) {
		json->set(name, *pointer);
	}

	void reset() {
		*pointer = defaultValue;
	}
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
};

struct ConfigTool {
public:
	ConfigTool();
	template <typename T>
	void addVariable(String name, T* pointer) {
		variables_.push_back(new ConfigVar<T>(name, pointer));
	};
	void load();
	void save();
	void getWebPortal();
	void reset();
private:
	std::list<BaseVar*> variables_;
};

#endif
