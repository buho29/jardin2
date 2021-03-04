#ifndef _DATA_h
#define _DATA_h

#include "arduino.h"

#include "lib/dataTable.h"
#include "lib/Task.h"
#include "modes.h"

/********************
	diccio lang
********************/

struct StrItem :public Item
{
	char value[50] = "";

	void set(const int id, const char * str) {
		this->id = id;
		strcpy(this->value, str);
	};
	void serializeItem(JsonObject &obj, bool extra) {
		obj["id"] = this->id;
		obj["value"] = this->value;
	};
	void deserializeItem(JsonObject &obj) {
		if (!obj.containsKey("id") || !obj.containsKey("value")) 
		{
			Serial.println("faill deserializeItem StrItem");
			return;
		}
		set(
			obj["id"].as<int>(),
			obj["value"].as<char*>()
		);
	};
};

template <int N>
class StringTable :public DataTable<N, StrItem> {
public:
	String get(int key) {
		if (this->has(key)) {
			StrItem * str = this->mapItems[key];
			return String(str->value);
		}
		return String(" String not Found ");
	};
	void add(int key, const char * str) {
		StrItem * item = this->getEmpty();
		if (item != nullptr) {
			item->set(key, str);
			this->push(item);
		}
	};
};

/*    datos    */
// dato que se guardara en la eeprom
struct Config :public Item 
{

	//char wifi_ssid[32] = "AndroidAP";
	//char wifi_pass[64] = "gzfq4137";
	char wifi_ssid[32] = "Movistar_1664";
	char wifi_pass[64] = "EGYDRNA6H4Q";
	char cityID[10] = "1451030";// tintores
	char cityName[20] = "Tintores";
	//char accuURL[100] = "SoOCQzMkcUaK83HlGPMz3rxaxlUsEr1a%20&language=es-ES&details=true&metric=true";
	char accuURL[100] = "q329xaaTojo0koLv6A3uFgh3dQLgp6em%20&language=es-ES&details=true&metric=true";
	char www_user[32] = "admin";
	char www_pass[64] = "admin";

	//offset, irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	uint8_t dst[8] = {0, 5, 3, 5, 10, 2, 3 };
	int8_t tz = 1;
	//private print result;
	String r;

	void set(
		const char * wifi_ssid, const char * wifi_pass,
		const char * cityID, const char *cityName, const char *accuURL,
		const char *www_user, const char *www_pass,
		const char * dst, int tz) 
	{
		setWifi(wifi_ssid, wifi_pass);
		setAccu(cityID, cityName, accuURL);
		setAdmin(www_user, www_pass);
		setTimeZone(dst, tz);
	};
	
	void setAdmin(const char * www_user, const char * www_pass) {
		strcpy(this->www_user, www_user);
		strcpy(this->www_pass, www_pass);
	};
	void setWifi(const char * wifi_ssid, const char * wifi_pass) {
		strcpy(this->wifi_ssid, wifi_ssid);
		strcpy(this->wifi_pass, wifi_pass);
	};
	void setAccu(const char * cityID, const char * cityName, const char *accuURL) 
	{
		strcpy(this->cityID, cityID);
		strcpy(this->cityName, cityName);
		strcpy(this->accuURL, accuURL);
		Serial.printf("read accuUrl %s \n", accuURL);
	};
	void setTimeZone(const char * dst, int tz) {
		setDst(dst);
		this->tz = tz;
	};


	void setDst(const char * dst) {
		Serial.println(dst);
		return;
		char str[30];
		strcpy(str, dst);

		char * pch;
		//printf("\"%s\" into tokens:\n", str);

		int i = 0;
		pch = strtok(str, " ,");
		while (pch != NULL)
		{
			if (i < 7) {
				//printf("%s %d\n", pch, i);
				this->dst[i++] = atoi(pch);
			}
			pch = strtok(NULL, " ,");
		}

		//for (i = 0;i < 7;i++) {
		//	printf("dst[%d] = %d\n", i, this->dst[i]);
		//}
	};

