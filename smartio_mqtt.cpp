#if defined(ESP8266)

#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <smartio_mqtt.h>

Mqtt::Mqtt(char* wifi_ssid,char* wifi_password,char* mqtt_host, int mqtt_port,
		char* mqtt_username, char* mqtt_password, char* mqtt_alive_topic):
		mqttClient(wifiClient), mqtt_username(mqtt_username), mqtt_password(mqtt_password) {
	WiFi.mode(WIFI_STA);
	WiFi.begin(wifi_ssid,wifi_password);
	mqttClient.setServer(mqtt_host,mqtt_port);
	mqttClient.setCallback([this](char* topic, unsigned char* payload, int length){ this->received(topic,(char*)payload,length); });

	byte mac[6];
	WiFi.macAddress(mac);

	mqtt_client_id=new char[15];
	sprintf(mqtt_client_id, "ESP8266_%02X%02X%02X", mac[3], mac[4], mac[5]);

	if (mqtt_alive_topic) {
		// replace %s in mqtt_keepalive_topic by 6 digits of hex mac address
		char aliveTopicId[7];
		sprintf(aliveTopicId, "%02X%02X%02X", mac[3], mac[4], mac[5]);
		this->mqtt_alive_topic=new char[255];
		sprintf(this->mqtt_alive_topic, mqtt_alive_topic, aliveTopicId);
	}
}


// From: https://github.com/iosphere/mosquitto/blob/master/lib/util_mosq.c#L220
bool Mqtt::topic_matches_subscription(const char *sub, const char *topic) {
	int slen, tlen;
	int spos, tpos;
	bool multilevel_wildcard = false;

	if(!sub || !topic) return false;

	slen = strlen(sub);
	tlen = strlen(topic);

	if(slen && tlen){
		if((sub[0] == '$' && topic[0] != '$')
				|| (topic[0] == '$' && sub[0] != '$')){

			return false;
		}
	}

	spos = 0;
	tpos = 0;

	while(spos < slen && tpos < tlen){
		if(sub[spos] == topic[tpos]){
			if(tpos == tlen-1){
				/* Check for e.g. foo matching foo/# */
				if(spos == slen-3 
						&& sub[spos+1] == '/'
						&& sub[spos+2] == '#'){
					return true;
				}
			}
			spos++;
			tpos++;
			if(spos == slen && tpos == tlen){
				return true;
			}else if(tpos == tlen && spos == slen-1 && sub[spos] == '+'){
				spos++;
				return true;
			}
		}else{
			if(sub[spos] == '+'){
				spos++;
				while(tpos < tlen && topic[tpos] != '/'){
					tpos++;
				}
				if(tpos == tlen && spos == slen){
					return true;
				}
			}else if(sub[spos] == '#'){
				multilevel_wildcard = true;
				if(spos+1 != slen){
					return false;
				}else{
					return true;
				}
			}else{
				return false;
			}
		}
	}
	if(multilevel_wildcard == false && (tpos < tlen || spos < slen)){
		return false;
	}

	return true;
}


void Mqtt::watch(char* topic, MqttWatchCallback callback) {
	watching.push_back(MqttWatch {
		topic,
		callback,
		0,
		0
	});
}

void Mqtt::on(char* topic, MqttReceiveCallback callback) {
	receiving.push_back(MqttReceive {
		topic,
		callback,
	});
	mqttClient.subscribe(topic);
}



void Mqtt::connect(char* topic, DimmableCWLed& led) {
	connect(new MqttDimmableCWLedAdapter(this,topic,led));
}

void Mqtt::connect(MqttConsumer* consumer) {
	consumers.push_back(consumer);
	mqttClient.subscribe(consumer->getTopicSubscription());
}

void Mqtt::received(char* topic, char* payload, unsigned int length) {
	payload[length]=0;
	for (int i=0;i<receiving.size();i++) {
		MqttReceive* receive=&receiving[i];
		if (strcmp(topic,receive->topic)==0) {
			int value=atoi(payload);
			receive->callback(value);
		}
	}
	for (int i=0;i<consumers.size();i++) {
		MqttConsumer* consumer=consumers[i];
		if (topic_matches_subscription(consumer->getTopicSubscription(),topic)) {
			consumer->consume(topic,payload);
		}
	}
}

void Mqtt::loop() {
	int newWifiStatus=WiFi.status();
	if (wifiStatus!=newWifiStatus) {
		wifiStatus=newWifiStatus;
		switch (wifiStatus) {
			case WL_CONNECTED:
				Serial.print("Wifi connected with IP ");
				Serial.println(WiFi.localIP());
				break;
			case WL_DISCONNECTED:
				Serial.println("Wifi disconnected, waiting for connection ...");
				break;
			default:
				Serial.printf("Unknown wifi status: %i",wifiStatus);
		}
	}

	if (wifiStatus == WL_CONNECTED && !mqttClient.connected() && mqtt_next_connect_retry<millis()) {
		Serial.println("MQTT disconnected, trying to connect ...");
		if (mqtt_alive_topic) {
			if (mqttClient.connect(mqtt_client_id, mqtt_username, mqtt_password, mqtt_alive_topic, 1, 1, MQTT_FALSE)) {
				Serial.printf("MQTT connected. Setting alive topic '%s' to 1\n",mqtt_alive_topic);
				mqttClient.publish(mqtt_alive_topic,MQTT_TRUE,1);
			}
		} else {
			if (mqttClient.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
				Serial.println("MQTT connected.");
			}
		}
		if (mqttClient.connected()) {
			// re-create subscriptions
			mqttClient.subscribe("#");
			for (int i=0;i<receiving.size();i++) {
				mqttClient.subscribe(receiving[i].topic);
			}
			for (int i=0;i<consumers.size();i++) {
				MqttConsumer* consumer=consumers[i];
				Serial.print("Subscribing: ");
				Serial.println(consumer->getTopicSubscription());
				mqttClient.subscribe(consumer->getTopicSubscription());
			}

		} else {
			Serial.printf("MQTT connection failed with state %i - retrying in 5s ...\n",mqttClient.state());
			mqtt_next_connect_retry=millis()+5000;
		}
	}
	if (mqttClient.connected()) {
		mqttClient.loop();
		for (int i=0;i<consumers.size();i++) {
			consumers[i]->loop();
		}

		for (int i=0;i<watching.size();i++) {
			MqttWatch* watch=& watching[i];
			if (millis()-watch->last_check<100) continue; // re-check every 100ms
			float new_value=watch->callback();
			if (watch->last_check==0 || new_value!=watch->old_value || millis()-watch->last_send>30000) { // send initially, on change and every 30s
				String s(new_value);
				char payload[s.length()+1];
				String(new_value).toCharArray(payload,s.length()+1);
				mqttClient.publish(watch->topic,payload);
				watch->old_value=new_value;
				watch->last_send=millis();
			}
			watch->last_check=millis(); // re-check every 100ms
		}

	}
}

