
#include "FS.h"
#include <SPIFFS.h>
#include <LITTLEFS.h>
#include <ArduinoJson.h>
#include <Streaming.h>
#include "mbedtls/md.h"

//#include "B.h"

#include "A.h"

void setup() {

	Serial.begin(115200);  
	delay(1000);

/*if (!LITTLEFS.begin(true)) {
		Serial.println("hdp SPIFFS Mount Failed");
		//return;
	}

	Serial.printf("LITTLEFS Total space: %dKb Free: %dKb Used: %dKb \n",
		(LITTLEFS.totalBytes() / 1024), ((LITTLEFS.totalBytes() - LITTLEFS.usedBytes()) / 1024),
		(LITTLEFS.usedBytes() / 1024));

	writeFile(LITTLEFS, "/data/teta.txt", "hola");

	Serial.print("LITTLEFS ");
	listDir(LITTLEFS, "/data/", 3);

	const String & token = sha1(String("admin:admin:192.168.8.100"));
	int t = millis();
	for (size_t i = 0; i < 1000; i++)
	{
		sha1(String("admin:admin:192.168.8.100"));
	}
	Serial.printf(" %dms", (millis() - t), token.c_str());
	
	DynamicJsonDocument doc(512);

	doc["accuURL"] = "SoOCQzMkcUaK83HlGPMz3rxaxlUsEr1a%20&language=es-ES&details=true&metric=true";
	doc["gw"] = WiFi.gatewayIP().toString();
	doc["nm"] = WiFi.subnetMask().toString();
	doc["signalStrengh"] = WiFi.RSSI();
	doc["getFreeSketchSpace"] = ESP.getFreeSketchSpace();
	doc["flashChipSize"] = ESP.getFlashChipSize();
	doc["freeHeap"] = ESP.getFreeHeap();
	doc["heapSize"] = ESP.getHeapSize();

	String buf = "";
	serializeJson(doc, buf);
	Serial.print(buf);*/

	A a;
	a.init();

}

void loop() {}
















































String readFile(fs::FS & fs, const char * path)
{
	Serial.printf("reading file: %s\r\n", path);

	File file = fs.open(path);

	String str = "";

	if (!file || file.isDirectory()) {
		Serial.printf("deserialize- failed to open %s for reading\n", path);
		return str;
	}

	while (file.available())
	{
		str += (char)file.read();
	}
	return str;
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
	Serial.printf("Writing file: %s\r\n", path);

	File file = fs.open(path, FILE_WRITE);
	if (!file) {
		Serial.println("- failed to open file for writing");
		return;
	}
	if (file.print(message)) {
		Serial.println("- file written");
	}
	else {
		Serial.println("- frite failed");
	}
}


const String & sha1(String payloadStr) {
	const char *payload = payloadStr.c_str();

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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				listDir(fs, file.name(), levels - 1);
			}
		}
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}
