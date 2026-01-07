#include <Wire.h>
#include <Servo.h>

Servo myServo;

// --- PIN DEFINITIONS ---
const int LED_PIN = 9;      // LED connected to Pin 7
const int SERVO_PIN = 10;    // Servo connected to Pin 9
const int SLAVE_ADDR = 8;   // I2C Address (Must match Master code)

// Variables to store received data
int receivedAngle = 0;
int receivedBtnState = 0;

void setup() {
  // 1. Setup Output Pins
  pinMode(LED_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  
  // Initialize to default states
  myServo.write(0);          // Start servo at 0 degrees
  digitalWrite(LED_PIN, LOW);// Start LED OFF

  // 2. Join I2C Bus as Slave
  Wire.begin(SLAVE_ADDR);    // Join with address 8
  
  // 3. Register the event to handle incoming data
  Wire.onReceive(receiveEvent);
  
}

void loop() {
  // The loop can stay empty because 'receiveEvent' triggers automatically
  // whenever data arrives from the Master.
  delay(100); 
}

// --- FUNCTION TO HANDLE DATA FROM MASTER ---
void receiveEvent(int howMany) {
  // We expect 2 bytes: [Angle, ButtonState]
  if (howMany >= 2) {
    receivedAngle = Wire.read();    // 1st Byte: Servo Angle (0-180)
    receivedBtnState = Wire.read(); // 2nd Byte: Button State (0 or 1)

    // A. Update Servo Position
    myServo.write(receivedAngle);

    // B. Update LED State
    if (receivedBtnState == 1) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }

    // Debugging: Print what we received to Serial Monitor
    Serial.print("Received Angle: ");
    Serial.print(receivedAngle);
    Serial.print(" | LED Command: ");
    Serial.println(receivedBtnState);
  }
}