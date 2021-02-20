// function template

#include "Model.h"

Model * model = Model::instance();

void setup() {

	Serial.begin(115200);
	Serial.println("Starting"); // will be shown in the terminal
	Serial.setDebugOutput(true);

	delay(1000);

	model->begin();
}

void loop() {
	model->update();
}