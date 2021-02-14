#include"lib/Clock.h"
#include <WiFi.h>
void setup()
{
	Serial.begin(115200);

	delay(1000);

	clockTime.begin();

	//spain
	clockTime.setTimeZone(1);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(0, 5, 3, 5, 10, 2, 3);

	// hours, min, sec, day, mon, year
	//clockTime.setTime(0, 59, 57, 29, 3, 2020);
	//clockTime.setTime(0, 59, 57, 25, 10,2020);

	//clockTime.setTime(0, 59, 57, 29, 3, 2020);
	//clockTime.setTime(0, 59, 57, 25, 10,2020);

	//clockTime.setTimeLocal(23, 59, 58, 28, 2, 2019);
	//clockTime.setTimeLocal(23, 59, 58, 28, 2, 2020);

	//if (connectWifi()) {
	//	Serial.printf(" actualizado : %d \n", clockTime.updateNTP());
	//}

}

void loop() {
	clockTime.now();

	DateTime utc(clockTime.utc());

	Serial.printf("utc %02d : %02d : %02d   %02d / %02d / %d \n",
		utc.hour(), utc.minute(), utc.second(),
		utc.day(), utc.month(), utc.year()
	);

	Serial.printf("is dst %d\n", clockTime.isDst());

	Serial.printf("dst %02d : %02d : %02d   %02d / %02d / %d \n\n",
		clockTime.hour(), clockTime.min(), clockTime.sec(),
		clockTime.day(), clockTime.month(), clockTime.year()
	);

	delay(1000);
}

bool connectWifi()
{

	WiFi.begin("Movistar_1664", "EGYDRNA6H4Q");

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

			IPAddress ip = WiFi.localIP();

			Serial.print("IP address: ");
			Serial.println(WiFi.localIP());

			return true;
		}
	}

	Serial.println("WiFi Error");

	return false;
}