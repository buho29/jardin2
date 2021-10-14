#include <AUnit.h>
using aunit::TestRunner;
#define private public

#include <WiFi.h>
WiFiClient client;

bool connnect() {
	WiFi.begin("Jardin");

	uint16_t timeout = 20000;
	uint32_t current_time = millis();
	uint32_t delay_time = millis();

	while (millis() - current_time < timeout)
	{
		//delay(500);
		bool delayed = millis() - delay_time > 500;

		if (delayed) delay_time = millis();

		if (delayed && WiFi.status() == WL_CONNECTED) {

			Serial.println("WiFi connected");

			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());
			Serial.println(WiFi.gatewayIP());
			//load();
			TestRunner::exclude("_*");
			return true;
		}
	};


	return false;
}

bool download() {
	IPAddress ip = WiFi.gatewayIP();

	if (client.connect(ip, 80))
	{

		// This will send the request to the server
		/*client.print(String("GET ") +
			"/register?"+ "name=" + "teta" +
			" HTTP/1.1\r\n" + "Host: " + host + "\r\n" +
			"Connection: close\r\n\r\n");*/

		client.print(String("GET ") + "/index.html" +
			" HTTP/1.1\r\n" + "Host: " + ip + "\r\n" +
			"Connection: close\r\n\r\n");

		unsigned long timeout = millis();

		while (client.available() == 0) {
			if (millis() - timeout > 15000) {
				Serial.println(">>> Client Timeout !");
				client.stop();
				break;
			}
		}

		// HTTP headers end with an empty line
		if (client.available() && client.find("\r\n\r\n")) {
			String json = client.readStringUntil('\r');

			Serial.println("client WiFi connected");
			Serial.println(json);
			client.stop();
			//WiFi.disconnect(true);
			return true;
		}
	}
	return false;
}

test(connection) {

	assertTrue(connnect());
	assertTrue(download());
}

void setup() 
{
	delay(1000);
	Serial.begin(115200);
}
void loop()
{
	TestRunner::run();
}