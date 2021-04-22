#ifndef _CLOCK_h
#define _CLOCK_h

#include "RTClib.h"

// TODO : not full tested using timelib.h
#define __RTC__
#ifndef __RTC__
#include <TimeLib.h>
#endif

class DST
{
// folk https://github.com/kosme/flex_DST
// added hours
public:
	/*
	Irregularities account for DST schedules that don't begin/end on a Sunday
	Irregularity  / Meaning
	  0             DST begins and ends on a Sunday (Most of the world)
	  1             DST begins on a Friday and ends on a Sunday (Israel)
	  2             DST begins and ends on a Friday (Jordan, Syria)
	  3             DST begins and ends on specific dates (Iran, Western Sahara)
	*/
	DST() {};
	void set(uint8_t irregularity,
		uint8_t beginDay, uint8_t beginMonth, 
		uint8_t endDay, uint8_t endMonth,
		uint8_t beginHour, uint8_t endHour) 
	{
		_irregularity = irregularity & 0x03;
		_beginDay = beginDay;
		_beginMonth = beginMonth;
		_endDay = endDay;
		_endMonth = endMonth;
		_beginHour = beginHour;
		_endhour = endHour;
	};
	boolean checkDST(DateTime RTCTime) {
		uint8_t currentDay = RTCTime.day();
		uint8_t currentMonth = RTCTime.month();
		uint8_t currentDOW = RTCTime.dayOfTheWeek();
		uint8_t currentHour = RTCTime.hour();

		int previousSunday = currentDay - currentDOW;
		if (_beginMonth < _endMonth) { //Northern Hemisphere
			if ((currentMonth > _beginMonth) && (currentMonth < _endMonth)) {
				return true;
			}
		}
		else {
			if ((currentMonth > _beginMonth) || (currentMonth < _endMonth)) {
				return true;
			}
		}

		boolean isBeginHour = currentHour >= _beginHour;
		boolean isEndHour = currentHour >= _endhour;

		if (currentMonth == _beginMonth) {
			if (_irregularity == 0) {
				return  (previousSunday > (7 * (_beginDay - 1))) && isBeginHour;
			}
			else if (_irregularity < 3) { //begins on Friday
				previousSunday = currentDay + 2 - ((currentDOW + 2) % 7);
				if (_beginDay < 5) {
					return(previousSunday > (7 * (_beginDay - 1)) && isBeginHour);
				}
				else {
					return _isLastSunday(currentMonth, previousSunday) && isBeginHour;
				}
			}

			else {
				return currentDay >= _beginDay && isBeginHour;
			}
		}
		else if (currentMonth == _endMonth) {
			if (_irregularity < 2) {
				if (_endDay < 5) {
					return(previousSunday < (7 * (_endDay - 1)) + 1);
				}
				else {
					return !(_isLastSunday(currentMonth, previousSunday) && isEndHour);
				}
			}
			else if (_irregularity == 2) {
				previousSunday = currentDay + 2 - ((currentDOW + 2) % 7);
				if (_endDay < 5) {
					return(previousSunday < (7 * (_endDay - 1)) + 1) && isEndHour;
				}
				else {
					return !(_isLastSunday(currentMonth, previousSunday) && isEndHour);
				}
			}
			else {
				return currentDay < _endDay && isEndHour;
			}
		}
		else {
			return false;
		}
	};
	DateTime calculateTime(DateTime RTCTime) {
		if (checkDST(RTCTime)) {
			_isDst = true;
			return  RTCTime.unixtime() + 3600;
		}
		else {
			_isDst = false;
			return RTCTime;
		}
	};
	boolean isDst() { return _isDst; };

private:
	uint8_t _irregularity;
	uint8_t _beginDay;
	uint8_t _beginHour;
	uint8_t _beginMonth;
	uint8_t _endDay;
	uint8_t _endMonth;
	uint8_t _endhour;
	boolean _isDst = false;

	boolean _isLastSunday(uint8_t month, int previousSunday) {
		return(previousSunday + 7 > _daysInMonth(month));
	};
	uint8_t _daysInMonth(uint8_t month) { //DST never begins / ends in February so it is not considered
		if (month < 8) {
			return(month % 2 ? 31 : 30);
		}
		else {
			return(month % 2 ? 30 : 31);
		}
	};
};

class ClockTime {
public:
	
	static ClockTime & instance() {
		static  ClockTime pInstance;
		return pInstance;
	};

