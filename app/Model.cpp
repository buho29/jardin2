#include "Model.h"

//singleton
Model * Model::instance()
{
	static  Model pInstance;
	return &pInstance;
}

// first run
void Model::begin()
{
	using namespace std::placeholders;

	status = Status::starting;
	dispachEvent(EventType::status);

#ifdef USELITTLEFS
	if (!LITTLEFS.begin(true)) {
		Serial.println("LITTLEFS Mount Failed");
		return;
	}
#else
	if (!SPIFFS.begin(true)) {
		Serial.println("SPIFFS Mount Failed");
		return;
	}
#endif // USELITTLEFS

	clockTime.begin();

	readFiles();

	updateTimeZone();

	modes.begin();

	//sensor 
	if (bme.begin(0x76)) {
		bme.setSampling(Adafruit_BME280::MODE_FORCED,
			Adafruit_BME280::SAMPLING_X1, // temperature
			Adafruit_BME280::SAMPLING_X1, // pressure
			Adafruit_BME280::SAMPLING_X1, // humidity
			Adafruit_BME280::FILTER_OFF);

		updateSensor();

		// cada 1/2 hora guardamos los valores de los sensores
		tasker.setInterval(std::bind(&Model::saveLoger, this, _1), (30*60));

		Task * t = tasker.setInterval(
			std::bind(&Model::saveLoger24, this, _1), 0,0);

	}
	else {
		Serial.println("bme280 Error");
		strcpy(msgError, lang.get(Str::errorSensor).c_str() );
		dispachEvent(EventType::error);
	}

	//activamos softAP
	enableSoftAP();
	////intentamos conectar al wifi y descargar la hora y la meteo
	connectWifi();

	calcModes();

	// iniciamos puertos de salida
	for (auto &it : taps) {
		TapItem * tap = it.second;

		pinMode(tap->pin, OUTPUT);
		digitalWrite(tap->pin, OFF);

		//Serial.printf("pin %d\n", tap->pin);
	}

	//iniciamos las tareas
	updateTasks();


	jsonFiles = printJsonFiles();

	startWebServer();

	status = Status::started;
	dispachEvent(EventType::status);
}

//		loop
void Model::update()
{
	//static uint32_t lastFreeHeap = 0;

	uint32_t c = millis();
	tasker.check();

	static uint32_t delayfree = 0;
	if (millis() - delayfree > 1000) {
		delayfree = millis();
		ws.cleanupClients();
	}

	dnsServer.processNextRequest();

	static uint32_t delaySensors = 0;
	if (millis() - delaySensors > 60000) {//SENSOR_INTERVAL 1min
		delaySensors = millis();
		updateSensor();
	}

	static uint32_t delayZone = 0;
	//se esta regando una zona
	if (currentZone != nullptr && !isPaused() && millis() - delayZone > 1000) {
		delayZone = millis();
		currentZone->elapsed = getElapsedAlarm();
		//actualizamos todos los clientes;
		sendAll(printJson("zone", currentZone));
	}

	if (millis() - c > 50)
		Serial.printf("model.update %d ms\n", millis() - c);
}

//		lengua
void Model::loadDefaultLang()
{
	lang.add(Str::welcome, "Bienvenido ");
	lang.add(Str::errorLogin, "usuario o pass no valido ");
	lang.add(Str::reqLogin, "Requierre logearse");
	lang.add(Str::notFound, "no encontrada");
	lang.add(Str::occupied, "ocupada por ");
	lang.add(Str::desired, " deseada: ");

	lang.add(Str::open, "abierto");
	lang.add(Str::close, "cerrado");
	lang.add(Str::pause, " pausado");
	lang.add(Str::deleteItem, "Borrada correctamente ");
	lang.add(Str::edit, "Editado correctamente ");
	lang.add(Str::create, "Creado correctamente ");

	lang.add(Str::errorOpen, "Error abriendo ");
	lang.add(Str::errorClose, "Error cerrando ");
	lang.add(Str::errorPause, "Error pausando ");
	lang.add(Str::errorDelete, "Error borrando ");
	lang.add(Str::errorEdit, "Error editando ");
	lang.add(Str::errorCreate, "Error creando ");
	lang.add(Str::errorBusy, "Estoy regando");
	//screen
	lang.add(Str::errorSensor, "Error con sensor bme280");
	lang.add(Str::errorWifi, "Error Wifi");
	lang.add(Str::conWifi, "Conectando a ");
	lang.add(Str::conWifi1, "Conectado a ");
	lang.add(Str::errorFore, "Error descargando Meteo");
	lang.add(Str::updatedFore, "Meteo actualizada");
	lang.add(Str::updatedTime, "Hora actualizada");
	lang.add(Str::errorTime, "Error actualizando la hora");

	//lang.add(Str::, "");

	writeJson("/data/lang.json", &lang);
}

//		crypto
String Model::sha1(const String & msg)
{
	const char *payload = msg.c_str();
	const int size = 20;

	byte shaResult[size];

	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;

	const size_t payloadLength = strlen(payload);

	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	mbedtls_md_update(&ctx, (const unsigned char *)payload, payloadLength);
	mbedtls_md_finish(&ctx, shaResult);
	mbedtls_md_free(&ctx);

	String hashStr = "";

	for (uint16_t i = 0; i < size; i++) {
		String hex = String(shaResult[i], HEX);
		if (hex.length() < 2) {
			hex = "0" + hex;
		}
		hashStr += hex;
	}

	return hashStr;
}

//		data file
bool Model::writeFile(const char * path, const char * message)
{
	Serial.printf("path : %s\n",path);
	int tim = millis();
	File file = fs.open(path, FILE_WRITE);
	if (!file) {
		Serial.println("- failed to open file for writing");
		return false;
	}
	if (file.print(message)) {
		int elapsed = (millis() - tim);
		file.close();
		return true;

		//// comprobamos que se guardo correctamente comparando string
		//const String &str = readFile(path);
		//if (!strcmp(message, str.c_str())){
		//	Serial.printf("- file written in %dms :%s len:%d\n",elapsed, path, strlen(message));
		//	return true;
		//}
	}
	Serial.println("- frite failed");
	return false;
}

String Model::readFile(const char * path)
{
	Serial.printf("reading file: %s\r\n", path);

	File file = fs.open(path);

	String str = "";

	if (!file || file.isDirectory()) {
		Serial.printf("readJson- failed to open %s for reading\n", path);
		return str;
	}

	while (file.available())
	{
		str += (char)file.read();
	}
	return str;
}

bool Model::readJson(const char * path, Iserializable * data)
{
	const String & str = readFile(path);
	if (str.length() > 0) {
		data->deserializeData(str);
		return true;
	}
	return false;
}

bool Model::readJson(const char * path, Item * item)
{
	DynamicJsonDocument doc(2000);

	const String & str = readFile(path);

	if (str.length() > 0) {

		// Parse
		deserializeJson(doc, str);

		JsonObject obj = doc.as<JsonObject>();
		item->deserializeItem(obj);
		return true;
	}
	return false;
}

void Model::writeJson(const char * path, Iserializable * data)
{
	 const String & json = data->serializeString();
	writeFile(path, json.c_str());
}

void Model::writeJson(const char * path, Item * item)
{
	DynamicJsonDocument doc(20000);
	JsonObject obj = doc.to<JsonObject>();
	String str;

	item->serializeItem(obj,false);
	serializeJsonPretty(obj, str);
	writeFile(path, str.c_str());
}

//		open all json
void Model::readFiles()
{
	if (!readJson("/data/zones.json", &zones) || 
		!readJson("/data/alarms.json", &alarms)) 
			loadDefaultZones();
	
	if(!readJson("/data/taps.json", &taps))
		loadDefaultTaps();

	if (!readJson("/data/lang.json", &lang))
		loadDefaultLang();

	if (!readJson("/data/config.json", &config))
		writeJson("/data/config.json", &config);

	readJson("/data/logSensors.json", &sensors);
	readJson("/data/logSensors24.json", &sensors24);
	readJson("/data/history.json", &history);
	calcZones();
}

