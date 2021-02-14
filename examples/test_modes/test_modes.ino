
#include "Model.h";

Model * model = Model::instance();
Modes & modes = model->modes;


void setup() {

	Serial.begin(115200);  
	Serial.setDebugOutput(true);
	delay(1000);

	
	Serial.println("\n-----------------------");

	fillWeather(10,30,95,75,3,3);
	
	fillSensors(6,990,45);

	modes.begin();

	Flags f = Flags(Modes::weatherF | Modes::sensorsF);

	modes.setFlags(Modes::weatherF | Modes::sensorsF);
	
	Flags fr = Flags(modes.getFlags());
		
	Serial.printf("same %s %d %d\n", ((f == fr) ? "true" : "false"), f.get() , fr.get());

	Serial.println((modes.skip()?" skip true":"skip false"));
	Serial.printf("final evaluate %d\n",modes.evaluate());

}

const char * logSensorJson = "[{\"te\":15.31,\"pr\":961.84,\"hu\":65,\"ti\":1612713594},{\"te\":15.53,\"pr\":961.36,\"hu\":62,\"ti\":1612715394},{\"te\":17.63,\"pr\":961.25,\"hu\":63,\"ti\":1612717194},{\"te\":18.28,\"pr\":960.52,\"hu\":63,\"ti\":1612718994},{\"te\":16.77,\"pr\":960.14,\"hu\":67,\"ti\":1612720795},{\"te\":15.92,\"pr\":959.66,\"hu\":63,\"ti\":1612722595},{\"te\":15.29,\"pr\":959.509,\"hu\":64,\"ti\":1612724395},{\"te\":15.7,\"pr\":959.07,\"hu\":66,\"ti\":1612726195},{\"te\":16.12,\"pr\":958.6,\"hu\":67,\"ti\":1612727995},{\"te\":15.82,\"pr\":958.1,\"hu\":68,\"ti\":1612729795},{\"te\":14.91,\"pr\":957.73,\"hu\":70,\"ti\":1612731595},{\"te\":15.79,\"pr\":957.47,\"hu\":70,\"ti\":1612733395},{\"te\":15.84,\"pr\":956.57,\"hu\":71,\"ti\":1612735195},{\"te\":15.35,\"pr\":956.07,\"hu\":73,\"ti\":1612736995},{\"te\":15.52,\"pr\":956.32,\"hu\":73,\"ti\":1612738795},{\"te\":15.27,\"pr\":955.56,\"hu\":74,\"ti\":1612740595},{\"te\":15.51,\"pr\":955.15,\"hu\":73,\"ti\":1612742395},{\"te\":15.45,\"pr\":955.46,\"hu\":74,\"ti\":1612744195},{\"te\":16.07,\"pr\":955.43,\"hu\":74,\"ti\":1612745995},{\"te\":15.7,\"pr\":955.27,\"hu\":74,\"ti\":1612747795},{\"te\":14.92,\"pr\":955.05,\"hu\":76,\"ti\":1612749595},{\"te\":14.37,\"pr\":955.21,\"hu\":77,\"ti\":1612751395},{\"te\":13.96,\"pr\":955.32,\"hu\":78,\"ti\":1612753195},{\"te\":13.67,\"pr\":955.24,\"hu\":81,\"ti\":1612754996},{\"te\":13.44,\"pr\":955.09,\"hu\":81,\"ti\":1612756796},{\"te\":13.26,\"pr\":954.49,\"hu\":82,\"ti\":1612758596},{\"te\":13.06,\"pr\":954.259,\"hu\":82,\"ti\":1612760396},{\"te\":12.86,\"pr\":954.34,\"hu\":84,\"ti\":1612762196},{\"te\":12.73,\"pr\":954.58,\"hu\":84,\"ti\":1612763996},{\"te\":12.6,\"pr\":954.62,\"hu\":84,\"ti\":1612765796},{\"te\":12.47,\"pr\":954.5,\"hu\":83,\"ti\":1612767596},{\"te\":12.37,\"pr\":954.89,\"hu\":85,\"ti\":1612769396},{\"te\":12.23,\"pr\":954.62,\"hu\":86,\"ti\":1612771196},{\"te\":12.13,\"pr\":954.84,\"hu\":87,\"ti\":1612772996},{\"te\":12.03,\"pr\":955.53,\"hu\":86,\"ti\":1612774796},{\"te\":11.87,\"pr\":955.84,\"hu\":83,\"ti\":1612776596},{\"te\":11.98,\"pr\":955.91,\"hu\":84,\"ti\":1612778396},{\"te\":12.71,\"pr\":955.84,\"hu\":83,\"ti\":1612780196},{\"te\":14.15,\"pr\":956.11,\"hu\":79,\"ti\":1612781996},{\"te\":15.13,\"pr\":955.85,\"hu\":77,\"ti\":1612783796},{\"te\":15.81,\"pr\":956.14,\"hu\":76,\"ti\":1612785596},{\"te\":16,\"pr\":956.27,\"hu\":74,\"ti\":1612787396},{\"te\":15.83,\"pr\":956.48,\"hu\":76,\"ti\":1612789197},{\"te\":15.09,\"pr\":956.45,\"hu\":74,\"ti\":1612790997},{\"te\":14.62,\"pr\":958.71,\"hu\":74,\"ti\":1612816181},{\"te\":14,\"pr\":952.89,\"hu\":77,\"ti\":1612880998},{\"te\":16.71,\"pr\":953.36,\"hu\":63,\"ti\":1612895397},{\"te\":15.43,\"pr\":953.68,\"hu\":65,\"ti\":1612897193}]";

void fillSensors(int temp,int press,int hum) {
	SensorsItem * sens = &model->currentSensor;
	sens->humidity = hum;
	sens->temperature = temp;
	sens->pressure = press;

	auto & sensors = model->getSensorsLog();

	sensors.deserializeData(String(logSensorJson));
}

void fillWeather(int tempMin,int tempMax,int cloudCover,int precipi,
	int hours , int win) {
	model->loadedForescast = true;

	WeatherData * wea = &model->weather;
	wea->temp[0] = tempMin;
	wea->temp[1] = 30;

	WeatherData::Forecast * fore = model->weather.getCurrent();
	fore->cloudCover = cloudCover;
	fore->precipitationProbability = precipi;
	fore->hoursOfPrecipitation = hours;
	fore->win[0] = win;
}

void loop() {}