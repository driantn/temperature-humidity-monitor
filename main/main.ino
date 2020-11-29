#include "PubSubClient.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "config.h"


DHT dht(DHTPIN, DHTTYPE);
const size_t bufferSize = JSON_OBJECT_SIZE(2);
DynamicJsonBuffer jsonBuffer(bufferSize);
JsonObject& root = jsonBuffer.createObject();
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// functoin to create the firebase payload
JsonObject& createPayload(float temp, float hmdt) {
  root["temperature"] =  temp;
  root["humidity"] =  hmdt;
  root.prettyPrintTo(Serial);
  return root;
}

void initSerial() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) { }
}

// function to init wifi and try to connent
void wifiConnect() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

// function to initialise the DHT sensor library
void initSensorLib() {
  dht.begin();
}

// init MQTT client
void initMQTT() {
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {}

void printMessage(String msg) {
  Serial.println(msg);
}

void showError(String msg) {
  Serial.println(msg);
}

int createDelayInSeconds(int seconds) {
  return ( seconds * 1000 );
}

// function to send the data to firebase
void sendSensorData(float temp, float hmdt) {
  JsonObject& currentData = createPayload(temp, hmdt);

  // send data to mqtt broker
  char data[200];
  root.printTo(data, currentData.measureLength() + 1);
  client.publish(MQTT_SENSOR_TOPIC, data, true);
}

void reconnectMQTTClient() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  initSerial();
  wifiConnect();
  initSensorLib();
  initMQTT();
}

void loop() {
  if (!client.connected()) {
    reconnectMQTTClient();
  }
  client.loop();

  float hmdt = dht.readHumidity();
  float temp = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temp) || isnan(hmdt)) {
    showError("Failed to read from DHT");
  } else {
    sendSensorData(temp, hmdt);
    client.disconnect();
  }
  delay(createDelayInSeconds(900));
}