//		data

int Model::addZone(const char * name, uint32_t modes)
{
	ZoneItem * zone = zones.getEmpty();
	Serial.println("addZone");
	if (zone != nullptr) {
		zone->set(-1, 0, 0, modes, name);
		zone = zones.push(zone);
		zone->can_watering = canWatering(modes);

		dispachZone(false);
		return zone->id;
	}
	return -1;
}

bool Model::editZone(int id, const char * name, uint32_t modes)
{
	if (zones.has(id)) {
		ZoneItem * zone = zones[id];

		Serial.println("editZone");
		strncpy(zone->name, name, MODEL_MAX_CHAR_NAME_ZONE);
		zone->modes = modes;
		zone->can_watering = canWatering(modes);

		dispachZone(false);

		return true;
	}
	return false;
}

bool Model::removeZone(int id)
{
	if (zones.has(id)) {

		Serial.println("removeZone");
		for (auto &it : alarms) {
			AlarmItem * alarm = it.second;
			if (id == alarm->zoneId) {
				if (alarm->task) {
					tasker.remove(alarm->task);
					alarm->task = nullptr;
				}
				alarms.remove(alarm);
			}
		}
		bool result = zones.remove(id);
		
		if (result) dispachZone(true);
		return result;
	}
	return false;
}

void Model::dispachZone(bool alarm)
{
	if (firevent) 
	{
		sendAll(printJson("zones", &zones));
		if(alarm) sendAll(printJson("alarms", &alarms));

		writeJson("/data/zones.json", &zones);
		if (alarm) writeJson("/data/alarms.json", &alarms);

		dispachEvent(EventType::zoneChanged);
	}
}

//		alarm
int Model::addAlarm(int zoneId, int tapId, uint32_t time, uint16_t duration)
{
	if (zones.has(zoneId)) 
	{
		time = time % TASK_TICKS_24H;

		AlarmItem* alarm = alarms.getEmpty();
		// usamos el tiempo como id y como key
		alarm->set(time, time, duration, tapId, zoneId);

		AlarmItem* alarm1 = alarms.push(alarm);

		Serial.printf("addAlarm %d\n",alarm1->id);
		calcZone(zoneId);
		updateTasks(zoneId);

		dispachZone(true);

		return alarm->id;
	}
	return -1;
}

int Model::addAlarm(int zoneId, int tapId, uint16_t duration, uint8_t h, uint8_t m, uint8_t s)
{
	int32_t alarmTime = Tasker::getTickTime(h, m, s);
	return addAlarm(zoneId, tapId, alarmTime, duration);
}

bool Model::editAlarm(int id, int tapId, uint32_t time, uint16_t duration)
{
	if (!taps.has(tapId) || time < 0 || duration < 0 || duration > 21600) {
		Serial.printf("!taps.has(tapId) |%d| time |%d| duration |%d|",
			!taps.has(tapId) ,time , duration
		);
		return false;
	}

	time = time % TASK_TICKS_24H;

	if (alarms.has(id)) {
		Serial.println("editAlarm");
		AlarmItem * a = alarms[id];
		//cuando cambiamos el alarm.time debemos borrar el actual y crear otro
		//el alarm.time se usa como Id
		if (a->time != time) {
			tasker.remove(a->task);
			a->task = nullptr;
			if (alarms.remove(a)) {
				return addAlarm(a->zoneId, tapId, time, duration) > -1;
			}
		}
		else {
			a->tapId = tapId;
			a->duration = duration;
		}

		calcZone(a->zoneId);
		updateTasks(a->zoneId);


		dispachZone(true);
		return true;
	}
	return false;
}

bool Model::removeAlarm(int index)
{
	if (alarms.has(index)) {

		Serial.println("removeAlarm");
		AlarmItem * a = alarms[index];
		int zoneId = a->zoneId;
		tasker.remove(a->task);
		a->task = nullptr;

		if (alarms.remove(index)) {
			calcZone(zoneId);

			dispachZone(true);
			//Serial.printf("removed %d zoneId: %d \n", index, zoneId);
			//printAlarms(zoneId);
			return true;
		}
	}
	Serial.printf("removing alarm failled\n");
	return false;
}

//		Serial data print 
void Model::printZones()
{
	for (auto &it : zones) 
	{		
		ZoneItem * zone = it.second;


		Serial.printf("zoneId: %d name %s time : %s duration : %d\n",
			zone->id, zone->name, Tasker::formatTime(zone->time), zone->duration);

		printAlarms(zone->id);
	}
}

void Model::printAlarms(uint8_t z)
{
	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;
		if (z == alarm->zoneId) {
			Serial.printf("\t alarm Id: %d time: %s duration: %d tap: %s\n",
				alarm->id, Tasker::formatTime(alarm->time), alarm->duration, taps[alarm->tapId]->name);
		}
	}
}

//		webclient
void Model::sendAll(const String & str)
{
	ws.textAll(str.c_str());
	//Serial.println(str);
}

void Model::sendAllAuth(const String & str)
{
	for(AsyncWebSocketClient * client : clientsAuth)
		client->text(str);
}

String Model::printJson(const char * name, Item * item)
{
	DynamicJsonDocument doc(2000);
	JsonObject root = doc.to<JsonObject>();

	JsonObject o = root.createNestedObject(name);
	item->serializeItem(o, true);

	String json;
	serializeJson(root, json);

	return json;
}

String Model::printJson(const char * name, Iserializable * data)
{
	DynamicJsonDocument doc(20000);
	JsonObject root = doc.to<JsonObject>();
	String str;

	JsonArray a = root.createNestedArray(name);
	data->serializeData(a, true);

	serializeJson(root, str);//Pretty
	return str;
}

String Model::printJsonFirstRun()
{

	uint32_t c = millis();

	DynamicJsonDocument doc(60000);
	JsonObject root = doc.to<JsonObject>();
	String str;

	JsonObject o = root.createNestedObject("sensor");
	currentSensor.serializeItem(o, true);

	JsonArray a = root.createNestedArray("taps");
	taps.serializeData(a, true);

	a = root.createNestedArray("zones");
	zones.serializeData(a, true);

	a = root.createNestedArray("alarms");
	alarms.serializeData(a, true);

	a = root.createNestedArray("sensors");
	sensors.serializeData(a, true);

	a = root.createNestedArray("sensors24");
	sensors24.serializeData(a, true);

	a = root.createNestedArray("history");
	history.serializeData(a, true);

	if (loadedForescast) {
		o = root.createNestedObject("weather");
		printJsonForecast(o);
	}

	serializeJson(root, str);

	Serial.printf("printJsonFirstRun total %d ms\n", millis() - c);
	return str;
}

String Model::printJsonOption()
{
	DynamicJsonDocument doc(10000);
	//StaticJsonDocument<2000> doc;
	uint32_t c = millis();

	const JsonObject & root = doc.to<JsonObject>();
	
	const JsonObject & system = root.createNestedObject("system");
	printJsonSystem(system);

	JsonObject o = root.createNestedObject("config");
	config.serializeItem(o, true);

	String json;
	serializeJson(root, json);

	Serial.printf("printJsonOption total %d ms\n", millis() - c);

	return json;
}

