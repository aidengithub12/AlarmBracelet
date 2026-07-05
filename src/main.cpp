#include <Arduino.h>
#include "RTClib.h"


#define setHour 0
#define setMinute 0
#define deltaTime 10 //minutes

RTC_DS3231 rtc;
RTC_DATA_ATTR uint8_t currTime[2] = {};
RTC_DATA_ATTR uint8_t setTime[2] = {};
// bool alarmReached = false;
RTC_DATA_ATTR bool hasInit = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(23, OUTPUT); //Change pin number to the one connected to the vibration motor

  while (!Serial) {
    delay(1); // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Serial is initialized...");

  // //motor test
  // digitalWrite(23, HIGH); // Turn on vibration motor
  // delay(1000);
  // digitalWrite(23, LOW); // Turn off vibration motor

  while (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (!hasInit) {
    currTime[0] = rtc.now().hour();
    currTime[1] = rtc.now().minute();
    setTime[0] = setHour;
    setTime[1] = setMinute;
    hasInit = true;
  }
  currTime[0] = rtc.now().hour();
  currTime[1] = rtc.now().minute();

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
// following line sets the RTC to the date & time this sketch was compiled 
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    
  }

  Serial.println("RTC is initialized...");
  if ((Serial.available() > 0) && !hasInit) {
    Serial.print("What time should you wake? (HH:MM:(A/P)\\:");
    String timestr = Serial.readStringUntil('\\');
    setTime[0] = timestr.substring(0, 2).toInt();
    setTime[1] = timestr.substring(3, 5).toInt();
  }

  if (((currTime[0] >= setTime[0]) && (currTime[0] <= setTime[0] + deltaTime / 60)) && ((currTime[1] >= setTime[1]) && (currTime[1] <= setTime[1] + deltaTime))) {
    Serial.println("Alarm reached!");
    delay(1000);
    digitalWrite(23, HIGH); // Turn on vibration motor
    delay(1000);
    digitalWrite(23, LOW); // Turn off vibration motor
    hasInit = false;
  }
  Serial.println("Checking alarm...");
  esp_sleep_enable_timer_wakeup(deltaTime * 60 * 1000000ULL);
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() {;}

