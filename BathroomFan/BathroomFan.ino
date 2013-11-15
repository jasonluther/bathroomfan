#include <DHT22.h>


const int humiditySensorPin = 4;
const int relayPin = 8;
const int ledPin = 13;

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
  pinMode(ledPin, OUTPUT); // the built-in Arduion LED on pin 13
}

void loop(void) {
  DHT22_ERROR_t errorCode;
  
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
    DURATION = 0;
  } else if (millis() > (STARTED_TIME + DURATION)) {
    Serial.println("Fan exceeded DURATION");
    FAN_ON = false;
    DURATION = 0;
  }
  
  // Should the fan be on? Check the humidity and set DURATION.
  Serial.print("Requesting data...");
  errorCode = sensor.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Serial.print("Got Data ");
      Serial.print(sensor.getTemperatureF());
      Serial.print("F ");
      Serial.print(sensor.getHumidity());
      Serial.println("%");
      // Alternately, with integer formatting which is clumsier but more compact to store and
	  // can be compared reliably for equality:
	  //	  
      char buf[128];
      sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
                   sensor.getTemperatureCInt()/10, abs(sensor.getTemperatureCInt()%10),
                   sensor.getHumidityInt()/10, sensor.getHumidityInt()%10);
      Serial.println(buf);
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(sensor.getTemperatureC());
      Serial.print("C ");
      Serial.print(sensor.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("Bus Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Must wait 2s before reading data again ");
      break;
  }
  
  float relative_humidity = sensor.getHumidity();
  blink(relative_humidity / 10);
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
  
  delay(3000);
}

void blink(int times) {
  int count = 10;
  while (count-- > 0) {
    if (times > 0) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
      times--;
    } else {
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  }
}