int Mqtt::publish(char* topic, char* payload) {
	return mqttClient.publish(topic,payload);
}

void MqttAdapterBase::consume(char* topic, char* payload) {
	if (strlen(topic)>topicPrefixLength) {
		char* property=topic+topicPrefixLength;
		set_value(property,payload);
		publish_value(property);
	}
}

void MqttAdapterBase::watch(const char* property) {
	properties.push_back({
		property,0,0,0
	});
}
void MqttAdapterBase::loop() {
	char value_buffer[255];
	for (int i=0;i<properties.size();i++) {
		MqttWatchedProperty* prop=& properties[i];

		if (millis()-prop->last_check<100) continue; // re-check every 100ms
		char* value=get_value(prop->property,value_buffer);

		unsigned long value_hash=hash(value);

		if (prop->last_check==0 || value_hash!=prop->last_value_hash || millis()-prop->last_send>30000) { // send initially, on change and every 30s
			if (publish_value(prop->property,value)) {
				prop->last_value_hash=value_hash;
				prop->last_send=millis();
			}
		}
		prop->last_check=millis();
	}
}

int MqttAdapterBase::publish_value(const char* property) {
	char value_buffer[255];
	char* value=get_value(property,value_buffer);
	return publish_value(property,value);
}

int MqttAdapterBase::publish_value(const char* property, char* value) {
	if (!value) {
		return 1; // no value
	}
	int len=strlen(value);
	if (!len) {
		return 1; // empty value
	}
	char topic_buffer[255];
	strcpy(topic_buffer,topicBase);
	strcat(topic_buffer,"get/");
	strcat(topic_buffer,property);
	return mqtt->publish(topic_buffer,value);
}

// https://stackoverflow.com/questions/7666509/hash-function-for-string
unsigned long MqttAdapterBase::hash(char *str)
{
	if (!str)
		return 0;

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


char* MqttAdapterBase::append(char* topic, char append, bool onlyIfMissing) {
	int len=strlen(topic);
	if (onlyIfMissing && topic[len-1]==append) {
		return topic;
	}
	char* newTopic=new char[len+2];
	strcpy(newTopic,topic);
	newTopic[len++]=append;
	newTopic[len]=0;
	return newTopic;
}
char* MqttAdapterBase::append(char* topic, char* append) {
	char* newTopic=new char[strlen(topic)+strlen(append)+1];
	strcpy(newTopic,topic);
	strcat(newTopic,append);
	return newTopic;
}
bool MqttAdapterBase::convert_on_off(char* payload) {
	return strcmp("ON",payload)==0;
}
char* MqttAdapterBase::convert_on_off(bool value, char* buffer) {
	if (value) {
		return "ON";
	}
	return "OFF";
}
int MqttAdapterBase::convert_int(char* payload) {
	return atoi(payload);
}
char* MqttAdapterBase::convert_int(int value, char* buffer) {
	return itoa(value,buffer,10);
}

// http://wiki.secondlife.com/wiki/Interpolation/Rescale/Float
float MqttAdapterBase::rescale(float value, float value_min, float value_max, float result_min, float result_max) {
	return result_min + ((result_max-result_min) * ((value_min-value) / (value_min-value_max)));
};

MqttDimmableCWLedAdapter::MqttDimmableCWLedAdapter(Mqtt* mqtt, char* topicBase, DimmableCWLed& led):
		MqttAdapterBase(mqtt,topicBase),led(led) {
	watch("state");
	watch("dim");
	watch("fade");
};

void MqttDimmableCWLedAdapter::set_value(const char* property, char* payload) {
	if (strcmp("state",property)==0) {
		return led.set_state(convert_on_off(payload));
	}
	if (strcmp("dim",property)==0) {
		return led.set_dim_value(convert_int(payload));
	}
	if (strcmp("fade",property)==0) {
		return led.set_fade_value(rescale(convert_int(payload),153,500,0,1023));
	}
};
char* MqttDimmableCWLedAdapter::get_value(const char* property, char* buffer) {
	if (strcmp("state",property)==0) {
		return convert_on_off(led.get_state(),buffer);
	}
	if (strcmp("dim",property)==0) {
		return convert_int(led.get_dim_value(),buffer);
	}
	if (strcmp("fade",property)==0) {
		return convert_int(rescale(led.get_fade_value(),0,1023,153,500),buffer);
	}
	return 0;
};

#endif