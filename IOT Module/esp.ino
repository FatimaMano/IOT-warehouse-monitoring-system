#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid     = "V2043";
const char* password = "1234567890"; // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "Broker.hivemq.com";
const char *topic1 = "sensor1"; //temperatureSensor 
const char *topic2 = "sensor2"; //FireSensor
const char *topic3 = "Buzzer";
const char *topic4 = "BuzzerOFF";
const char *outtopic5 = "TemperatureSensor"; //send temperature value
const char *outtopic6 = "FireTemperatureSensor"; //send firesensorvalue
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(ConnectedToAP_Handler, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(GotIP_Handler, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFi_Disconnected_Handler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi /Network ..");
  //connecting to an MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  char TemperatureValue = 150;
  char FlameSensor = 250;
  String TemperatureValueStr = String(TemperatureValue);
  String FlameSensorStr = String(FlameSensor);

  client.subscribe(topic1);
  client.subscribe(topic2);
  client.subscribe(topic3);
  client.subscribe(topic4);
  client.publish(outtopic5, TemperatureValueStr.c_str());
  client.publish(outtopic6, FlameSensorStr.c_str());

}

void callback(char *topic, byte *payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.write((char)payload[i]);
  }
    Serial1.write(payload, length); 
  Serial.println();
}

void loop() {
  client.loop();
  delay(2000);
}

void ConnectedToAP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info)  {
  Serial.println("Connected To The WiFi Network");
}

void GotIP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void WiFi_Disconnected_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Disconnected From WiFi Network");
  // Attempt Re-Connection
  WiFi.begin(ssid, password);
}