String Model::printJsonFiles()
{
	DynamicJsonDocument doc(10000);
	uint32_t c = millis();

	const JsonObject& root = doc.to<JsonObject>();

	const JsonArray& files = root.createNestedArray("root");

	const JsonObject& www = files.createNestedObject();
	www["path"] = "/www/";
	const JsonArray& wwwFolders = www.createNestedArray("folders");
	listDir("/www/", wwwFolders, 3);

	const JsonObject& data = files.createNestedObject();
	data["path"] = "/data/";
	const JsonArray& dataFolders = data.createNestedArray("folders");
	listDir("/data/", dataFolders, 3);

	Serial.printf("config %d ms\n", millis() - c);

	String json;
	serializeJson(root, json);

	Serial.printf("printJsonFiles total %d ms\n", millis() - c);

	return json;
}

//888.494 bytes
void Model::printJsonSystem(const JsonObject & doc)
{

	uint32_t c = millis();

	doc["ip"] = WiFi.localIP().toString();
	doc["softApIp"] = WiFi.softAPIP().toString();
	doc["gateway"] = WiFi.gatewayIP().toString();
	doc["signalStrengh"] = -WiFi.RSSI();
	doc["freeHeap"] = ESP.getFreeHeap();
	doc["heapSize"] = ESP.getHeapSize();

	Serial.printf("printJsonSystem %d ms\n", millis() - c);


#ifdef USELITTLEFS
	doc["totalSpace"] = LITTLEFS.totalBytes();
	doc["usedSpace"] = LITTLEFS.usedBytes();
#else
// TODO: print spiff
#endif

	Serial.printf("printJsonSystem %d ms\n", millis() - c);
}

void Model::printJsonForecast(const JsonObject & doc)
{
	// dia = 0 noche = 1
	uint8_t index = !weather.isDay();

	WeatherData::Forecast * fore = weather.getCurrent();

	doc["phrase"] = fore->phrase;
	doc["winSpeed"] = fore->win[0];
	doc["winDirection"] = fore->win[1];
	doc["hoursOfPrecipitation"] = fore->hoursOfPrecipitation;
	doc["precipitationProbability"] = fore->precipitationProbability;
	doc["totalLiquid"] = fore->totalLiquid;
	doc["cloudCover"] = fore->cloudCover;
	doc["icon1"] = fore->icon;
	
	//clockTime.timeNow();
	doc["time"] = clockTime.utc();
	doc["minTemp"] = weather.minTemp();
	doc["maxTemp"] = weather.maxTemp();
	doc["sunStart"] = weather.sun[0]-clockTime.tz();
	doc["sunEnd"] = weather.sun[1]-clockTime.tz();
	doc["isDay"] = weather.isDay();
	doc["cityName"] = config.cityName;
}

String Model::printJsonForecast()
{
	DynamicJsonDocument doc(10000);
	//StaticJsonDocument<2000> doc;
	const JsonObject & root = doc.to<JsonObject>();
	
	JsonObject o = root.createNestedObject("weather");
	printJsonForecast(o);

	String json;
	serializeJson(root, json);
	return json;
}

void Model::listDir(const char * dirname,const JsonArray & rootjson, uint8_t levels)
{
	Serial.printf("Listing %s \n", dirname);

	uint32_t c = millis();

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}

	String path = String(dirname);

	JsonObject obj = rootjson.createNestedObject();
	obj["path"] = path;
	JsonArray dir = obj.createNestedArray("files");

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			if (levels) {
				//path += "/";
				listDir(file.name(),rootjson, levels - 1);
			}
		}
		else {
			JsonObject obj = dir.createNestedObject();
			obj["name"] = String(file.name());
			obj["size"] = file.size();
		}
		file = root.openNextFile();
	}

	Serial.printf("%dms\n", millis() - c);

}

void Model::sendMessage(uint8_t type, const String & msg, AsyncWebSocketClient * client)
{
	DynamicJsonDocument doc(2000);
	JsonObject root = doc.to<JsonObject>();

	JsonObject o = root.createNestedObject("message");
	o["type"] = type;
	o["content"] = msg;

	Serial.println(msg);
	String json;
	serializeJson(root, json);

	if(client != nullptr) client->text(json);
	else ws.textAll(json);

	Serial.println(json);
}

void Model::sendMessage(uint8_t type, Str str, const String& msg, AsyncWebSocketClient* client)
{
	sendMessage(type, String(str) + "|" + msg,client);
}

void Model::sendClient(const char* tag,const char* msg, AsyncWebSocketClient * client)
{
	String json = String("{\"")+tag+"\":\"" + msg + "\"}";
	if(client != nullptr) client->text(json);
	else ws.textAll(json);
}

void Model::receivedJson(AsyncWebSocketClient * client, const String & json)
{
	Serial.println(json);
	DynamicJsonDocument doc(20000);
	// Parse
	deserializeJson(doc, json);
	JsonObject cmd = doc.as<JsonObject>();

	if (cmd.containsKey("login")) {
		JsonObject user = cmd["login"];
		if (user.containsKey("name") && user.containsKey("pass")) {
			const char * name = user["name"];
			const char * pass = user["pass"];

			Serial.printf("loggin user %s pass%S", name, pass);

			if (isValide(name, pass)) 
			{
				String token = createToken(client->remoteIP());
				sendClient("token", token.c_str(),client);
				//go home
				sendClient("goTo", "/", client);

				sendMessage(0, Str::welcome,String(name), client);
				return;
			}else {
				sendMessage(1, String(Str::errorLogin), client);
				return;
			}
		}
	}

	bool auth = false;

	if (cmd.containsKey("token")) {
		const char * token = cmd["token"];

		auth = isAuthenticate(token, client->remoteIP());
	}

	if (!auth) {
		// si no estamos identificado
		// llevamos al cliente a la pagina de login
		sendClient("goTo", "/login");
		sendMessage(1, String(Str::reqLogin), client);
		return;
	}

	// 
	if (cmd.containsKey("auth")) {
		bool log = cmd["auth"];
		if (log) {
			saveClientAuth(client);
			client->text(printJsonOption());
			client->text(jsonFiles);
		}
		else removeClientAuth(client);
	}

	if (cmd.containsKey("tap")) {
		JsonObject tap = cmd["tap"];

		if (tap.containsKey("id") && tap.containsKey("open"))
		{
			int id = tap["id"]; bool open = tap["open"];
			String const & name = String(taps[id]->name);
			// abrir/cerrar grifo
			if (openTap(id, open)) {
				//notificamos 
				sendMessage(0, (open ? Str::open: Str::close),name);
				//mostamos la pagina grifos
				if (open) sendClient("goTo", "/taps");
			}
			else // a ocurrido un error
				sendMessage(1, Str::errorOpen , name, client);
		}
	}

	if (cmd.containsKey("zone")) 
		receivedJsonZone(client, cmd["zone"]);

	if (cmd.containsKey("alarm")) 
		receivedJsonAlarm(client,cmd["alarm"]);

	if (cmd.containsKey("system") && cmd["system"].containsKey("restart")) {
		ESP.restart();
	}
	bool modified = false;
	if (cmd.containsKey("config")) {
		JsonObject con = cmd["config"];
		String r;
		serializeJson(con, r);
		Serial.println(r);

		if (con.containsKey("tz") && con.containsKey("dst")) {
			config.setTimeZone(con["dst"].as<char*>(), con["tz"].as<int>());
			updateTimeZone();
			sendAll(printJsonForecast());
			modified = true;
		}

		if (con.containsKey("wifi_ssid") && con.containsKey("wifi_pass")) {
			config.setWifi(
				con["wifi_ssid"].as<char*>(), con["wifi_pass"].as<char*>()
			);
			modified = true;
			//TODO actualizar wifi o resetEsp
		}

		if (con.containsKey("cityName") && con.containsKey("accuURL") 
				&& con.containsKey("cityID")) 
		{
			config.setAccu(
				con["cityID"].as<char*>(),
				con["cityName"].as<char*>(), 
				con["accuURL"].as<char*>()
			);
			modified = true;
			if (connectedWifi) loadForecast();
		}

		if (con.containsKey("www_user") && con.containsKey("www_pass")) {
			config.setAdmin(
				con["www_user"].as<char*>(),
				con["www_pass"].as<char*>()
			);
			modified = true;
			//TODO actualizar werver pass :???
		}

		if (modified) {
			writeJson("/data/config.json", &config);
			sendMessage(0, String(Str::edit));
			sendAllAuth(printJsonOption());
			// dispach event
		}
	}

}

