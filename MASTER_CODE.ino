#include <Wire.h>
#include <DHT.h>

// --- PIN DEFINITIONS ---
const int POT_PIN = A0;     // Potentiometer Input
const int DHT_PIN = 2;      // DHT11 Signal Input

// --- SENSOR SETUP ---
#define DHTTYPE DHT11       
DHT dht(DHT_PIN, DHTTYPE);  

// --- VARIABLES ---
int potValue = 0;
int angle = 0;
int temp = 0;
int ledCommand = 0;         // 0 = OFF, 1 = ON

// --- CONTROL FLAGS ---
bool autoMode = true;       // Start in Automatic (Sensor) Mode

void setup() {
  Wire.begin();       
  dht.begin();        
  Serial.begin(9600); 
  
  Serial.println("SYSTEM STARTED");
  Serial.println("Default Mode: AUTO (Sensors active)");
  Serial.println("----------------------------------------------");
  Serial.println("COMMANDS:");
  Serial.println("  Type 'M' -> Switch to MANUAL Mode");
  Serial.println("  Type 'A' -> Switch to AUTO Mode");
  Serial.println("  (In Manual) Type number 0-180 -> Move Servo");
  Serial.println("  (In Manual) Type 'L' -> Toggle LED");
  Serial.println("----------------------------------------------");
  delay(2000);
}

void loop() {
  // ==========================================
  // STEP 1: CHECK FOR USER INPUT (SERIAL)
  // ==========================================
  if (Serial.available() > 0) {
    char cmd = Serial.peek(); // Look at the first character without removing it

    // SWITCH TO AUTO MODE
    if (cmd == 'A' || cmd == 'a') {
      Serial.read(); // Clear the character from buffer
      autoMode = true;
      Serial.println(">>> MODE CHANGED: AUTO (Sensors Controlling)");
    }
    
    // SWITCH TO MANUAL MODE
    else if (cmd == 'M' || cmd == 'm') {
      Serial.read(); // Clear the character
      autoMode = false;
      Serial.println(">>> MODE CHANGED: MANUAL (User Input Controlling)");
    }
    
    // MANUAL CONTROL: TOGGLE LED
    else if (!autoMode && (cmd == 'L' || cmd == 'l')) {
       Serial.read(); // Clear character
       if (ledCommand == 0) ledCommand = 1; else ledCommand = 0; // Toggle
       Serial.print(">>> Manual LED Set to: ");
       Serial.println(ledCommand);
    }
    
    // MANUAL CONTROL: SET SERVO ANGLE
    else if (!autoMode && isDigit(cmd)) {
       // If the user typed a number, read the whole number
       int newAngle = Serial.parseInt(); 
       
       // Constrain it to valid servo range
       if (newAngle >= 0 && newAngle <= 180) {
         angle = newAngle;
         Serial.print(">>> Manual Servo Angle: ");
         Serial.println(angle);
       } else {
         Serial.println(">>> Error: Angle must be 0-180");
       }
       
       // Clear any remaining "Enter" or newline characters
       while (Serial.available()) Serial.read(); 
    }
    
    // CLEAR JUNK DATA (Spaces, newlines we didn't catch)
    else {
      Serial.read(); 
    }
  }

  // ==========================================
  // STEP 2: READ SENSORS (Only if in Auto Mode)
  // ==========================================
  if (autoMode == true) {
    // A. Read Potentiometer
    potValue = analogRead(POT_PIN);
    angle = map(potValue, 0, 1023, 0, 180);

    // B. Read Temperature
    temp = dht.readTemperature();
    if (isnan(temp)) temp = 0;

    // C. LED Logic (Temp Alarm)
    if (temp >= 28) {
      ledCommand = 1; 
    } else {
      ledCommand = 0;
    }
  }

  // ==========================================
  // STEP 3: SEND FINAL DATA TO SLAVE
  // ==========================================
  // Whether data came from Sensors (Auto) or Serial (Manual), 
  // we send the current 'angle' and 'ledCommand' to the Slave.
  
  Wire.beginTransmission(8); 
  Wire.write(angle);         
  Wire.write(ledCommand);    
  Wire.endTransmission();    


  // ==========================================
  // DEBUGGING
  // ==========================================
  // Only print periodically to avoid spamming the monitor
  Serial.print("MODE: ");
  if (autoMode) Serial.print("AUTO   "); else Serial.print("MANUAL ");
  
  Serial.print("| Servo: "); 
  Serial.print(angle);
  Serial.print(" | LED: "); 
  Serial.print(ledCommand);
  
  if (autoMode) {
    Serial.print(" | Temp: "); 
    Serial.print(temp);
    Serial.print("C");
  }
  Serial.println(); // New line

  delay(500); 
}