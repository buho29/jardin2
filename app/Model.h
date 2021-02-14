// Model.h
//883.530 bytes
//883.458 bytes

#ifndef _MODEL_h
#define _MODEL_h

#include "Arduino.h"
#include <WiFi.h>
#include <iterator>
#include <ESPmDNS.h>
#include <FS.h>
#include "mbedtls/md.h"//encript 

#include <LITTLEFS.h>
#include <Adafruit_BME280.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "lib/Task.h"
#include "data.h"
#include "observer.h"

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define FLT_MAX 340282346638528859811704183484516925440.0

#define ON LOW
#define OFF HIGH

#define MODEL_MAX_ZONES 10
#define MODEL_MAX_CHAR_NAME_ZONE 20
#define MODEL_MAX_ALARMS 20

#define SENSOR_INTERVAL 60000
#define DELAY_MSG_DISPLAY 2000

class Model :public Observer
{

private:

	WiFiClient client;

	//Meteo service
	// http://dataservice.accuweather.com/forecasts/v1/daily/1day/1451030?apikey=q329xaaTojo0koLv6A3uFgh3dQLgp6em%20&language=es-ES&details=true&metric=true
	// http://dataservice.accuweather.com/forecasts/v1/daily/1day/1451030?apikey=SoOCQzMkcUaK83HlGPMz3rxaxlUsEr1a%20&language=es-ES&details=true&metric=true
	const char* host = "dataservice.accuweather.com";
	///url
	const char* url1 = "/forecasts/v1/daily/1day/";
	const char* url2 = "?apikey=";

	// dicionary
	StringTable<30> lang;
	enum Str {
		welcome, errorLogin, reqLogin, notFound, occupied, desired,
		open, close, pause, deleteItem, edit, create,
		errorOpen, errorClose, errorPause,
		errorDelete, errorEdit, errorCreate, errorBusy,
		errorSensor, errorWifi, conWifi, conWifi1, errorFore, updatedFore,
		updatedTime, errorTime
	};

	//timer & alarm
	ClockTime& clockTime = ClockTime::instance();
	Tasker& tasker = Tasker::instance();

	// data
	Config config;

	bool firevent = true;
	Task* sunTask = nullptr;

	//sensor
	Adafruit_BME280 bme; // I2C

	AsyncWebServer server;
	AsyncWebSocket ws;

	// el tiempo para volver a intentarlo
	const uint8_t retryTime = 60;

	DataTable<10, ZoneItem> zones;
	DataTable<20, TapItem> taps;
	DataTable<200, AlarmItem> alarms;

	static const uint logSize = 48;
	DataList<logSize, SensorsItem> sensors;

	//pause variable
	int16_t elapsedPausedTask = 0;
	int32_t pausedTime = 0;

	//default
	void loadDefaultTaps();
	void loadDefaultLang();
	void loadDefaultZones();

	//		data file
	bool writeFile(const char* path, const char* message);
	String readFile(const char* path);
	bool readJson(const char* path, Iserializable* data);
	bool readJson(const char* path, Item* item);
	void writeJson(const char* path, Iserializable* data);
	void writeJson(const char* path, Item* item);

	void readEprom();

	//internet
	void load();

	void connectWifi();
	void enableSoftAP();
	void loadForecast();
	bool parseForescast();

	// cuando guardamos el log sensor cada 1/2h
	void saveLoger(Task* t);

	//crypto
	String sha1(const String& msg);

	bool openTap(uint8_t tapId, bool val);

	void loadLocalTime();
	// en caso de error 
	void retryLater();
	// para setTimeout
	void onRetry(Task* t);
	//para actualizar cuando es de noche/dia
	void onSunChanged(Task* t);
	void updateSunTask();

	void updateSensor();
	void onWater(Task* t);
	void reloadTasks(uint8_t zoneId);

	void calcZone(int32_t zoneId);
	void calcZones();

