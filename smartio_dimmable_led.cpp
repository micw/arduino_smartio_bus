#include <Arduino.h>
#include <smartio_dimmable_led.h>

void DimmableLedBase::loop() {
	if (perform_dim()) {
		update();
	}
}

boolean DimmableLedBase::perform_dim() {
	if (dimming) {
		if (millis()-last_dim<3) return false; // dim every 3 ms -> 1 dim cycle ~ 3s
		last_dim=millis();
		dim_value+=dim_direction;
		if (dim_value<1) {
			// when dimmed down to minimum, we switch direction and dim up
			dim_value=1;
			dim_direction=1;
		} else if (dim_value>1300) {
			// Valid dim range ends at 1023. We dimm up to 1300 so
			// that there's a time window of ~0.5s at 100% before dimming down
			dim_value=1300;
			dim_direction=-1;
		}
		return true;
	}
	return false;
}

void DimmableLedBase::toggle() {
	on_off=!on_off;
	update();
}
void DimmableLedBase::dim_start() {
	dimming=true;
	if (!on_off) { // off -> dim up
		dim_direction=1;
		dim_value=0;
		on_off=true;
	} else { // continue dim in other direction
		dim_direction=-dim_direction;
		// dim_value might be >1023 when we was dimming up
		dim_value=normalize(dim_value,1023);
	}
}

void DimmableLedBase::dim_stop() {
	dimming=false;
}

int DimmableLedBase::get_dim_value() {
	return dim_value;
}
void DimmableLedBase::set_dim_value(int dim_value) {
	if (dim_value<=0) this->dim_value=0;
	else if (dim_value>1023) this->dim_value=1023;
	else this->dim_value = dim_value;
	update();
}
bool DimmableLedBase::get_state() {
	return on_off;
}
void DimmableLedBase::set_state(bool state) {
	on_off=state;
	update();
}

int DimmableLedBase::normalize(int value, int max_value) {
	if (value<0) return 0;
	if (value>max_value) return max_value;
	return value;
}

int DimmableLedBase::value_to_pwm(int value, int max_value) {

//	return (int)pow(max_value, value/1023.0);
	// https://www.mikrocontroller.net/articles/LED-Fading
	// This formular gives an almost linear impression of the fading
	return (int) (pow(2, log2(max_value) * (value+1)/1024.0) - 1);
}

void DimmableLedBase::update() {
	if (on_off && dim_value>0) {
		int v=normalize(dim_value,1023);
		dim_to(v);
	} else {
		dim_to(-1);
	}
}


DimmableLed::DimmableLed(unsigned int pin):
		pin(pin) {
	pinMode(pin,OUTPUT);
	update();
}

void DimmableLed::dim_to(int dim_value) {
	if (dim_value<0) {
		digitalWrite(pin,0);
	} else {
		int pwm_value=value_to_pwm(dim_value,pwm_max_value);
		pwm_value=normalize(pwm_value,pwm_max_value);
		analogWrite(pin,pwm_value);
	}
}
