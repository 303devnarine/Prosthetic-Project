#include "LiquidCrystal_I2C.h"
#include <ArduinoBLE.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int outputX = A0; // pin for control 

BLEService controlService("979084DE-930F-4134-8B08-7C0FB8E10C37");
BLEByteCharacteristic XCharacteristic("979084DE-931F-4134-8B08-7C0FB8E10C37", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  
  pinMode(outputX, INPUT);

  // BLE
  if (!BLE.begin()) Serial.println("starting Bluetooth® Low Energy failed.");
  else Serial.println("Bluetooth® Low Energy started successfully.");
  
  BLE.setLocalName("Control Module");
  BLE.setAdvertisedService(controlService);

  controlService.addCharacteristic(XCharacteristic);

  BLE.addService(controlService);
  BLE.advertise();
  Serial.println("BLE Electrode Module, awaiting connection....");
  
  // LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Dev.");
  delay(1000);
  lcd.clear();
  delay(1000);
}

void loop() {

  BLEDevice central = BLE.central();

  lcd.setCursor(3, 0);
  lcd.print("Awaiting");
  lcd.setCursor(2, 1);
  lcd.print("Connection");
  delay(200);
  lcd.print(".");
  delay(200);
  lcd.print(".");
  delay(200);
  lcd.print(".");

  if (central) {
    lcd.clear();
    delay(100);
    lcd.setCursor(2, 0);
    lcd.print("Successful");
    lcd.setCursor(2, 1);
    lcd.print("Connection");
    delay(400);
    lcd.clear();
    delay(200);

    Serial.print("Connected to Arm Module Device (");
    Serial.print(central.address());
    Serial.println(")");

    while (central.connected()) {
      // takes values from pins, ADC to volts (V)
      float rawValX = (analogRead(outputX) / 4);
      float displayValX = ((rawValX * 20)/1023);
  
      // Serial Monitor
      Serial.println("Raw X: " + String(rawValX));
      Serial.println("Display X: " + String(displayValX, 4) + "V");
      // LCD
      lcd.setCursor(0, 0);
      lcd.print("X: " + String(displayValX, 4) + "V");
      lcd.setCursor(11, 1);
      lcd.print("Dev");

      // BLE Characteristic Write
      XCharacteristic.writeValue((byte)rawValX);

      delay(200);

      lcd.clear();
    }
    
    Serial.print(F("Arm Module Device Disconnected. ("));
    Serial.print(central.address());
    Serial.println(")");
  }
  delay(1);

  lcd.clear();
}