	void updateTasks();
	void updateTasks(uint8_t zone);

	bool isLastAlarm(AlarmItem* a);

	//servidor
	void startWebServer();
	//force headers
	void sendResponse(AsyncWebServerRequest* request, AsyncWebServerResponse* response);

	//login

	std::list <AsyncWebSocketClient*> clientsAuth;
	void saveClientAuth(AsyncWebSocketClient* client);
	void removeClientAuth(AsyncWebSocketClient* client);
	void sendAllAuth(const String& str);

	bool isAuthenticate(const String& token, const IPAddress& ip);
	bool isAuthenticate(AsyncWebServerRequest* request);
	String createToken(const IPAddress& ip);
	bool isValide(const char* user, const char* pass);

	//websocket send
	void sendAll(const String& str);
	String printJson(const char* name, Item* item);
	String printJson(const char* name, Iserializable* data);

	String printJsonFirstRun();
	String printJsonOption();
	String printJsonForecast();

	void printJsonSystem(const JsonObject& obj);
	void printJsonForecast(const JsonObject& obj);

	//actions clients
	void executeJson(AsyncWebSocketClient* client, const String& json);
	void executeJsonZone(AsyncWebSocketClient* client, const JsonObject& zone);
	void executeJsonAlarm(AsyncWebSocketClient* client, const JsonObject& alarm);

	// wasap websocket
	void sendMessage(uint8_t type, const String& msg, AsyncWebSocketClient* client = nullptr);
	void sendClient(const char* tag, const char* msg, AsyncWebSocketClient* client = nullptr);

	//file
	void onUploadFile(AsyncWebServerRequest* request,
		const String& filename, size_t index, uint8_t* data, size_t len, bool final);
	void onFilePage(AsyncWebServerRequest* request);

	void listDir(const char* dirname, const JsonArray& rootjson, uint8_t levels);

	void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
		AwsEventType type, void* arg, uint8_t* data, size_t len);
	//update clients and write data
	void dispachZone(bool alarm);

public:

	Model() :server(80), ws("/ws") { };
	//singleton
	static Model * instance();

	//setup
	void begin();
	//loop
	void update();
	//serial.print(zone)
	void printZones();
	void printAlarms(uint8_t zone);

	bool connectedWifi = false;
	bool loadedTime = false;

	Status status;
	char msgError[100] = "";
	char msgStatus[100] = "";

	ZoneItem* currentZone = nullptr;
	AlarmItem* currentAlarm = nullptr;

	// estamos regando manualmente una zona ?
	bool isManualZoneWater = false;
	// estamos regando manualmente ?
	bool isManualWater = false;
	// grifos

	// esta abierto algun grifo ?
	bool isWatering();
	// esta pausado ?
	bool isPaused();

	DataList<logSize, SensorsItem> & getSensorsLog() {
		return sensors;
	}

	//data Zone
	bool editZone(int zone, const char * name, uint32_t modes);
	int addZone(const char * name, uint32_t modes);
	bool removeZone(int zone);

	//alarmas
	int addAlarm(int zone, int id, uint32_t time, uint16_t duration);
	int addAlarm(int zone, int id, uint16_t duration,
		uint8_t h, uint8_t m, uint8_t s = 0);

	bool editAlarm(int id, int tapId, uint32_t time, uint16_t duration);
	bool removeAlarm(int index);
	AlarmItem * isAlarmUsingTime(uint32_t time, uint16_t duration, int ignoreId = -1);

	//riego zona manual
	bool waterZone(uint8_t zone);
	//parar el riego de la zona actual
	bool stopWaterZone();
	bool pauseWaterZone(bool pause);

	SensorsItem currentSensor;

	WeatherData weather;
	bool loadedForescast = false;
	bool isDay();

	bool canWatering();
	int32_t lastTimeModesChanged = 0;

	uint16_t getElapsedAlarm();

};


#endif
