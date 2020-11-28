#define SSID "SSID"
#define PASSWORD "Password"

// Definitions
#define DHTPIN D2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define MQTT_VERSION MQTT_VERSION_3_1_1

// MQTT: ID, server IP, port, username and password
#define MQTT_CLIENT_ID "local-sensor"
#define MQTT_SERVER_IP "192.168.0.15"
#define MQTT_SERVER_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASSWORD "password"
#define MQTT_SENSOR_TOPIC "home/sensor/data"

//Firebae config
#define FIREBASE_HOST "someHost.firebaseio.com"
#define FIREBASE_AUTH "2kjhdsh"
