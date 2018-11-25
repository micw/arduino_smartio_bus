#include <Arduino.h>
#include <smartio_dimmable_led.h>

// Arduino dims from 0...255, ESP8266 from 0..1023
#if defined(ESP8266)
#define PWM_MAX_VALUE 1023
#else
#define PWM_MAX_VALUE 255
#endif

DimmableLed::DimmableLed(unsigned int pin):
		pin(pin), dim_value(1023), old_dim_value(-1), on_off(false) {
	pinMode(pin,OUTPUT);
	update();
}

void DimmableLed::loop() {
	if (dimming) {
		if (millis()-last_dim<3) return; // dim every 3 ms -> 1 dim cycle ~ 3s
		last_dim=millis();
		dim_value+=dim_direction;
		if (dim_value<1) {
			// when dimmed down to minimum, we switch direction an dim up
			dim_value=1;
			dim_direction=1;
		} else if (dim_value>1300) {
			// Valid dim range ends at 1023. We dimm up to 1300 so
			// that there's a time window of ~0.5s at 100% before dimming down
			dim_value=1300;
			dim_direction=-1;
		}
		update();
	}
}

void DimmableLed::toggle() {
	on_off=!on_off;
	update();
}
void DimmableLed::dim_start() {
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

void DimmableLed::dim_stop() {
	dimming=false;
}

int DimmableLed::normalize(int value, int max_value) {
	if (value<0) return 0;
	if (value>max_value) return max_value;
	return value;
}

int DimmableLed::value_to_pwm(int value, int max_value) {

//	return (int)pow(max_value, value/1023.0);
	// https://www.mikrocontroller.net/articles/LED-Fading
	// This formular gives an almost linear impression of the fading
	return (int) (pow(2, log2(max_value) * (value+1)/1024.0) - 1);
}

void DimmableLed::update() {
	if (on_off && dim_value>0) {
		int v=normalize(dim_value,1023);
		if (v!=old_dim_value) {
			// we update the PWM signal only if the actual value changes
			old_dim_value=v;
			int pwm_value=value_to_pwm(v,PWM_MAX_VALUE);
			pwm_value=normalize(pwm_value,PWM_MAX_VALUE);
			analogWrite(pin,pwm_value);
		}
	} else {
		old_dim_value=-1;
		digitalWrite(pin,0);
	}
}
