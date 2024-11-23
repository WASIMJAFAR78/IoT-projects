#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "WASIM";       
const char* password = "123456789"; 
const char* mqtt_server = " 192.168.50.115"; 
const int mqtt_port = 1883;               
const char* mqtt_topic = "gyro/data";     
const int MPU_ADDR = 0x68; 
WiFiClient espClient;
PubSubClient client(espClient);
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}
void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("NodeMCU_Client")) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
void setupMPU6050() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0x00); 
  Wire.endTransmission();
}
void setup() {
   Serial.begin(115200);
  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  setupMPU6050();
}
void loop() {
    if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();
  int16_t gyroX, gyroY, gyroZ;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); 
  gyroX = (Wire.read() << 8 | Wire.read());
  gyroY = (Wire.read() << 8 | Wire.read());
  gyroZ = (Wire.read() << 8 | Wire.read());
  String gyroData = "X: " + String(gyroX) + " Y: " + String(gyroY) + " Z: " + String(gyroZ);  
  client.publish(mqtt_topic, gyroData.c_str());
  Serial.println("Published: " + gyroData);
  delay(1000); 
}
