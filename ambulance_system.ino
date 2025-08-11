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
  SPI.begin();            // Initialize SPI bus
  mfrc522.PCD_Init();     // Initialize RFID reader
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Default state: Red light ON (relay OFF)

  Serial.println("🚦 Traffic System Ready - Waiting for RFID...");
}

void loop() {
  // Check if a new RFID card is present and readable
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Debug: Show UID of scanned card
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Check if the scanned card matches ambulance tag
  bool isAmbulance = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != ambulanceTag[i]) {
      isAmbulance = false;
      break;
    }
  }               

  // If ambulance detected
  if (isAmbulance) {
    Serial.println("🚑 AMBULANCE DETECTED! Switching to GREEN light.");
    digitalWrite(RELAY_PIN, HIGH);  // Green light ON
    delay(10000);                   // Green stays ON for 10 seconds
    digitalWrite(RELAY_PIN, LOW);   // Back to Red light
    Serial.println("🟥 Back to normal traffic mode.");
  } else {
    Serial.println("Unauthorized RFID tag detected.");
  }

  // Halt communication with RFID tag
  mfrc522.PICC_HaltA();
}