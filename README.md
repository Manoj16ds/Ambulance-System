#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D2      // RFID SDA pin
#define RST_PIN D1     // RFID RST pin
#define RELAY_PIN D3   // Relay to control traffic light (green)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Hardcoded UID of the ambulance RFID tag (update as needed)
byte ambulanceTag[] = {0xDE, 0xAD, 0xBE, 0xEF}; // Replace with actual tag UID

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Default: Red light ON

  Serial.println("🚦 Traffic System Ready - Waiting for RFID...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  bool isAmbulance = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != ambulanceTag[i]) {
      isAmbulance = false;
      break;
    }
  }

  if (isAmbulance) {
    Serial.println("🚑 AMBULANCE DETECTED! Switching to GREEN light.");
    digitalWrite(RELAY_PIN, HIGH);
    delay(10000);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("🟥 Back to normal traffic mode.");
  } else {
    Serial.println("Unauthorized RFID tag detected.");
  }

  mfrc522.PICC_HaltA();
}