	String getDst() {
		//write
		r = "";
		for (int i = 0;i < 7;i++)
		{
			int param = dst[i];

			String tt = String(param);

			if ((i == 2 || i == 4) && param < 10) {
				tt = "0" + tt;
			}

			if (i < 6) tt += ",";

			r += tt;
		}
		Serial.println(r);
		return r;
	};
	
	void serializeItem(JsonObject &obj, bool extra) {
		if (!extra) {
			obj["wifi_pass"] = this->wifi_pass;
			obj["www_pass"] = this->www_pass;
			obj["www_user"] = this->www_user;
		}
		obj["wifi_ssid"] = this->wifi_ssid;
		obj["cityID"] = this->cityID;
		obj["cityName"] = this->cityName;
		obj["accuURL"] = this->accuURL;
		obj["tz"] = this->tz;
		obj["dst"] = getDst();
	};

	void deserializeItem(JsonObject &obj) {

		if (!obj.containsKey("wifi_ssid") || !obj.containsKey("wifi_pass") || 
			!obj.containsKey("cityID") || !obj.containsKey("cityName") || 
			!obj.containsKey("accuURL") ||
			!obj.containsKey("www_user") || !obj.containsKey("www_pass") || 
			!obj.containsKey("dst") || !obj.containsKey("tz"))
		{
			Serial.println("faill deserializeItem Config");
			return;
		}

		set(
			obj["wifi_ssid"].as<char*>(), obj["wifi_pass"].as<char*>(),
			obj["cityID"].as<char*>(), obj["cityName"].as<char*>(),
			obj["accuURL"].as<char*>(),
			obj["www_user"].as<char*>(), obj["www_pass"].as<char*>(),
			obj["dst"].as<char*>(), obj["tz"].as<int>()
		);

	};
};

struct ZoneItem : public Item
{
	uint32_t time;
	int32_t alarmId=-1;
	uint32_t elapsed;
	uint16_t duration;
	uint32_t modes;
	char name[20] = "";
	bool can_watering = true;
	bool runing = false; bool paused = false;

	void set(int id, uint32_t time, uint16_t duration,
		uint32_t modes, const char * name) 
	{
		this->id = id; this->time = time; this->duration = duration; this->modes = modes;
		strcpy(this->name, name); this->runing = false; this->paused = false;
	};
	void serializeItem(JsonObject & obj, bool extra) 
	{
		obj["id"] = this->id; obj["time"] = time,
		obj["name"] = name; obj["duration"] = duration;
		obj["modes"] = modes;
		if (extra) {
			obj["paused"] = paused;
			obj["runing"] = runing;
			obj["elapsed"] = elapsed;
			obj["alarmId"] = alarmId;
			obj["can_watering"] = can_watering;
		}
	};
	void deserializeItem(JsonObject & obj) {
		if (!obj.containsKey("id") || !obj.containsKey("time") ||
			!obj.containsKey("duration") || !obj.containsKey("modes") ||
			!obj.containsKey("name"))
		{
			Serial.println("faill deserializeItem ZoneItem");
			return;
		}
		set(
			obj["id"].as<int>(), obj["time"].as<uint32_t>(),
			obj["duration"].as<int>(), obj["modes"].as<int>(),
			obj["name"].as<char*>()
		);
	};
};

struct TapItem : public Item
{
	uint8_t pin = 0;
	bool open = false;
	char name[20] = "";

	void set(int id, uint8_t pin, const char * name) 
	{
		this->id = id; this->pin = pin;
		strcpy(this->name, name); this->open = open;
	};
	void serializeItem(JsonObject & obj, bool extra) 
	{
		obj["id"] = this->id;
		obj["name"] = this->name;
		obj["pin"] = this->pin;
		if (extra) obj["open"] = this->open;
	};
	void deserializeItem(JsonObject & obj) 
	{
		if (!obj.containsKey("id") || !obj.containsKey("pin") ||
			!obj.containsKey("name") )
		{
			Serial.println("faill deserializeItem TapItem");
			return;
		}
		set(
			obj["id"].as<int>(),obj["pin"].as<int>(),
			obj["name"].as<char*>()
		);
	};
};

