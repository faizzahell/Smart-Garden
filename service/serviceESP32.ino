#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <DHT.h>

const char* ssid = "BS DORMITORY";
const char* password = "78787878";
const char* serverAddress = "192.168.100.32";
const int serverPort = 3000;

#define DHT_PIN 2 
#define DHT_TYPE DHT22 
DHT dht(DHT_PIN, DHT_TYPE);

#define LDR_PIN 5
#define LED_PIN 18
 
WiFiClient wifiClient;
WebSocketsClient webSocket;

void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Terputus dari server");
      webSocket.begin(serverAddress, serverPort);
      Serial.println("Terhubung ke server WebSocket");
      Serial.println(WiFi.localIP());
      break;
    case WStype_TEXT:
      Serial.print("Menerima pesan: ");
      Serial.println((char*)payload);

      if (strcmp((char*)payload, "1") == 0) {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        int lightState = digitalRead(LDR_PIN);
        String status = "Undefined";

        char tempStr[8]; 
        char humStr[8];  
        dtostrf(temperature, 6, 2, tempStr);
        dtostrf(humidity, 6, 2, humStr);

        Serial.print("Suhu: ");
        Serial.println(tempStr);
        Serial.print("Kelembaban: ");
        Serial.println(humStr);

        if (lightState == HIGH) {
          Serial.println("It is dark");
          digitalWrite(LED_PIN, HIGH);
          status = "Gelap";
        } else {
          Serial.println("It is light");
          digitalWrite(LED_PIN, LOW);
          status = "Terang";
        }

        StaticJsonDocument<200> doc;

        doc["suhu"] = String(tempStr);
        doc["kelembaban"] = String(humStr);
        doc["status"] = status;

        String jsonStr;

        serializeJson(doc, jsonStr);

        webSocket.sendTXT(jsonStr);
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  webSocket.begin(serverAddress, serverPort);
  webSocket.onEvent(onWebSocketEvent);

  Serial.println("Terhubung ke server WebSocket");

  dht.begin();

  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  webSocket.loop();
  webSocket.onEvent(onWebSocketEvent);
}
