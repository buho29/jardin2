// observer.h

#ifndef _OBSERVER_h
#define _OBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MODEL_MAX_LISTENERS 50

enum Status {
	starting, started, standby, watering
};

/*			Model			*/

enum EventType {
	all,
	error,
	conectedWifi,
	loadedForescast,
	loadedTime,
	sensor,
	sensorLog,
	status,
	zoneChanged,
	modesChanged
};

//clase virtual para registrase a eventos en model
class EventChange
{
public:
	virtual void onChange(EventType ev) = 0;
};

class Observer {
protected:
	EventChange * listeners[MODEL_MAX_LISTENERS];
	EventType types[MODEL_MAX_LISTENERS];
	uint8_t listenerCount = 0;
	//event
	void dispachEvent(EventType ev) {
		for (uint8_t i = 0; i < listenerCount; i++)
		{
			EventChange * e = listeners[i];
			EventType t = types[i];

			if (t == ev || t == EventType::all)
			{
				e->onChange(ev);
			}
		}
	};
public:
	void addEvent(EventType type, EventChange * callBack) {
		if (listenerCount < MODEL_MAX_LISTENERS) {
			listeners[listenerCount] = callBack;
			types[listenerCount++] = type;
		}
	};
};
#endif

