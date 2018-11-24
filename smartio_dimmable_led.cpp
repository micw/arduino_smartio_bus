#include <Arduino.h>
#include <smartio_dimmable_led.h>

DimmableLed::DimmableLed(unsigned int pin):
		pin(pin), pwm_value(1023) {
	pinMode(pin,OUTPUT);
	update();
}

void DimmableLed::loop() {
			if (dimming) {
		if (millis()-last_dim<3) return; // dim every 3 ms -> 1 dim cycle ~ 3s
		last_dim=millis();
		pwm_value+=dim_direction;
		if (pwm_value<1) { // switch to dim_up
			dim_direction=1;
			pwm_value=1;
		} else if (pwm_value>1300) { // max 1300 -> remain ~0.5s at 100%
			dim_direction=-1;
			pwm_value=1300;
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
		pwm_value=0;
		on_off=true;
			} else { // continue dim in other direction
		dim_direction=-dim_direction;
		if (pwm_value<1) pwm_value=1;
		else if (pwm_value>1023) pwm_value=1023;
			}
		}

void DimmableLed::dim_stop() {
			dimming=false;
		}
		
void DimmableLed::update() {
			Serial.print(dim_direction);
			Serial.print(" ");
			Serial.println(pwm_value);
			if (on_off && pwm_value>0) {
		int v=pwm_value;
		if (v<0) v=0;
		else if (v>1023) v=1023;
		analogWrite(pin,(int)pow(1023, v/1023.0));
			} else {
		digitalWrite(pin,0);
			}
		}