struct AlarmItem :public Item
{
	uint8_t tapId;
	int8_t zoneId = -1;
	uint32_t time;
	int16_t duration;
	Task * task;

	void set(int id, uint32_t time, uint16_t duration,
		uint8_t tapId, int8_t zoneId) 
	{
		this->id = id; this->time = time; this->duration = duration;
		this->tapId = tapId; this->zoneId = zoneId;
	};

	void serializeItem(JsonObject & obj, bool extra) 
	{
		obj["id"] = this->id;
		obj["duration"] = this->duration;
		obj["time"] = this->time;
		obj["tapId"] = this->tapId;
		obj["zoneId"] = this->zoneId;
	};

	void deserializeItem(JsonObject & obj) {
		if (!obj.containsKey("id") || !obj.containsKey("time") ||
			!obj.containsKey("duration") || !obj.containsKey("tapId") ||
			!obj.containsKey("zoneId") )
		{
			Serial.println("faill deserializeItem ZoneItem");
			return;
		}
		set(
			obj["id"].as<int>(), obj["time"].as<int>(),
			obj["duration"].as<int>(), obj["tapId"].as<int>(),
			obj["zoneId"].as<int>()
		);
	};
};

struct SensorsItem :public Item
{
	float temperature;
	float pressure;
	uint8_t humidity;
	uint32_t time;

	void set(float temperature, float pressure, uint8_t humidity, uint32_t time) 
	{
		this->temperature = temperature; this->pressure = pressure;
		this->humidity = humidity; this->time = time;
	};
	void serializeItem(JsonObject & obj, bool extra = false) 
	{
		obj["te"] = this->temperature; obj["pr"] = this->pressure;
		obj["hu"] = this->humidity; obj["ti"] = this->time;
	};
	void deserializeItem(JsonObject & obj)
	{
		if (!obj.containsKey("te") || !obj.containsKey("pr") ||
			!obj.containsKey("hu") || !obj.containsKey("ti") )
		{
			Serial.println("faill deserializeItem ZoneItem");
			return;
		}
		set(
			obj["te"].as<float>(), obj["pr"].as<float>(),
			obj["hu"].as<int>(), obj["ti"].as<int>()
		);
	};
};

/* meteo */
struct WeatherData {
	struct Forecast {
		int8_t icon;
		char phrase[100];
		float win[2];// "Speed","Direction"
		uint8_t hoursOfPrecipitation;
		uint8_t precipitationProbability;
		uint8_t cloudCover;
		float totalLiquid;
	} forecast[2];//day/nigth

	time_t time;
	int8_t temp[2]; // min max
	time_t sun[2]; // inicio final

	bool isDay() {
		uint32_t t = ClockTime::instance().local();
		return t > sun[0] && t < sun[1];
	};

	Forecast * getCurrent() {
		// dia = 0 noche = 1
		uint8_t index = !isDay();
		return &forecast[index];
	};

	int8_t minTemp() { return temp[0]; };
	int8_t maxTemp() { return temp[1]; };

	uint8_t hoursOfPrecipitation(){
		return forecast[0].hoursOfPrecipitation + forecast[1].hoursOfPrecipitation;
	};

	uint8_t precipitationProbability() {
		return (uint8_t)((forecast[0].precipitationProbability + forecast[1].precipitationProbability) / 2);
	};

	uint8_t cloudCover() {
		return (uint8_t)((forecast[0].cloudCover + forecast[1].cloudCover) / 2);
	};
	float totalLiquid() {
		return forecast[0].totalLiquid + forecast[1].totalLiquid;
	};
	float speedWin() {
		// dia = 0 noche = 1
		uint8_t index = !isDay();
		return forecast[index].win[0];
	};
};
#endif