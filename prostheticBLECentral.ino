#include <ArduinoBLE.h>
#include <WiFiNINA.h>
#include <Servo.h>

Servo digital[5];

void setup() {
  pinMode(A0, OUTPUT);

  for (int i=0; i<5; i++) { 
    digital[i].attach(i + 7);
    digital[i].write(45);
  }

  BLE.begin();
  BLE.scanForUuid("979084DE-930F-4134-8B08-7C0FB8E10C37");
}

void loop() {
  BLEDevice controlModule = BLE.available();
  Serial.println("Awaiting Connection...");
  delay(100);

  if (controlModule) {
    Serial.print("Discovered ");
    Serial.print("'");
    Serial.print(controlModule.localName());
    Serial.print("' at [");
    Serial.print(controlModule.address());
    Serial.print("] (");
    Serial.print(controlModule.advertisedServiceUuid());
    Serial.println(")");
    if (controlModule.localName().indexOf("Control Module") < 0) {
      Serial.println("'Control Module' not found."); return;
    }
    BLE.stopScan();
    control(controlModule);
    BLE.scanForUuid("979084DE-930F-4134-8B08-7C0FB8E10C37");
  }
}

void control(BLEDevice controlModule) {
  Serial.println("Connecting...");
  if (controlModule.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }
  
  Serial.println("Discovering attributes...");
  if (controlModule.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    controlModule.disconnect();
    return;
  }

  BLECharacteristic XCharacteristic = controlModule.characteristic("979084DE-931F-4134-8B08-7C0FB8E10C37");

         if (!XCharacteristic)
    Serial.println("Missing characteristics.");
  

  while (controlModule.connected()) {
    if (XCharacteristic.canRead()) {
      byte XVal = XCharacteristic.read();
      XCharacteristic.readValue(XVal);
      Serial.println(XVal);

           if (XVal < 44)               { for (int i=0; i<5; i++) digital[i].write(0);    Serial.println("0_");   analogWrite(A0, 0);   }
      else if (XVal > 44 && XVal < 134) { for (int i=0; i<5; i++) digital[i].write(50);   Serial.println("50_");  analogWrite(A0, 512);  }
      else if (XVal > 134)              { for (int i=0; i<5; i++) digital[i].write(100);  Serial.println("100_"); analogWrite(A0, 1023);   }

    }
  }
}
