#include <AUnit.h>
using aunit::TestRunner;

#define private public

#include"lib/Clock.h"
#include <WiFi.h>

ClockTime clockTime = ClockTime::instance();

void check(uint secs) {
	for (uint i = 0; i < secs; i++)
	{
		delay(1000);
		clockTime.timeNow();

		//DateTime utc(clockTime.utc());

		//Serial.printf("utc %02d:%02d:%02d  %02d/%02d/%d \n",
		//	utc.hour(), utc.minute(), utc.second(),
		//	utc.day(), utc.month(), utc.year()
		//);

		//Serial.printf("is dst %d\n", clockTime.isDst());

		//Serial.printf("dst %02d:%02d:%02d %02d/%02d/%d \n\n",
		//	clockTime.hour(), clockTime.min(), clockTime.sec(),
		//	clockTime.day(), clockTime.month(), clockTime.year()
		//);

	}
}

test(dst) {

	// hours, min, sec, day, mon, year
	clockTime.setTimeNow(0, 59, 57, 29, 3, 2020);

	assertFalse(clockTime.isDst());
	check(4);
	assertTrue(clockTime.isDst());

	clockTime.setTimeNow(0, 59, 57, 25, 10, 2020);
	assertTrue(clockTime.isDst());
	check(4);
	assertFalse(clockTime.isDst());

}

test(setTime) {

	// hours, min, sec, day, mon, year
	clockTime.setTimeNow(1, 0, 0, 29, 3, 2020);

	//Serial.printf("%02d:%02d:%02d  %02d/%02d/%d \n",
	//	clockTime.hour(), clockTime.min(), clockTime.sec(),
	//	clockTime.day(), clockTime.month(), clockTime.year()
	//);

	assertEqual(clockTime.hour(), 3);
	assertEqual(clockTime.min(), 0);
	assertEqual(clockTime.sec(), 0);
	assertEqual(clockTime.day(), 29);
	assertEqual(clockTime.month(), 3);
	assertEqual(clockTime.year(), 2020);

	clockTime.setTimeLocal(1, 0, 0, 29, 3, 2020);

	//Serial.printf("%02d:%02d:%02d  %02d/%02d/%d \n",
	//	clockTime.hour(), clockTime.min(), clockTime.sec(),
	//	clockTime.day(), clockTime.month(), clockTime.year()
	//);

	assertEqual(clockTime.hour(), 1);
	assertEqual(clockTime.min(), 0);
	assertEqual(clockTime.sec(), 0);
	assertEqual(clockTime.day(), 29);
	assertEqual(clockTime.month(), 3);
	assertEqual(clockTime.year(), 2020);


	clockTime.setTimeLocal(0, 0, 0, 29, 3, 2020);

	//Serial.printf("%02d:%02d:%02d  %02d/%02d/%d \n",
	//	clockTime.hour(), clockTime.min(), clockTime.sec(),
	//	clockTime.day(), clockTime.month(), clockTime.year()
	//);

	assertEqual(clockTime.hour(), 0);
	assertEqual(clockTime.min(), 0);
	assertEqual(clockTime.sec(), 0);
	assertEqual(clockTime.day(), 29);
	assertEqual(clockTime.month(), 3);
	assertEqual(clockTime.year(), 2020);

	delay(1000);
}

test(updateNTP) {
	if (connectWifi()) {
		assertTrue(clockTime.updateNTP());
		Serial.printf("%02d:%02d:%02d  %02d/%02d/%d \n",
			clockTime.hour(), clockTime.min(), clockTime.sec(),
			clockTime.day(), clockTime.month(), clockTime.year()
		);
	}
	else assertFalse(true);
}



void setup()
{
	delay(1000);
	Serial.begin(115200);

	clockTime.begin();

	//spain
	clockTime.setTimeZone(1);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(0, 5, 3, 5, 10, 2, 3);


	TestRunner::exclude("_*");


	//if (connectWifi()) Serial.printf(" actualizado : %d \n", clockTime.updateNTP());

}

void loop() {

	TestRunner::run();

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