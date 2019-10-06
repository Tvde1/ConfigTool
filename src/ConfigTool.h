/*
	Name:	ConfigTool.h
	Author:	Tvde1
*/
#ifndef _ConfigTool_h
#define _ConfigTool_h

#include <map>
#include <ArduinoJson.h>


struct BaseVar {
	String name;
	virtual void serialize(JsonDocument*) = 0;
	virtual void deserialize(JsonDocument*) = 0;
	virtual void reset() = 0;
	virtual String toString() = 0;
	virtual void fromString(String) = 0;
};

template <typename T>
struct ConfigVar : BaseVar {
	ConfigVar(String n, T* p) {};

	void deserialize(JsonDocument* json) {};

	void serialize(JsonDocument* json) {};

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
};

struct ConfigTool {
public:
	template <typename T>
	void addVariable(String name, T* pointer) {
		variables_[name] = (new ConfigVar<T>(name, pointer));
	};
	void load();
	void save();
	int ConfigSize = 1024;

	void reset();
private:
	std::map<String, BaseVar*> variables_;

};

#endif