	bool begin() {
		// initializing the rtc

#ifdef __RTC__
		if (!rtc.begin()) {
			Serial.println("Couldn't find RTC!");
			return false;
		}
		else {

			if (rtc.lostPower()) {
				// this will adjust to the date and time at compilation
				rtc.adjust(DateTime(__DATE__, __TIME__));
			}

			//we don't need the 32K Pin, so disable it
			rtc.disable32K();

			timeNow();

			return true;
		}
#else 
		return true;
#endif
	};

	uint32_t timeNow() {
		static int time = 0;
		if (millis() - time > 50) {
			time = millis();

#ifndef __RTC__
			DateTime st = now();	
#else 
			DateTime st = rtc.now();		
#endif
			_utc = st.unixtime();

			DateTime local = dst.calculateTime(st + (_offset * 3600)); // takes into account DST

			_hour = local.hour();
			_min = local.minute();
			_sec = local.second();
			_mon = local.month();
			_day = local.day();
			_year = local.year();
			_dayWeek = local.dayOfTheWeek();
			_local = local.unixtime();
		}
		return _local;
	};

	void setTimeNow(int8_t hours, int8_t min, int8_t sec, int8_t day,
		int8_t mon, int16_t year) {
		DateTime dt = DateTime(year, mon, day, hours, min, sec);
		if (dt.unixtime() != _utc) {

#ifndef __RTC__
			setTime(hours, min, sec, day, mon, year);
#else 
			rtc.adjust(dt);
#endif
		}
	};

	void setTimeLocal(int8_t hours, int8_t min, int8_t sec, int8_t day,
		int8_t mon, int16_t year) {
		int dsth = 0;
		if (dst.checkDST(DateTime(year, mon, day, hours, min, sec))) {
			dsth = 1;
		}

		setTimeNow(hours - _offset - dsth, min, sec, day, mon, year);
	};

	void setTimeNow(int32_t utc) {
		if (utc != _utc) {

#ifdef __RTC__
			rtc.adjust(DateTime(utc));
#else 
			setTime(utc);
#endif
		}
	}

	void setDst(uint8_t irregularity,
		uint8_t beginDay, uint8_t beginMonth,
		uint8_t endDay, uint8_t endMonth,
		uint8_t beginHour = 2, uint8_t endHour = 3) 
	{
		dst.set(irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour - 1);
	}

	void setTimeZone(uint8_t offset) { _offset = offset; }

	//get time
	int8_t hour(){ return _hour; }; int8_t min() { return _min; }; 
	int8_t sec(){ return _sec; }; int8_t day() { return _day; }; 
	int8_t month(){ return _mon; }; int16_t year(){ return _year; };

	int8_t dayWeek(){ return _dayWeek; };
	float temp(){

#ifdef __RTC__
		return rtc.getTemperature();
#else 
		return 0;
#endif
		
	};
	uint32_t utc() { return _utc; };
	uint32_t local(){ return _local; };
	uint16_t tz() { return _local-_utc; };

	bool updateNTP() {

		//https://github.com/G6EJD/ESP32-Time-Services-and-SETENV-variable/blob/master/README.md
		//https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
		//configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");
		//init and get the time
		configTime(0, 0, "pool.ntp.org");

		struct tm timeinfo;

		if (getLocalTime(&timeinfo)) {

			DateTime dt = DateTime(
				timeinfo.tm_year + 1900,//year, 
				timeinfo.tm_mon + 1, // month
				timeinfo.tm_mday,//day of the month
				timeinfo.tm_hour,// hours
				timeinfo.tm_min,// minutes
				timeinfo.tm_sec// seconds 
			);

			if (abs(dt.unixtime() - _utc) > 2) {
				setTimeNow(dt.unixtime());
				Serial.printf("updateNTP dife = %ds ", (dt.unixtime() - _utc));
				Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
			}
			return true;
		}

		return false;
	};
	bool isDst() { return dst.isDst();};

private:

	ClockTime() {};

	int8_t _hour; int8_t _min;int8_t _sec;
	int8_t _day; int8_t _mon; int16_t _year;
	uint32_t _utc = 0;
	uint32_t _local = 0;
	int8_t _offset = 0;
	int8_t _dayWeek = 0;

	DST dst;

#ifdef __RTC__
	RTC_DS3231 rtc; // clock object
#endif
	
};

#endif
