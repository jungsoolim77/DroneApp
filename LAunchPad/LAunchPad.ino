#include <ArduinoBLE.h>

const int ledPin = 12;
const int motor1pin1 = 10;
const int motor1pin2 = 9;
const int enbA = 8;
const int interval = 1000;
const int low = 150;
const int mid = 190;
const int high = 220;
const int ramp_time = 40; 

// variables
int PWM_offset = 150;   // offset for PWM signal (between 0-255), roughly the value that makes the drone hover
int PWM_signal = 0;     // PWM value

BLEService ledService("180A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  // while (!Serial);

  // set built in LED pin to output mode
  pinMode(ledPin, OUTPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(enbA,OUTPUT);
   
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

  // Serial.println("BLE LED Peripheral");
  Serial.println(switchCharacteristic.uuid());

}

void loop() {
  // put your main code here, to run repeatedly:
   
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
        // setPins();
        uint32_t value = switchCharacteristic.value();
        
        // blink LED as requested
        ledBlink(value);
            
        switch (value) {   
          case 00:
            Serial.println("Land");
            PWM_offset = 0;
            break;
          case 01:
            Serial.println("Low Speed");
            PWM_offset = low;
            break;
          case 02:
            Serial.println("Medium Speed");
            PWM_offset = mid;
            break;
          case 03:
            Serial.println("High Speed");
            PWM_offset = high;
            break;
          default:
            Serial.println(F("Land"));       //using slider
            PWM_offset = 0;
            break;
        }
        if (PWM_offset > 0){
          LiftOff();
        } else{
          Land();
        }
              
      }
    }
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    // digitalWrite(ledPin, LOW);         // will turn the LED off
  }
}

void ledBlink(int times){
  for (int i = 0; i <times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(interval);
    digitalWrite(ledPin, LOW);
    delay(interval);
  }
}

void LiftOff(){         // slowly ramp up motor speed to lift off
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  while(PWM_signal<PWM_offset){ //slowly ramp up motor speed for smooth takeoff
    analogWrite(enbA, PWM_signal);  // send PWM signal to output pin
    PWM_signal++;
    delay(ramp_time);
  }
}

void Land(){           // slowly ramp down motor speed to land safely
  while(PWM_signal>0){ //slowly ramp up motor speed for smooth takeoff
    analogWrite(enbA, PWM_signal);  // send PWM signal to output pin
    PWM_signal--;
    delay(ramp_time);
  }
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(enbA, LOW);
}