void Model::receivedJsonZone(AsyncWebSocketClient * client, const JsonObject & zone)
{
	if (zone.containsKey("id") && zone.containsKey("runing")
		&& zone["runing"].as<bool>())
	{
		int id = zone["id"];
		const String & name = String(zones[id]->name);


		if (waterZone(id)) {
			sendMessage(0, Str::open, name);

		}
		else if(isWatering()) 
			sendMessage(1, String(Str::errorBusy), client);
		else sendMessage(1, Str::errorOpen , name, client);
	}
	else if (zone.containsKey("runing") && !zone["runing"].as<bool>())
	{
		String name = "";
		if (currentZone) name = String(currentZone->name);

		if (stopWaterZone()) {
			sendMessage(0, Str::close, name);
		}
		else sendMessage(1, Str::errorClose, name, client);
	}
	else if (zone.containsKey("pause"))
	{
		bool pause = zone["pause"];

		String name = "";
		if (currentZone != nullptr) name = String(currentZone->name);

		if (pauseWaterZone(pause))
			sendMessage(0, Str::pause, name);
		else sendMessage(1, Str::errorPause, name, client);
	}
	else if (zone.containsKey("delete"))
	{
		int id = zone["delete"];

		String name = String(Str::notFound);
		if (zones.has(id)) name = String(zones[id]->name);

		if (removeZone(id))
			sendMessage(0, Str::deleteItem , name);
		else sendMessage(1, Str::errorDelete , name, client);
	}
	else if (zone.containsKey("edit"))
	{
		JsonObject edit = zone["edit"];
		int id = edit["id"];uint32_t modes = edit["modes"];
		const char * name = edit["name"];

		if (editZone(id, name, modes))
			sendMessage(0, Str::edit , name);
		else sendMessage(1, Str::errorEdit , name, client);
	}
	else if (zone.containsKey("new"))
	{
		JsonObject newZone = zone["new"];

		uint32_t modes = newZone["modes"];
		const char * name = newZone["name"];

		if (addZone(name, modes))
			sendMessage(0, Str::create , name);
		else
			sendMessage(1, Str::errorCreate , name, client);
	}
}

void Model::receivedJsonAlarm(AsyncWebSocketClient * client, const JsonObject & alarm)
{
	if (alarm.containsKey("delete"))
	{
		int id = alarm["delete"];

		String msg = lang.get(Str::notFound);

		if (alarms.has(id)) {
			AlarmItem* a = alarms[id];
			msg = String(zones[a->zoneId]->name) + " " + Tasker::formatTime(a->time);
		}

		if (removeAlarm(id))
			sendMessage(0, Str::deleteItem, msg);
		else
			sendMessage(1, Str::errorDelete, msg, client);
	}
	else if (alarm.containsKey("edit"))
	{
		JsonObject edit = alarm["edit"];

		int id = edit["id"];uint32_t time = edit["time"];
		int tapId = edit["tapId"];int duration = edit["duration"];

		String msg = lang.get(Str::notFound);

		AlarmItem* ocuped = nullptr;
		
		isAlarmUsingTime(time, duration, id);

		if (alarms.has(id)) {
			AlarmItem* a = alarms[id];
			msg = String(zones[a->zoneId]->name) + " : " + Tasker::formatTime(time);
		}

		if (ocuped) {
			msg = String(zones[ocuped->zoneId]->name) + " : " +
				Tasker::formatTime(ocuped->time) + " " + Tasker::formatTime(time);

			sendMessage(1, Str::occupied, msg, client);
		}
		else if (editAlarm(id, tapId, time, duration))
			sendMessage(0, Str::edit, msg);
		else sendMessage(1, Str::errorEdit, msg, client);
	}
	else if (alarm.containsKey("new"))
	{
		JsonObject edit = alarm["new"];

		int zoneId = edit["zoneId"];uint32_t time = edit["time"];
		int tapId = edit["tapId"];int duration = edit["duration"];

		String msg = lang.get(Str::notFound);

		isAlarmUsingTime(time, duration);

		AlarmItem* ocuped = nullptr;
		

		if (zones.has(zoneId))
			msg = String(zones[zoneId]->name) + " : " + Tasker::formatTime(time);

		if (ocuped) {
			msg = String(zones[ocuped->zoneId]->name) + " : " +
				Tasker::formatTime(ocuped->time) + " " + Tasker::formatTime(time);

			sendMessage(1, Str::occupied, msg, client);
		}
		else if (addAlarm(zoneId, tapId, time, duration)>-1)
			sendMessage(0, Str::create, msg);
		else
			sendMessage(1, Str::errorCreate, msg, client);
	}
}

bool Model::isValide(const char * user, const char * pass)
{
	return strcmp(config.www_user, user) == 0 &&
		strcmp(config.www_pass, pass) == 0;
}

String Model::createToken(const IPAddress & ip)
{
	return sha1("jardin:" + 
		String(config.www_user) + ":" +
		String(config.www_pass) + ":" +
		ip.toString()
	);
}

void Model::saveClientAuth(AsyncWebSocketClient * client)
{
	auto it = std::find(clientsAuth.begin(), clientsAuth.end(), client);
	if (it == clientsAuth.end()) {
		clientsAuth.push_back(client);
	}
	Serial.printf("clientAuth save size%d\n", clientsAuth.size());
}

void Model::removeClientAuth(AsyncWebSocketClient * client)
{
	//buscamos si esta en la lista de logeados si es asi se borra
	auto it = std::find(clientsAuth.begin(), clientsAuth.end(), client);
	if (it != clientsAuth.end()) {
		clientsAuth.erase(it);
	}

	Serial.printf("clientAuth remove size%d\n", clientsAuth.size());
}

bool Model::isAuthenticate(const String & token, const IPAddress & ip)
{
	return token == createToken(ip);
}

bool Model::isAuthenticate(AsyncWebServerRequest * request)
{
	if (request->hasHeader("Authorization")) {
		String authStr = request->header("Authorization");
		//remove "Basic "
		String token = authStr.substring(6);
		return isAuthenticate(token, request->client()->remoteIP());
	}
	Serial.println("not head");
	return false;
}

//		app

