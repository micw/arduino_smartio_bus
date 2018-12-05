#include <Arduino.h>
#include <smartio_dimmable_cw_led.h>

DimmableCWLed::DimmableCWLed(unsigned int pin1, unsigned int pin2):
		pin1(pin1), pin2(pin2) {
	pinMode(pin1,OUTPUT);
	pinMode(pin2,OUTPUT);
	update();
}

void DimmableCWLed::loop() {
	boolean changed=false;
	if (this->perform_dim()) {
		changed=true;
	}
	if (perform_fade()) {
		changed=true;
	}
	if (changed) {
		update();
	}
}

void DimmableCWLed::fade_start() {
	fading=true;
	if (on_off) {
		fade_direction=-fade_direction;
	}
}

void DimmableCWLed::fade_stop() {
	fading=false;
}


boolean DimmableCWLed::perform_fade() {
	if (fading && on_off) {
		if (millis()-last_fade<3) return false; // fase every 3 ms -> 1 fase cycle ~ 3s
		last_fade=millis();
		fade_value+=fade_direction;
		if (fade_value<1) {
			// when faded top one end, we switch direction
			fade_value=1;
			fade_direction=1;
		} else if (fade_value>1024) {
			fade_value=1024;
			fade_direction=-1;
		}
		return true;
	}
	return false;
}


void DimmableCWLed::dim_to(int dim_value) {
	if (dim_value<0) {
		digitalWrite(pin1,0);
		digitalWrite(pin2,0);
	} else {

		// in cw_value range 1023..511 value2 fades from 0 to max. Below 511 it's always max
		int dim_value1=(int) dim_value*min(1.0,(1023-fade_value)/511.0);
		int pwm_value1=this->value_to_pwm(dim_value1,pwm_max_value);
		pwm_value1=this->normalize(pwm_value1,pwm_max_value);

		// in cw_value range 0..511 value2 fades from 0 to max. Above 511 it's always max
		int dim_value2=(int) dim_value*min(1.0,fade_value/511.0);
		int pwm_value2=this->value_to_pwm(dim_value2,pwm_max_value);
		pwm_value2=this->normalize(pwm_value2,pwm_max_value);

		Serial.print(dim_value1);
		Serial.print(" ");
		Serial.println(dim_value2);

		analogWrite(pin1,pwm_value1);
		analogWrite(pin2,pwm_value2);
	}
}
