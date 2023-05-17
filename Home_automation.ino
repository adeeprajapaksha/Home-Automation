#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "TMPL5f_8Lpr8N"
#define BLYNK_TEMPLATE_NAME "Home automation"

// Set up Wi-Fi credentials
char auth[] = "Qe-phawkQvZWquNVg-lKWnmBxACbWpUI";
char ssid[] = "SLT-Fiber-9987";
char pass[] = "ks2499881";

// Set up PIR sensor and relay modules
int pirPin = 23; // PIR sensor connected to digital pin 23
int relayPin1 = 14; // First relay module connected to digital pin 14
int relayPin2 = 5; // Second relay module connected to digital pin 5
int ldrPin = 34; // LDR sensor connected to analog pin 34

// Initialize PIR sensor state
int pirState = LOW;

//Variable for get the ldr on and of status through blynk app
int ldr_state = 1;

void setup() {

 Serial.begin(9600); // Start serial communication with 9600 baud rate
  pinMode(pirPin, INPUT); // Set PIR sensor pin as input
  pinMode(relayPin1, OUTPUT); // Set first relay module pin as output
  pinMode(relayPin2, OUTPUT); // Set second relay module pin as output
  pinMode(ldrPin, INPUT); // Set LDR sensor pin as input

 // Connect to Wi-Fi
  WiFi.begin(ssid, pass); // Connect to Wi-Fi network with SSID and password
  while (WiFi.status() != WL_CONNECTED) { // Wait until Wi-Fi is connected
    delay(1000); // Wait for 1 second``
    Serial.println("Connecting to WiFi..."); // Print message to serial monitor
  }

  // Connect to Blynk server
  Blynk.begin(auth, ssid, pass); // Connect to Blynk server with authorization token, SSID and password
  while (!Blynk.connected()) { // Wait until Blynk is connected
    delay(1000); // Wait for 1 second
    Serial.println("Connecting to Blynk server..."); // Print message to serial monitor
  }

  Serial.println("Connected!"); // Print message to serial monitor
  Serial.println(digitalRead(pirPin)); // Print PIR sensor state to serial monitor

  digitalWrite(relayPin1, HIGH); // Set first relay module state to off
  digitalWrite(relayPin2, HIGH); // Set second relay module state to off
}

// Check if switch for first relay is turned on/off in Blynk app
BLYNK_WRITE(V1) {
  int state = param.asInt();
  if (state == HIGH) {
    digitalWrite(relayPin1, LOW); // Turn on first relay module
  } else {
    digitalWrite(relayPin1, HIGH); // Turn off first relay module
  }
}

// Check if switch for second relay is turned on/off in Blynk app
BLYNK_WRITE(V2) {
  int state = param.asInt();
  if (state == HIGH) {
    digitalWrite(relayPin2, LOW); // Turn on second relay module
  } else {
    digitalWrite(relayPin2, HIGH); // Turn off second relay module
  }
}

// Check if switch for LDR sensor is turned on/off in Blynk app
BLYNK_WRITE(V3) {
  int state = param.asInt();
  // If PIR sensor detects motion and is not in a triggered state
  if (state == HIGH) {
    ldr_state = 1; // LDR sensor is on
  } else {
    ldr_state = 0; // LDR sensor is off
  }
}


void loop() {
  
  int sensorValue = digitalRead(pirPin);// Read PIR sensor value
  int ldrValue = analogRead(ldrPin);//Read LDR sensor value
  int mappedValue = map(ldrValue, 250, 4095, 0, 100);// Read LDR sensor value and map it from 250-4095 to 0-100

  if (sensorValue == 1 && pirState == LOW) {   // PIR sensor detected motion and PIR sensor turned on through Blynk app
    
    
    
    digitalWrite(relayPin1, LOW);// Turn on first relay module
    delay(5000);// Wait for 5 seconds before turning off the relay module
    digitalWrite(relayPin1, HIGH);
    delay(1000);
    Blynk.logEvent("something_suspicious_is_happening", "CHECK OUTSIDE!!!");// Log event in Blynk app
    pirState = HIGH;// Set PIR sensor state to triggered
    
  } else if (sensorValue == 0 && pirState == HIGH) {
    // PIR sensor no longer detecting motion
    
    pirState = LOW;
  }

  if (ldr_state == 1) {    // Check if LDR sensor is turned on in Blynk app

    if (mappedValue > 50) {    // If LDR value is above threshold, turn on second relay module
      digitalWrite(relayPin2, LOW);
    } else {
      digitalWrite(relayPin2, HIGH); // If LDR value is below threshold, turn off second relay module
    }
    
  }
  // Print LDR value and LDR state to Serial Monitor for debugging
  
  Serial.println(sensorValue);
  


  // Check if Blynk app is connected and receiving messages
  Blynk.run();
}