// pausa el riego de la zona en curso
bool Model::pauseWaterZone(bool pause)
{
	if (currentZone == nullptr || 
		 (currentZone != nullptr && currentZone->paused == pause)
		) return false;

	//Serial.printf("\n--pauseWaterZone %d\n", currentZone->id);

	Task * t = currentAlarm->task;

	bool r = false;
	bool iswate = isWatering();

	uint32_t now = tasker.timeNow();
/*
	Serial.printf("now %s ", Tasker::formatTime(now));
	Serial.printf("elapsedPausedTask %s ", Tasker::formatTime(elapsedPausedTask));
	Serial.printf("pausedTime %s \n", Tasker::formatTime(pausedTime));*/
	//pausamos
	if (iswate && !isPaused()) {

		// tiempo trancurrido de la tarea actual
		elapsedPausedTask = Tasker::getDuration(t->start, now);
		pausedTime = now;

		uint16_t da = currentAlarm->duration;
		uint16_t dt = Tasker::getDuration(t->start, t->stop);

		// cuando la duracion de la alarma es mas grande 
		// que la duracion de la tarea (porq fue pausado)
		if (da > dt) {
			//sumamos esa dife
			elapsedPausedTask += (da - dt);
		}

		//detenemos todas las alarmas de la zona
		for (auto &it : alarms) {
			AlarmItem * alarm = it.second;

			if (alarm->zoneId == currentZone->id) {
				Task * t = alarm->task;
				t->enabled = false;
				t->runing = false;
				//t->start = alarm->init;
				//t->stop = alarm->end;
			}
		}

		currentZone->paused = true;
		currentZone->elapsed = getElapsedAlarm();

		sendAll(printJson("zone", currentZone));


		addHistory(clockTime.local(),Action::pausedA, 1, currentZone->id);
		// cerramos el q esta activo 
		r = openTap(currentAlarm->tapId, false); // apaga el pin

		//Serial.println("--pauseWaterZone pausado");
	}
	else if (!iswate && !pause) {//reanudamos

		uint16_t elapsed = Tasker::getDuration(pausedTime, now);
		// 
		t->enabled = true;
		t->start = now;
		t->stop = (now + currentAlarm->duration - elapsedPausedTask) % TASK_TICKS_24H;

		//Serial.printf("alarm id %d ", currentAlarm->id);
		//Serial.print("current task :"); printTask(t);


		//if (isManualZoneWater) {

			bool found = false;
			int index = 0;
			uint32_t start = t->stop;

			// iniciamos desde currentalarm index
			for (auto &it : alarms) {
				AlarmItem * alarm = it.second;
				if (alarm->zoneId == currentZone->id)
				{
					if (alarm == currentAlarm) found = true;
					else if (found) {
						index++;

						Task * ct = alarm->task;
						ct->enabled = true;//por si fue cancelado
						ct->runing = false;

						//sumamos la pausa
						ct->start = (ct->start + elapsed) % TASK_TICKS_24H;
						ct->stop = (ct->stop + elapsed) % TASK_TICKS_24H;
					}

					//Serial.printf("for reanuda found %d %d\n", found, index);
				}
			}
		//}

		elapsedPausedTask = 0;
		pausedTime = 0;
		currentZone->paused = false;
		currentZone->elapsed = getElapsedAlarm();

		sendAll(printJson("zone", currentZone));

		addHistory(clockTime.local(), Action::pausedA, 0, currentZone->id);

		r = openTap(currentAlarm->tapId, true); // enciende el pin;

		//Serial.println("--pauseWaterZone reanudado");
	}

	//Serial.println("--pauseWaterZone final");
	return r;
}

//llamado de evento para tasker x tiempo (alarmas)
void Model::onWater(Task * t)
{
	// si se abre los grifos manualment
	// o esta pausado
	if (isManualWater || isPaused()) {
		Serial.printf("exit onWater manualwater %s || paused %s \n",
			isManualWater ? "true" : "false",
			isPaused() ? "true" : "false");
		// nos salimos
		return;
	}

	//reseteamos uno que fue cancelado 
	// y salimos
	if (!t->enabled) {
		t->enabled = true;
		return;
	}

	//buscamos la correspondencia con la alarma
	bool found = false;
	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;
		if (alarm->task == t) {
			currentAlarm = alarm;
			currentZone = zones[alarm->zoneId];
			currentZone->alarmId = alarm->id;
			currentZone->runing = t->runing;
			found = true;
			break;
		}
	}

	if (found)
	{
		// si no se puede regar cancelamos
		if (t->runing && !isManualZoneWater 
			&& !canWatering(currentZone->modes)) 
		{
			stopWaterZone();
			return;
		}

		if (t->runing) {
			//actualizamos clientes
			const String & str = printJson("zone", currentZone);
			sendAll(str);
			Serial.println(str);

			
			//mostrar a todos los clientes q sa iniciado
			String path = "/zones/"+String(currentZone->id);
			sendClient("goTo", path.c_str());
		}

		addHistory(clockTime.local(), 
			isManualZoneWater ? Action::zoneManualA : Action::zoneA, t->runing, currentAlarm->id);
		
		openTap(currentAlarm->tapId, t->runing);

		if (!t->runing) {
			// si fue activado el riego de la zona manualmente
			// y es la ultima alarma de la zona
			if (isManualZoneWater && isLastAlarm(currentAlarm)) {
				isManualZoneWater = false;
				// reseteamos los tasks de la zona
				reloadTasks(currentZone->id);
			}

			//stop current
			currentZone->runing = false;
			currentZone->alarmId = -1;
			currentZone->elapsed = 0;
			//actualizamos clientes
			const String & str = printJson("zone", currentZone);
			sendAll(str);
			Serial.println(str);
			//borramos
			currentZone = nullptr;
			currentAlarm = nullptr;
			elapsedPausedTask = 0;

			//disparamos evento
			status = Status::standby;
			dispachEvent(EventType::status);
		}

	}
	else
		Serial.printf("Meeec! onWater task not found:%d isWatering:%d zoneId:%d iindex:%d\n",
			found, t->runing, currentZone, currentAlarm);
}


// detiene el riego de la zona en curso
bool Model::stopWaterZone()
{
	Serial.println("stopWaterZone");

	if (currentZone == nullptr) return false;

	//cancelamos las siguientes tareas
	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;

		if (alarm->zoneId == currentZone->id) {

			Task * t = alarm->task;

			t->enabled = false;
			t->runing = false;

			// cuando task a sido manipulado
			// lo reseteamos
			if (isManualZoneWater || isPaused()) {
				t->start = alarm->time;
				t->stop = alarm->time + alarm->duration;
			}
		}
	}

	isManualZoneWater = false;

	currentZone->paused = false;
	currentZone->runing = false;
	currentZone->elapsed = getElapsedAlarm();

	sendAll(printJson("zone", currentZone));


	addHistory(clockTime.local(),Action::zoneManualA, 0, currentAlarm->id);

	// cancelamos el q esta activo 
	bool r = openTap(currentAlarm->tapId, false); // apaga el pin

	currentZone = nullptr;
	currentAlarm = nullptr;
	elapsedPausedTask = 0;
	pausedTime = 0;

	status = Status::standby;
	dispachEvent(EventType::status);
	return r;
}

bool Model::waterZone(uint8_t zoneId)
{
	if (!isWatering() && zones.has(zoneId)) {

		uint32_t current = tasker.timeNow();

		isManualZoneWater = true;

		for (auto &it : alarms) {
			AlarmItem * alarm = it.second;

			if (alarm->zoneId == zoneId) {
				Task * t = alarm->task;

				t->runing = false;
				t->enabled = true;//por si fue cancelado
				// encolamos alarmas una tras otra
				t->start = current;
				t->stop = (current + alarm->duration) % TASK_TICKS_24H;

				current = t->stop;
			}
		}
		return true;
	}
	return false;
}

bool Model::openTap(uint8_t tapId, bool val)
{
	// no existe nos salimos
	if (!taps.has(tapId)) return false;

	TapItem * tap = taps[tapId];

	if (val)
		digitalWrite(tap->pin, ON);// Enciende el pin 
	else digitalWrite(tap->pin, OFF);// apaga el pin

	tap->open = val;

	//update Clients
	sendAll(printJson("taps", &taps));

	addHistory(clockTime.local(), isManualWater ? Action::tapManualA : Action::tapA, val, tap->id);
	saveHistory();

	status = Status::watering;
	dispachEvent(EventType::status);

	if (!isWatering()) {
		isManualWater = false;
		status = Status::standby;
		dispachEvent(EventType::status);
	}
	return true;
}

