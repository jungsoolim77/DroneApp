#include <ArduinoBLE.h>

long previousMillis = 0;
int interval = 0;
int ledState = LOW;
const int ledPin = 11;
const int motor1pin1 = 2;
const int motor1pin2 = 3;
const int motor2pin1 = 4;
const int motor2pin2 = 5;
const int enbA = 9;
const int enbB = 10;
const int low = 100;
const int mid = 150;
const int high = 220;


BLEService ledService("180A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set built in LED pin to output mode
  pinMode(ledPin, OUTPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(enbA,OUTPUT);
  pinMode(enbB,OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  // Serial.println(switchCharacteristic.uuid());

}

void loop() {
  

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      
      if (switchCharacteristic.written()) {
        Serial.println("received : "); Serial.print(switchCharacteristic.value()); Serial.println("// ");
        setPins();
        uint32_t value = switchCharacteristic.value();
        uint32_t speed = value*20;
        Serial.println(speed);
        switch (value) {   
          case 00:
            Serial.println("Stop");
            stopMotor();
            digitalWrite(ledPin, LOW);
            break;
          case 01:
            Serial.println("Low Speed");
            ledBlink(1);       //blink once
            setSpeed(low);
            break;
          case 02:
            Serial.println("Medium Speed");
            ledBlink(2);       //blink once
            setSpeed(mid);  
            break;
          case 03:
            Serial.println("High Speed");
            ledBlink(3);       //blink once
            setSpeed(high); 
            break;
          default:
            Serial.println(F("LED off"));       //using slider
            digitalWrite(ledPin, LOW);          // will turn the LED off
            setSpeed(speed); 
            break;
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(ledPin, LOW);         // will turn the LED off
  }
}
void setPins(){
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void stopMotor(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
  analogWrite(enbA, 0);
  analogWrite(enbB, 0);
}

void setSpeed(int speed){
  analogWrite(enbA, speed);
  analogWrite(enbB, speed);
}

void ledBlink(int times){
  for(int i=0; i < times; i++){
    digitalWrite(ledPin, HIGH);         // will turn the LED on
    delay(1000);
    digitalWrite(ledPin, LOW);         // will turn the LED off
    delay(1000);
  }
}

