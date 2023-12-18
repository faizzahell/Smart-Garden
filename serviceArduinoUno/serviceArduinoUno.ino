#define RXp2 0
#define TXp2 1

#define rellay 2

String incomingByte;

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  pinMode(rellay, OUTPUT);
}

void loop() {
  if (Serial2.available()) {
    incomingByte = Serial2.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(incomingByte);

    if (incomingByte == incomingByte) {
      digitalWrite(rellay, LOW);
    } else {
      Serial.print("Lapor Tidak Menyala");
    }

    if (incomingByte == '0') {
      digitalWrite(rellay, HIGH);
    } else {
      Serial.print("Lapor Pompa Menyala");
    }
    
    delay(1000);
  } else {
    Serial.println("Nothing in the buffer...");
    delay(5000);
  }
}