// default json
void Model::loadDefaultZones()
{
	Serial.println("loadDefault");

	zones.clear();
	alarms.clear();

	uint8_t duration = 60;

	///*	//grifo 0 27	//grifo 1 14	//grifo 2 21	//grifo 3 17
	//*/
	firevent = false;
	int id = addZone("Cesped D", 0);

	addAlarm(id, 0, duration, 7, 0, 0);
	addAlarm(id, 1, duration, 7, 0, duration);
	addAlarm(id, 2, duration, 7, 0, duration * 2);

	duration = 20;

	id = addZone("Cesped N", 0);
	addAlarm(id, 0, duration, 21, 0, 0);
	addAlarm(id, 1, duration, 21, 0, duration);
	addAlarm(id, 3, duration, 21, 0, duration * 2);

	id = addZone("Huerta", 0);
	addAlarm(id, 3, duration, 14, 0, 0);
	addAlarm(id, 3, duration, 14, 0, duration);

	writeJson("/data/zones.json", &zones);
	writeJson("/data/alarms.json", &alarms);
	firevent = true;
}


void Model::loadDefaultTaps()
{
	taps.clear();
	uint8_t pins[4] = { 27,14,21,17 };
	char buffer[20] = "Grifo 1000";

	for (size_t i = 0; i < 4; i++)
	{
		sprintf(buffer, "Grifo %d", i);
		TapItem* tap = taps.getEmpty();
		tap->set(-1, pins[i], buffer);
		taps.push(tap);
	}

	writeJson("/data/taps.json", &taps);
}

// comprobamos si hay grifos abiertos
bool Model::isWatering()
{
	for (auto &it : taps) {
		TapItem * tap = it.second;
		if (tap->open) return true;
	}

	return false;
}
// si esta pausado
bool Model::isPaused()
{
	return pausedTime > 0;
}

//		wifi
void Model::load()
{
	loadLocalTime();
	loadForecast();
}

void Model::connectWifi()
{
	strcpy(msgStatus, lang.get(Str::conWifi).c_str());
	strcat(msgStatus, config.wifi_ssid);

	dispachEvent(EventType::status);

	WiFi.begin(config.wifi_ssid, config.wifi_pass);

	uint16_t timeout = 20000;
	uint32_t current_time = millis();
	uint32_t delay_time = millis();

	while (millis() - current_time < timeout) {

		//delay(500);

		bool delayed = millis() - delay_time > 500;

		if (delayed) {
			delay_time = millis();
		}

		if (delayed && WiFi.status() == WL_CONNECTED) {

			connectedWifi = true;

			Serial.println("WiFi connected");
			//if (MDNS.begin("jardin")) {
			//	Serial.println("MDNS responder started");
			//}
			strcpy(msgStatus, lang.get(Str::conWifi1).c_str());
			strcat(msgStatus, config.wifi_ssid);

			Serial.printf("%s", msgStatus);
			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());

			dispachEvent(EventType::conectedWifi);

			// descarga el tiempo y meteo
			load();
			return;
		}
	}

	Serial.println("WiFi Error");
	strcpy(msgError, lang.get(Str::errorWifi).c_str());

	dispachEvent(EventType::error);

	//volvemos intentarlo en 60sg
	retryLater();
}

void Model::enableSoftAP()
{
	WiFi.persistent(false);
	WiFi.setSleep(false);

	WiFi.mode(WIFI_AP_STA);
	delay(2000);//?
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	
	WiFi.softAP("Jardin");

	//IPAddress apIP(8, 8, 4, 4); // The default android DNS

	Serial.printf("sleep %d",WiFi.getSleep());

	Serial.printf(" softAPIP : %s , apIP %s\n",WiFi.softAPIP().toString().c_str(),
		apIP.toString().c_str());

	// modify TTL associated  with the domain name (in seconds)
	// default is 60 seconds
	//dnsServer.setTTL(300);
	// set which return code will be used for all other domains (e.g. sending
	// ServerFailure instead of NonExistentDomain will reduce number of queries
	// sent by clients)
	// default is AsyncDNSReplyCode::NonExistentDomain
	//dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);


	//// if DNSServer is started with "*" for domain name, it will reply with
	//// provided IP to all DNS request
	dnsServer.start(53, "*", apIP);

	startWebServer();
}

//					time
void Model::loadLocalTime()
{
	if (clockTime.updateNTP()) 
	{
		loadedTime = true;

		char buff[9];
		Tasker::formatTime(buff, tasker.timeNow());

		strcpy(msgStatus, lang.get(Str::updatedTime).c_str());
		strcat(msgStatus, buff);
		//actualizar alarmas
		updateTasks();
		dispachEvent(EventType::loadedTime);
		return;
	}
	// a fallado
	Serial.println("Failed to obtain time");
	strcpy(msgError, lang.get(Str::errorTime).c_str());

	loadedTime = false;
	dispachEvent(EventType::error);
	retryLater();
}

void Model::retryLater()
{
	using namespace std::placeholders;
	tasker.setTimeout(std::bind(&Model::onRetry, this, _1), retryTime);
}

void Model::onRetry(Task * current)
{
	Serial.println("retry");
	if (!connectedWifi) {
		connectWifi();
		return;
	}
	if (!loadedTime)
		loadLocalTime();
	if (!loadedForescast)
		loadForecast();
}

//		sensor
void Model::updateSensor()
{
	bme.takeForcedMeasurement();

	if (isnan(bme.readTemperature()) || isnan(bme.readPressure()) ||
		isnan(bme.readHumidity()) || bme.readHumidity() == 255)
	{
		Serial.println("sensor isnan");
		delay(100);
		updateSensor();
		return;
	}

	currentSensor.temperature = bme.readTemperature();
	currentSensor.pressure = round(bme.readPressure()) / 100.0F;
	currentSensor.humidity = bme.readHumidity();
	currentSensor.time = clockTime.timeNow();

	const String &json = printJson("sensor", &currentSensor);
	ws.textAll(json);

	dispachEvent(EventType::sensor);
}


void Model::saveHistory()
{
	const String &json = printJson("history", &history);
	Serial.println(json);
	ws.textAll(json);
	writeJson("/data/history.json", &history);
}

void Model::addHistory(uint32_t time, Action action, uint8_t value, int32_t idItem)
{
	if (history.size() >= history.maxSize) {
		history.shift();
	}

	ActionItem* a = history.getEmpty();
	a->set(time, action, value, idItem);

	history.push(a);
}

void Model::saveLoger(Task * t)
{
	if (sensors.size() >= sensors.maxSize) {
		sensors.shift();
	}

	SensorItem* sensor = sensors.getEmpty();

	sensor->set(
		currentSensor.temperature,
		currentSensor.pressure,
		currentSensor.humidity,
		currentSensor.time
	);

	sensors.push(sensor);

	const String & str = sensors.serializeString();

	writeFile("/data/logSensors.json", str.c_str());

	uint32_t c = millis();
	const String & json = printJson("sensors", &sensors);
	ws.textAll(json.c_str());

	//Serial.println(str);

	dispachEvent(EventType::sensorLog);
}

void Model::saveLoger24(Task* t)
{
	if (sensors24.size() >= sensors24.maxSize) {
		sensors24.shift();
	}

	SensorAvgItem* sensor = sensors24.getEmpty();

	int8_t minTemp = 100; int avgTemp; int8_t maxTemp;
	int8_t minHum = 100; int avgHum; int8_t maxHum;

	if (!sensors.size()) return;

	for (SensorItem* item : sensors)
	{
		minTemp = (minTemp < item->temperature) ? minTemp : item->temperature;
		minHum = (minHum < item->humidity) ? minHum : item->humidity;

		avgTemp += item->temperature;
		avgHum += item->humidity;

		maxTemp = (maxTemp > item->temperature) ? maxTemp : item->temperature;
		maxHum = (maxHum > item->humidity) ? maxHum : item->humidity;
	};

	avgTemp /= sensors.size();
	avgHum /= sensors.size();

	uint32_t time = clockTime.local();
	sensor->set(
		minTemp, avgTemp, maxTemp,
		minHum, avgHum, maxHum,
		time
	);

	sensors24.push(sensor);

	const String& str = sensors.serializeString();

	writeFile("/data/logSensors24.json", str.c_str());

	uint32_t c = millis();
	const String& json = printJson("sensors24", &sensors24);
	ws.textAll(json.c_str());

	//Serial.println(str);

	dispachEvent(EventType::sensorLog24);
}

