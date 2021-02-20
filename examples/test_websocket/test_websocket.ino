/*
 Name:		test_websocket.ino
 Created:	30/11/2020 12:49:23
 Author:	pp
*/

#include <DNSServer.h>

#include <FS.h>
#include <LITTLEFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

DNSServer dnsServer;
const byte DNS_PORT = 53;
IPAddress apIP(8, 8, 4, 4); // The default android DNS
IPAddress apIP1(8, 8, 4, 3); // The default android DNS
IPAddress apIP3(255, 255, 255, 0); // The default android DNS

const char* ssid = "Movistar_1664";
const char* password = "EGYDRNA6H4Q";
const char * hostName = "jardin";
const char* http_username = "admin";
const char* http_password = "admin";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
	CaptiveRequestHandler() {}
	virtual ~CaptiveRequestHandler() {}

	bool canHandle(AsyncWebServerRequest* request) 
	{
		return request->host() != WiFi.softAPIP().toString();
	}

	void handleRequest(AsyncWebServerRequest* request) {
		AsyncResponseStream* response = request->beginResponseStream("text/html");
		response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
		response->print("<p>This is out captive portal front page.</p>");
		response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
		response->printf("<h3>Try opening <a href='http://%s' target='_blank'>Jardin</a> instead</h3>", WiFi.softAPIP().toString().c_str());
		response->print("</body></html>");
		request->send(response);
	}
};

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

void setup() {
	Serial.begin(115200);
	delay(1000);
	initSever();
}

void initSever() {

	Serial.setDebugOutput(true);

	server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

	Serial.printf("setup\n");

	WiFi.mode(WIFI_AP_STA);
	
	WiFi.softAPConfig(apIP, apIP1, apIP3);

	WiFi.softAP(hostName);

	Serial.print("softIP Address: ");
	Serial.println(WiFi.softAPIP());
	Serial.println(apIP);

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	dnsServer.start(53, "*", apIP);

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
	
	LITTLEFS.begin();

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", String(ESP.getFreeHeap()));
	});

	server.serveStatic("/", LITTLEFS, "/").setDefaultFile("index.html");

	server.onNotFound([](AsyncWebServerRequest *request) {
		Serial.printf("NOT_FOUND: ");
		request->send(404);
	});

	//more handlers...
	server.begin();

}

void loop() {
	static uint32_t c = 0;

	dnsServer.processNextRequest();

	if (millis() - c > 5000) {
		ws.printfAll("hay %d imbeciles conectados",ws.count());
		c = millis();

	}
	ws.cleanupClients();
}
