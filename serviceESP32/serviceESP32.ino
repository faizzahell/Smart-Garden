#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <DHT.h>

const char* ssid = "IoTMikrokontroler";
const char* password = "asdfghjkl";
const char* serverAddress = "192.168.146.176";
const int serverPort = 3000;

#define DHT_PIN 33
#define DHT_TYPE DHT22 
DHT dht(DHT_PIN, DHT_TYPE);

#define LDR_PIN 19
#define LED_PIN 5
#define SOIL_PIN 32
#define RELLAY_PIN 18

#define RXp2 RX
#define TXp2 TX
 
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

      if (payload[1], "1") {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();
        // float soilHumidity = (100 - ((analogRead(SOIL_PIN)/4095.00) * 100));
        float soilHumidity = 55;

        int lightState = digitalRead(LDR_PIN);
        String status = "Undefined";
        String statusPamp = "Undefined";

        char tempStr[8]; 
        char humStr[8];  
        char soilHumStr[8];
        
        dtostrf(temperature, 6, 2, tempStr);
        dtostrf(humidity, 6, 2, humStr);
        dtostrf(soilHumidity, 6, 2, soilHumStr);

        Serial.print("Suhu: ");
        Serial.println(tempStr);
        Serial.print("Kelembaban: ");
        Serial.println(humStr);
        Serial.print("Kelembaban Soil: ");
        Serial.println(soilHumStr);

        if (payload[2] == '0') {
           if (lightState == HIGH) {
             Serial.println("It is dark");
             digitalWrite(LED_PIN, HIGH);
             status = "Gelap";
           } else {
             Serial.println("It is light");
             digitalWrite(LED_PIN, LOW);
             status = "Terang";
           }

           if (soilHumidity == 55) {
            Serial2.print(1);
            Serial.print('Berhasil mengirim data ke Arduino UNO : ');
            Serial.println(1);
            statusPamp = "Menyala";
           } else {
            Serial2.print(0);
            Serial.print('Berhasil mengirim data ke Arduino UNO : ');
            Serial.println(0);
            statusPamp = "Padam";
           }
        } else {
          if (payload[4] == '1') {
            Serial.println("It is dark");
            digitalWrite(LED_PIN, HIGH);
            status = "Gelap";
          } 
          
          if (payload[4] == '0') { 
            Serial.println("It is light");
            digitalWrite(LED_PIN, LOW);
            status = "Terang";
          }

          if (payload[6] == '1') {
            Serial2.print(1);
            Serial.print('Berhasil mengirim data ke Arduino UNO : ');
            Serial.println(1);
            statusPamp = "Menyala";
          }

          if (payload[6] == '0') {
            Serial2.print(0);
            Serial.print('Berhasil mengirim data ke Arduino UNO : ');
            Serial.println(0);
            statusPamp = "Padam";
          }
        }

        StaticJsonDocument<200> doc;

        doc["suhu"] = String(tempStr);
        doc["kelembaban"] = String(humStr);
        doc["status"] = status;
        doc["kelembaban_soil"] = String(soilHumidity);
        doc["status_pompa"] = String(statusPamp);

        String jsonStr;

        serializeJson(doc, jsonStr);

        webSocket.sendTXT(jsonStr);
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
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
  pinMode(RELLAY_PIN, OUTPUT);
}

void loop() {
  webSocket.loop();
  webSocket.onEvent(onWebSocketEvent);
}