//		Meteo
void Model::loadForecast()
{
	if (client.connect(host, 80)) {

		// This will send the request to the server
		client.print(String("GET ") +
			url1 + config.cityID + url2 + config.accuURL +
			" HTTP/1.1\r\n" + "Host: " + host + "\r\n" +
			"Connection: close\r\n\r\n");

		unsigned long timeout = millis();

		while (client.available() == 0) {
			if (millis() - timeout > 5000) {
				Serial.println(">>> Client Timeout !");
				client.stop();
				break;
			}
		}

		// HTTP headers end with an empty line
		if (client.available() && client.find("\r\n\r\n")
			&& parseForescast()) {

			loadedForescast = true;

			strcpy(msgStatus, lang.get(Str::updatedFore).c_str());

			dispachEvent(EventType::loadedForescast);

			sendAll(printJsonForecast());
			sendAll(printJson("zones", &zones));

			updateSunTask();

			return;
		}
	}

	strcpy(msgError, lang.get(Str::errorFore).c_str());

	loadedForescast = false;
	dispachEvent(EventType::error);

	retryLater();
}

bool Model::parseForescast()
{
	DynamicJsonDocument doc(60000);

	String json = client.readStringUntil('\r');

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, json);

	// Test if parsing succeeds.
	if (error) {
		Serial.print("deserializeJson() failed: ");
		Serial.println(error.c_str());

		//DeserializationError  IncompleteInput
		return false;
	}

	// Get the modes object in the document
	JsonObject root = doc.as<JsonObject>();

	// probamos el json q es correcto
	if (!root.containsKey("DailyForecasts")) {
		Serial.println("forecast error estructura json");
		return false;
	}

	JsonObject DailyForecasts = root["DailyForecasts"][0];

	weather.time = DailyForecasts["EpochDate"]; // 1532149200
	weather.temp[0] = DailyForecasts["Temperature"]["Minimum"]["Value"];
	weather.temp[1] = DailyForecasts["Temperature"]["Maximum"]["Value"];

	Serial.printf("Temperatura min %d° max %d°\n", weather.temp[0], weather.temp[1]);

	JsonObject sun = DailyForecasts["Sun"];

	weather.sun[0] = (uint32_t)sun["EpochRise"] + clockTime.tz(); // 1536213600;
	weather.sun[1] = (uint32_t)sun["EpochSet"] + clockTime.tz();// 1536260160;

	float hoursOfSun = DailyForecasts["HoursOfSun"]; // 5.9

//dia
	JsonObject Forecasts_Day = DailyForecasts["Day"];
	WeatherData::Forecast * day = &weather.forecast[0];

	strcpy(day->phrase, Forecasts_Day["LongPhrase"]);// "Intervalos de nubes y sol"

	//Serial.printf("json len:%d sizeof:%d ",json.length(),sizeof(DailyForecasts0_Day["ShortPhrase"]));

	day->icon = Forecasts_Day["Icon"]; // 4


	day->precipitationProbability = Forecasts_Day["PrecipitationProbability"]; // 25

	day->win[0] = Forecasts_Day["Wind"]["Speed"]["Value"]; // 9.3// "km/h"
	day->win[1] = Forecasts_Day["Wind"]["Direction"]["Degrees"]; // 332

	day->totalLiquid = Forecasts_Day["TotalLiquid"]["Value"];// 0// "mm"

	day->hoursOfPrecipitation = Forecasts_Day["HoursOfPrecipitation"]; // 0
	day->cloudCover = Forecasts_Day["CloudCover"]; // 67

	// noche
	JsonObject Forecasts_Night = DailyForecasts["Night"];
	WeatherData::Forecast * night = &weather.forecast[1];

	strcpy(night->phrase, Forecasts_Night["LongPhrase"]);// "Intervalos de nubes y sol"
	night->icon = Forecasts_Night["Icon"]; // 36
	night->precipitationProbability = Forecasts_Night["PrecipitationProbability"]; // 25

	//Serial.printf("icon dia %d icon noche %d", day->icon,night->icon);

	night->win[0] = Forecasts_Night["Wind"]["Speed"]["Value"]; // 9.3// "km/h"
	night->win[1] = Forecasts_Night["Wind"]["Direction"]["Degrees"]; // 332

	night->totalLiquid = Forecasts_Night["TotalLiquid"]["Value"];// 0// "mm"

	night->hoursOfPrecipitation = Forecasts_Night["HoursOfPrecipitation"]; // 0
	night->cloudCover = Forecasts_Night["CloudCover"]; // 67

	return true;

}

//TODO : falla settimeout
void Model::updateSunTask()
{

	uint32_t s;

	uint32_t now = clockTime.local();

	if (isDay()) s = weather.sun[1] - clockTime.local();
	else s = weather.sun[0] - clockTime.local();

	char buff[9], buff1[9];
	Tasker::formatTime(buff, weather.sun[0]);
	Tasker::formatTime(buff1, weather.sun[1]);

	Serial.printf("sol se levanta a %s se acuesta a %s\n", buff, buff1);
	Serial.printf("proxima actualizacion sol a %s\n", isDay() ? buff1 : buff);
	Serial.printf("faltan %d sun1 %d sun %d now %s\n", s, weather.sun[1], weather.sun[0],Tasker::formatTime(now));

	using namespace std::placeholders;
	//Task * t = tasker.setTimeout(std::bind(&Model::onSunChanged, this, _1), s );
	//Tasker::printTask(t);
	//Serial.println("teta");
	
}

void Model::onSunChanged(Task * t)
{
	Serial.printf("onSunChanged, %s %s %s\n",
		Tasker::formatTime(tasker.timeNow()).c_str(),
		Tasker::formatTime(clockTime.timeNow()).c_str(),
		Tasker::formatTime(clockTime.timeNow() % TASK_TICKS_24H).c_str()
	);
	Tasker::printTask(t);

	//loadForecast();
}

bool Model::isDay()
{
	if (loadedForescast) {
		return weather.isDay();
	}
	return true;
}

//		Modes
bool Model::canWatering(uint flag)
{
	modes.setFlags(flag);
	int8_t evaluate = modes.evaluate();
	bool skip = modes.skip();
	
	Serial.printf("skip %d evaluate %d result %d\n", 
		skip, evaluate, (!skip && (evaluate < 50 || evaluate < 0)));
	// 
	return !skip && (evaluate < 50 || evaluate < 0);

}

//		web server
void Model::startWebServer()
{
	using namespace std::placeholders;
	//websocket
	ws.onEvent(std::bind(&Model::onWsEvent, this,_1,_2,_3,_4,_5,_6));
	server.addHandler(&ws);
	// file manager
	server.on("/file", HTTP_ANY, std::bind(&Model::onFilePage, this, _1),
		std::bind(&Model::onUploadFile, this, _1, _2, _3, _4, _5, _6));
	//root
	server.serveStatic("/", fs, "/www/").setDefaultFile("index.html");

	server.onNotFound([](AsyncWebServerRequest *request) {
		request->redirect("/");
	});

	server.begin();
}

//fix cors errors
void Model::sendResponse(AsyncWebServerRequest * request, AsyncWebServerResponse * response)
{
	//limit max requests
	response->addHeader("Connection", "Keep-Alive");
	response->addHeader("Keep-Alive", "max=2");
	//fix cors errors
	response->addHeader("Access-Control-Allow-Origin", "*");
	request->send(response);
}

