// Model.h
//883.530 bytes
//883.458 bytes


#ifndef _MODEL_h
#define _MODEL_h

#include <WiFi.h>
#include <iterator>
#include <FS.h>
#include "mbedtls/md.h"//encript 
#include <DNSServer.h>
#include <ArduinoOTA.h>

#define USELITTLEFS
#ifdef USELITTLEFS
	#include <LITTLEFS.h>
#else
	#include <SPIFFS.h>
#endif // USELITTLEFS

#include <Adafruit_BME280.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "modes.h"
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

	Model() :
		server(80), ws("/ws"), 
		apIP(8, 8, 4, 4) { };// The default android DNS

	WiFiClient client;
#ifdef USELITTLEFS
	fs::FS& fs = LITTLEFS;
#else
	fs::FS& fs = SPIFFS;
#endif // USELITTLEFS
	

	//Meteo service
	// http://dataservice.accuweather.com/forecasts/v1/daily/1day/1451030?apikey=q329xaaTojo0koLv6A3uFgh3dQLgp6em%20&language=es-ES&details=true&metric=true
	// http://dataservice.accuweather.com/forecasts/v1/daily/1day/1451030?apikey=SoOCQzMkcUaK83HlGPMz3rxaxlUsEr1a%20&language=es-ES&details=true&metric=true
	const char* host = "dataservice.accuweather.com";
	///url
	const char* url1 = "/forecasts/v1/daily/5day/";
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
	void loadDefaultLang();

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

	IPAddress apIP;
	DNSServer dnsServer;

	//debug
	bool writeFileEnabled = true;

	// el tiempo para volver a intentarlo
	const uint8_t retryTime = 60;
	//app
	DataTable<10, ZoneItem> zones;
	DataTable<10, ModesItem> modesItems;
	Modes modes;

	DataTable<20, TapItem> taps;
	DataTable<200, AlarmItem> alarms;
	//log
	//30min x 24h
	DataList<48, SensorItem> sensors;
	// 24h x year
	DataList<365, SensorAvgItem> sensors24;

	//history action (log)
	DataList<100, ActionItem> history;
	Task* taskSaveHistory = nullptr;
	void saveHistory(bool delayed = true);
	void addHistory(
		uint32_t time, Action action, 
		uint8_t value = 1, int32_t idItem = Item::CREATE_NEW
	);
	void saveDelayedHistory(Task* t);
	
	//pause variable
	int16_t elapsedPausedTask = 0;
	int32_t pausedTime = 0;

	String jsonFilesCached;
	bool lock;

	//default
	void loadDefaultTaps();
	void loadDefaultZones();

	//		data file
	bool writeFile(const char* path, const char* message);
	String readFile(const char* path);
	bool readJson(const char* path, Iserializable* data);
	bool readJson(const char* path, Item* item);
	void writeJson(const char* path, Iserializable* data);
	void writeJson(const char* path, Item* item);

	void readFiles();

	void updateTimeZone();

	//internet
	void load();

	void connectWifi();
	void enableSoftAP();

	//weather
	void loadForecast();
	bool parseForescast();

	// cuando guardamos el log sensor cada 1/2h
	void saveLoger(Task* t);
	// cuando guardamos el log sensor cada 24h
	void saveLoger24(Task* t);

	//crypto
	String sha1(const String& msg);

	bool openTap(uint8_t tapId, bool val, bool save = false);

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
	void calcModes();

	void updateTasks();
	void updateTasks(uint8_t zone);

	bool isLastAlarm(AlarmItem* a);
	bool isFirstAlarm(AlarmItem* a);

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
	String printJsonFiles();

	String printJsonForecast();

	void printJsonSystem(const JsonObject& obj);
	void printJsonForecast(const JsonObject& obj);
	void printJsonDayForecast(const JsonObject& obj,bool current,
		WeatherData& w);

	//actions clients
	void receivedJson(AsyncWebSocketClient* client, const String& json);
	void receivedJsonZone(AsyncWebSocketClient* client, const JsonObject& zone);
	void receivedJsonAlarm(AsyncWebSocketClient* client, const JsonObject& alarm);

	// wasap websocket
	void sendMessage(uint8_t type, const String& msg, AsyncWebSocketClient* client = nullptr);
	void sendMessage(uint8_t type,Str str, const String& msg, AsyncWebSocketClient* client = nullptr);
	void sendClient(const char* tag, const char* msg, AsyncWebSocketClient* client = nullptr);
	void send(const String& str,AsyncWebSocketClient* client);

	//file
	void onUploadFile(AsyncWebServerRequest* request,
		const String& filename, size_t index, uint8_t* data, size_t len, bool final);
	void onFilePage(AsyncWebServerRequest* request);

	void onRegisterIOT(AsyncWebServerRequest* request);

	void listDir(const char* dirname, const JsonArray& rootjson, uint8_t levels);

	void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
		AwsEventType type, void* arg, uint8_t* data, size_t len);
	//update clients and write data
	void dispachZone(bool alarm,bool mode=false);

	void beginOTA();

public:

	//singleton
	static Model * instance();

	//setup
	void begin();
	//loop
	void update();
	//serial.print(zone)
	void printZones();
	void printAlarms(uint zone);

	bool isConnectedWifi();
	bool loadedTime = false;

	Status status;
	char msgError[100] = "";
	char msgStatus[100] = "";

	ZoneItem* currentZone = nullptr;
	ModesItem* getModesItem(uint id);

	// estamos regando manualmente una zona ?
	bool isManualZoneWater = false;
	// estamos regando manualmente ?
	bool isManualWater = false;
	// grifos
	bool isAlarmRunning = false;
	// esta abierto algun grifo ?
	bool isWatering();
	// esta pausado ?
	bool isPaused();

	DataList<48, SensorItem> & getSensorsLog() {
		return sensors;
	}

	//data Zone
	bool editZone(uint zone, const char * name, uint32_t fmodes, const char* dates);
	ZoneItem* addZone(const char * name, uint32_t fmodes, const char* dates);
	bool removeZone(uint zone);

	//alarmas
	AlarmItem* addAlarm(uint32_t zone, uint32_t tapid, uint32_t time, uint16_t duration);
	AlarmItem* addAlarm(uint32_t zone, uint32_t tapid, uint16_t duration,
		uint8_t h, uint8_t m, uint8_t s = 0);

	bool editAlarm(uint32_t id, uint32_t tapId, uint32_t time, uint16_t duration);
	bool removeAlarm(uint32_t  index);
	AlarmItem * findAlarm(uint32_t time, uint16_t duration, uint32_t ignoreId = -1);

	//riego zona manual
	bool waterZone(uint zone);
	//parar el riego de la zona actual
	bool stopWaterZone(bool save = true);
	bool pauseWaterZone(bool pause);

	SensorItem currentSensor;

	WeatherData weather[5];
	bool loadedForescast = false;

	bool canWatering(uint32_t zoneId);

	uint16_t getElapsedAlarm();

};


#endif

