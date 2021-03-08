// function template
#include "Model.h"

Model * model = Model::instance();

void setup() {

	Serial.begin(115200);

	delay(1000);
	
	model->begin();
}

void loop() {
	model->update();
}