//		file
void Model::onFilePage(AsyncWebServerRequest * request)
{

	if (request->method() == HTTP_OPTIONS) 
	{
		AsyncWebServerResponse *response = request->beginResponse(200);
		response->addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
		response->addHeader("Access-Control-Allow-Headers", "*");
		sendResponse(request, response);
		Serial.println("HTTP_OPTIONS");
		return;
	}

	if (!isAuthenticate(request)) {
		//no authorized bye bye
		Serial.println("bye bye");
		sendResponse(request, request->beginResponse(401));
		return;
	}

	if (request->method() == HTTP_GET)
	{
		if (request->hasArg("download")) {

			String filename = request->arg("download");
			Serial.println("Download Filename: " + filename);

			if (fs.exists(filename))
				sendResponse(request, request->beginResponse(fs, filename, String(), true));
			else
				sendResponse(request, request->beginResponse(404));

		}
		else if(request->hasArg("delete")) {

			String filename = request->arg("delete");
			Serial.println("delete Filename: " + filename);

			if (fs.exists(filename) && fs.remove(filename)) {
				sendResponse(request, request->beginResponse(200));
				jsonFiles = printJsonFiles();
				sendAllAuth(jsonFiles);
			}
			else
				sendResponse(request, request->beginResponse(404));
		}
	}
	//upload
	else if (request->method() == HTTP_POST)
	{
		String path = "/data";
		if (request->hasArg("path")) path = request->arg("path");

		if (request->hasParam("file", true, true) &&
			fs.exists(path +"/"+ request->getParam("file", true, true)->value())) {
			
			sendResponse(request, request->beginResponse(200));//ok
			jsonFiles = printJsonFiles();
			sendAllAuth(jsonFiles);
		}	
		else 
			sendResponse(request, request->beginResponse(404));// error
	}
}

void Model::onUploadFile(AsyncWebServerRequest * request, const String & filename,
	size_t index, uint8_t * data, size_t len, bool final)
{
	static bool authenticate = false;

	String path = "/data";
	if (request->hasArg("path")) {
		path = request->arg("path");
	}

	if (!index) {

		if (isAuthenticate(request)) {

			String p = path + "/" + filename;
			Serial.printf("UploadStart: %s\n", p.c_str());
			request->_tempFile = fs.open(p, "w");
			authenticate = true;
		}
		else authenticate = false;
	}

	Serial.println((request->_tempFile ? "siiii" : "nçooooo"));

	if (authenticate && request->_tempFile) {
		if (len) {
			request->_tempFile.write(data, len);
		}
		if (final) {
			request->_tempFile.close();
			authenticate = false;
			Serial.printf(" existe %d", fs.exists(filename));
		}
	}

	for (size_t i = 0; i < len; i++) {
		Serial.write(data[i]);
	}

	if (final) Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
}

void Model::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, 
	AwsEventType type, void * arg, uint8_t * data, size_t len)
{
	if (type == WS_EVT_CONNECT) {

		const String & json = printJsonFirstRun();

		client->printf(json.c_str());
		client->ping();
		Serial.printf("ws[%s][%u] ip%d connect and send :\n", server->url(), client->id());
	}
	else if (type == WS_EVT_DISCONNECT) {
		Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
		removeClientAuth(client);
	}
	else if (type == WS_EVT_ERROR) {
		Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	}
	else if (type == WS_EVT_PONG) {
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
	}
	else if (type == WS_EVT_DATA) {
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";
		if (info->final && info->index == 0 && info->len == len) {
		//	//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)data[i];
				}
			}
			receivedJson(client,msg);
		}
	}
}

//		private

void Model::updateTimeZone()
{

	clockTime.setTimeZone(config.tz);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(
		config.dst[0], config.dst[1], config.dst[2],
		config.dst[3], config.dst[4], config.dst[5], config.dst[6]
	);
}

//bind task alarma y reasignamos los valores
void Model::updateTasks(uint8_t zoneId)
{
	using namespace std::placeholders;

	for (auto &it : alarms)
	{
		AlarmItem * alarm = it.second;
		Task * t = alarm->task;
		//Serial.printf("zoneid :" << zoneId << "alarmid: " << alarm->id << "task :" << (t!= nullptr)  << endl;
		if (alarm->zoneId == zoneId)
		{
			if (t == nullptr)
			{
				//Serial.printf("initAlarm crea task zoneId = %d id: %d time: %d duration: %d\n", zoneId, alarm->id, alarm->time, alarm->duration);

				t = tasker.add(
					alarm->time, alarm->time + alarm->duration,
					std::bind(&Model::onWater, this, _1)
				);

				alarm->task = t;
			} else {
				//Serial.printf("initAlarm recupera task zoneId = %d id :%d time: %d duration :%d\n", zoneId, alarm->id, alarm->time, alarm->duration);
				t->start = alarm->time;
				t->stop = alarm->time + alarm->duration;
			}
		}
	}
}

void Model::updateTasks()
{
	for (auto &it : zones) {
		updateTasks(it.second->id);
	}
}

void Model::reloadTasks(uint8_t zoneId)
{
	Serial.printf("reloadTasks %d\n", zoneId);
	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;

		if (alarm->zoneId == zoneId) {
			Task * t = alarm->task;
			t->start = alarm->time;
			t->stop = alarm->time + alarm->duration;
			t->runing = false;
			t->enabled = true;
		}
	}

}

void Model::calcZones()
{
	for (auto &it : zones) {
		ZoneItem * zone = it.second;
		calcZone(zone->id);
	}
}

void Model::calcModes()
{
	for (auto &it : zones) {
		ZoneItem * zone = it.second;
		zone->can_watering = canWatering(zone->modes);
	}
}

void Model::calcZone(int32_t zoneId)
{
	//refrescamos datos de la zona
	uint32_t dura = 0;
	uint32_t start = UINT32_MAX;
	int16_t index = -1;

	if (zones.has(zoneId)) 
	{
		ZoneItem * zone = zones[zoneId];

		for (auto &it : alarms) {
			AlarmItem * alarm = it.second;
			if (zoneId == alarm->zoneId) {
				dura += alarm->duration;
				start = MIN(start, alarm->time);
			}
		}

		if (start == UINT32_MAX) start = 0;

		zone->duration = dura;
		zone->time = start;
	}
}

//TODO por terminar
AlarmItem * Model::isAlarmUsingTime(uint32_t time, uint16_t duration, int ignoreId)
{
	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;

		uint32_t tStart = alarm->time; 
		uint32_t tEnd = tStart + alarm ->duration; 
		uint32_t timeEnd = time + duration;
		
		if (ignoreId != alarm->id && 
			(time >= tStart && time <= tEnd) || 
			(timeEnd >= tStart && timeEnd <= tEnd)
		){
			Serial.printf("ckeckAlarm ign%d id%d tStart%d tEnd%d time%d timeEnd%d\n ",
				ignoreId, alarm->id, tStart, tEnd,time,timeEnd);
			return alarm;
		}
	}
	return nullptr;
}

bool Model::isLastAlarm(AlarmItem * a)
{
	int8_t zoneId = a->zoneId;

	for (auto &it : alarms) {
		AlarmItem * alarm = it.second;
		if (zoneId == alarm->zoneId &&
			a->time < alarm->time) {
			return false;
		}
	}
	return true;
}

uint16_t Model::getElapsedAlarm()
{
	uint16_t elapsed = 0;

	if (isPaused()) {
		elapsed = elapsedPausedTask;
	}
	else if(currentAlarm && currentAlarm->task){

		Task * t = currentAlarm->task;
		uint16_t da = currentAlarm->duration;
		uint16_t dt = Tasker::getDuration(t->start, t->stop);

		// tiempo trancurrido de la tarea actual
		elapsed = Tasker::getDuration(t->start, tasker.timeNow());

		// cuando la duracion de la alarma es mas grande 
		// que la duracion de la tarea (porq fue pausado)
		if (da > dt) {
			//sumamos esa dife
			elapsed += (da - dt);
		}
	}
	return elapsed;
}