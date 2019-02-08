#ifndef Included_SmartIO_MQTT_H
#define Included_SmartIO_MQTT_H

#include <ESP8266WiFi.h>

#define MQTT_KEEPALIVE 10
#include <PubSubClient.h>

#include <smartio_main_loop.h>
#include <smartio_dimmable_cw_led.h>
#include <vector.h>

#define _WIFI_SSID WIFI_SSID


typedef int (*MqttWatchCallback) ();
struct MqttWatch {
	char* topic;
	MqttWatchCallback callback;
	float old_value;
	unsigned long last_check;
	unsigned long last_send;
};
typedef void (*MqttReceiveCallback) (float value);
struct MqttReceive {
	char* topic;
	MqttReceiveCallback callback;
};

/*
struct MqttProperty {
	char* getTopic;
	MqttWatchCallback getter;

	char* setTopic;
	MqttReceiveCallback setter;

	float old_value;
	unsigned long last_check;
	unsigned long last_send;
};
*/

class MqttConsumer;
class MqttDimmableCWLedAdapter;

class Mqtt : MainLoopListener {
	public:
		Mqtt(char* wifi_ssid,char* wifi_password,char* mqtt_host, int mqtt_port,
			char* mqtt_username, char* mqtt_password, char* mqtt_keepalive_topic);

		int publish(char* topic, char* payload);

		void watch(char* topic, MqttWatchCallback callback);
		void on(char* topic, MqttReceiveCallback callback);
		void connect(MqttConsumer* consumer);
		void connect(char* topic, DimmableCWLed& led);

		const char* MQTT_FALSE = "0";
		const char* MQTT_TRUE = "1";

		static bool topic_matches_subscription(const char* subscription, const char* topic);

	private:
		WiFiClient wifiClient;
		PubSubClient mqttClient;

		int wifiStatus=-1;

		char* mqtt_client_id;
		char* mqtt_username;
		char* mqtt_password;
		char* mqtt_alive_topic;

		unsigned long mqtt_next_connect_retry=0;

		Vector<MqttWatch> watching;
		Vector<MqttReceive> receiving;
		Vector<MqttConsumer*> consumers;

		void loop();

		void received(char* topic, char* payload, unsigned int length);
};

class MqttConsumer {
	public:
		virtual char* getTopicSubscription()=0;
		virtual void consume(char* topic, char* payload)=0;
		virtual void loop() {};
};

struct MqttWatchedProperty {
	const char* property;
	unsigned long last_value_hash;
	unsigned long last_check;
	unsigned long last_send;
};

class MqttAdapterBase : public MqttConsumer {
	public:
		MqttAdapterBase(Mqtt* mqtt, char* topicBase):
				mqtt(mqtt),
				topicBase(append(topicBase,'/', true)),
				topicSubscription(append(this->topicBase,"set/#")),
				topicPrefixLength(strlen(this->topicBase)+4) {
		}
		char* getTopicSubscription() {
			return topicSubscription;
		}
		void consume(char* topic, char* payload);

	private:
		int publish_value(const char* property);
		int publish_value(const char* property, char* payload);

		static char* append(char* topic, char append, bool onlyIfMissing);
		static char* append(char* topic, char* append);
		static unsigned long hash(char *str);

		virtual void set_value(const char* property, char* payload)=0;
		virtual char* get_value(const char* property, char* buffer)=0;

		Mqtt* mqtt;
		char* topicBase;
		char* topicSubscription;
		unsigned int topicPrefixLength;
		Vector<MqttWatchedProperty> properties;

		void loop();

	protected:
		void watch(const char* property);

		static bool convert_on_off(char* payload);
		static char* convert_on_off(bool value, char* buffer);

		static int convert_int(char* payload);
		static char* convert_int(int value, char* buffer);

		static float rescale(float value, float value_min, float value_max, float result_min, float result_max);
};

class MqttDimmableCWLedAdapter : public MqttAdapterBase {
	public:
		MqttDimmableCWLedAdapter(Mqtt* mqtt, char* topicBase, DimmableCWLed& led);

	private:
		DimmableCWLed& led;

		void set_value(const char* property, char* payload);
		char* get_value(const char* property, char* buffer);
};

#endif
