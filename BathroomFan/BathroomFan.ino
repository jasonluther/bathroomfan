#include <DHT22.h>


const int humiditySensorPin = 4;
const int relayPin = 8;

DHT22 sensor(humiditySensorPin);

// Global state
boolean FAN_ON = true;
unsigned long STARTED_TIME;
unsigned long MAX_DURATION = 1000L * 60L * 90L;
unsigned long DURATION;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Bathroom Fan Controller, ahoy!");
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Turn the fan on when the switch is turned on.
  FAN_ON = true;
  STARTED_TIME = millis();
  DURATION = 1000L * 60L * 15L;
  delay(2001); // Give the sensor 2 seconds to warm up.
  Serial.print("MAX_DURATION: ");
  Serial.println(MAX_DURATION);   
  Serial.print("DURATION: ");
  Serial.println(DURATION); 
}

void loop(void) {
  // If the fan state is on, make sure it's on
  if (FAN_ON) {
    Serial.println("Fan is on.");
    digitalWrite(relayPin, HIGH); 
    if (STARTED_TIME == 0) {
      STARTED_TIME = millis();
    }
  } else {
    Serial.println("Fan is off.");
    digitalWrite(relayPin, LOW); 
    STARTED_TIME = 0;
  }

  // Should the fan be on? Check the time.
  if (millis() > (STARTED_TIME + MAX_DURATION)) {
    Serial.println("Fan exceeded MAX_DURATION");
    FAN_ON = false;
  } else if (millis() > (STARTED_TIME + DURATION)) {
    Serial.println("Fan exceeded DURATION");
    FAN_ON = false;
  }
  
  // Should the fan be on? Check the humidity and set DURATION.
  sensor.readData();
  float relative_humidity = sensor.getHumidity();
  Serial.print("Humidity: ");
  Serial.print(relative_humidity);
  Serial.println("%");
  if (relative_humidity > 90) {
    DURATION = 1000L * 60L * 45L; // Stay on for 45 minutes as long as it's super humid.
    FAN_ON = true;
  } else if (relative_humidity > 70) {
    DURATION = 1000L * 60L * 20L; // Stay on for 20 minutes as long as it's a bit humid.
    FAN_ON = true;
  }
  
  delay(5000);
}
