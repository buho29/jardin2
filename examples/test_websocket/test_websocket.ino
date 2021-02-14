/*
 Name:		test_websocket.ino
 Created:	30/11/2020 12:49:23
 Author:	pp
*/


#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
	if (type == WS_EVT_CONNECT) {
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
		client->printf("Hola imbecil n° %u", client->id());
		client->ping();
	}
	else if (type == WS_EVT_DISCONNECT) {
		Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
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
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)data[i];
				}
				server->printfAll("el imbecil n° %d a escrito :\n%S", client->id(), msg);
				if (msg == "fy") {
					client->close();
				}

			}
			Serial.printf("%s\n", msg.c_str());

		}
	}
}


const char* ssid = "Movistar_1664";
const char* password = "EGYDRNA6H4Q";
const char * hostName = "jardin";
const char* http_username = "admin";
const char* http_password = "admin";

void setup() {
	Serial.begin(115200);
	delay(1000);
	initSever();
}

void initSever() {

	Serial.setDebugOutput(true);

	Serial.printf("setup");
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(hostName);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.printf("STA: Failed!\n");
		WiFi.disconnect(false);
		delay(1000);
		WiFi.begin(ssid, password);
	}


	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	MDNS.addService("http", "tcp", 80);

	SPIFFS.begin();

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", String(ESP.getFreeHeap()));
	});

	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	server.onNotFound([](AsyncWebServerRequest *request) {
		Serial.printf("NOT_FOUND: ");
		request->send(404);
	});
	server.begin();
}

void loop() {
	static uint32_t c = 0;

	if (millis() - c > 5000) {
		ws.printfAll("hay %d imbeciles conectados",ws.count());
		c = millis();

	}
	ws.cleanupClients();
}
