#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "config.h"

// global declarations
DHT dht(DHTPIN, DHTTYPE);
const size_t bufferSize = JSON_OBJECT_SIZE(2);
DynamicJsonBuffer jsonBuffer(bufferSize);
JsonObject& root = jsonBuffer.createObject();
HTTPClient http;

// function to init Serial
void initSerial() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
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

// functoin to create the firebase payload
JsonObject& createPayload(float temp, float hmdt) {
  root["temperature"] = temp;
  root["humidity"] = hmdt;
  root.prettyPrintTo(Serial);
  return root;
}



// function to send the data to firebase
void sendSensorData(float temp, float hmdt) {
  JsonObject& data = createPayload(temp, hmdt);

  String postData = "";
  data.printTo(postData);
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.POST(postData);
  printMessage(postData);  
  printMessage(http.getString());
  http.end();
}

void printMessage(String msg) {
  Serial.println(msg);
}

void showError(String msg) {
  Serial.println(msg);
}

int createDelayInSeconds(int seconds) {
  return ( seconds * 1000 );
}

void setup(){
  initSerial();
  wifiConnect();
  initSensorLib();
}

void loop()
{
  float hmdt = dht.readHumidity();
  float temp = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temp) || isnan(hmdt)) {
    showError("Failed to read from DHT");
  } else {
    sendSensorData(temp, hmdt);
  }
  delay(createDelayInSeconds(900));
}
