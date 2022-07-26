#include <Arduino.h>
#include <ESPDateTime.h>
#include <LittleFS.h>
#include <Wire.h>
#include <WiFi.h>
#include <JY901.h>
 
#define FORMAT_LITTLEFS_IF_FAILED true

String path = "/sensorData-";

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("TomeGuest", "tomeroyaloak");
  Serial.println(millis());
  Serial.println("WiFi Connecting...");
  delay(3000);
  Serial.println(WiFi.status());
}

void setupDateTime() {
  DateTime.setTimeZone("EST5EDT,M3.2.0,M11.1.0");
  DateTime.setServer("asia.pool.ntp.org");
  DateTime.begin(15 * 1000);
  // this method config ntp and wait for time sync
  // default timeout is 10 seconds
  DateTime.begin(/* timeout param */);
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  }
}

void setup() 
{
  Serial.begin(9600);
  setupWiFi();
  setupDateTime();

  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
    Serial.println("LittleFS Mount Failed");
    return;
  }

  delay(1000);
  path.concat(DateTime.now());
  path.concat(".csv");
}

String getSensorData() {
  // First row: Time
  String csvData= "Time,";
  csvData.concat(JY901.stcTime.ucYear);
  csvData.concat(",");
  csvData.concat(JY901.stcTime.ucMonth);
  csvData.concat(",");
  csvData.concat(JY901.stcTime.ucDay);
  csvData.concat(",");
  csvData.concat(JY901.stcTime.ucHour);
  csvData.concat(",");
  csvData.concat(JY901.stcTime.ucMinute);
  csvData.concat(",");
  csvData.concat(JY901.stcTime.ucSecond);
  csvData.concat(",");
  csvData.concat(millis());
  csvData.concat(",\n");

  // Second Row: Accel
  csvData.concat("Accel,");
  csvData.concat((float)JY901.stcAcc.a[0]/32768*16);
  csvData.concat(",");
  csvData.concat((float)JY901.stcAcc.a[1]/32768*16);
  csvData.concat(",");
  csvData.concat((float)JY901.stcAcc.a[3]/32768*16);
  csvData.concat(",\n");
  
  // Third row: Gyro
  csvData.concat("Gyro,");
  csvData.concat((float)JY901.stcGyro.w[0]/32768*2000);
  csvData.concat(",");
  csvData.concat((float)JY901.stcGyro.w[1]/32768*2000);
  csvData.concat(",");
  csvData.concat((float)JY901.stcGyro.w[2]/32768*2000);
  csvData.concat(",\n");

  // Fourth row: Angle
  csvData.concat("Angle,");
  csvData.concat((float)JY901.stcAngle.Angle[0]/32768*180);
  csvData.concat(",");
  csvData.concat((float)JY901.stcAngle.Angle[1]/32768*180);
  csvData.concat(",");
  csvData.concat((float)JY901.stcAngle.Angle[2]/32768*180);
  csvData.concat(",\n");

  // Fourth row: Mag
  csvData.concat("Mag,");
  csvData.concat(JY901.stcMag.h[0]);
  csvData.concat(",");
  csvData.concat(JY901.stcMag.h[1]);
  csvData.concat(",");
  csvData.concat(JY901.stcMag.h[2]);
  csvData.concat(",\n");

  csvData.concat("\n");
  return csvData;
}

void loop() 
{
  //print received data. Data was received in serialEvent;
  String csvData = getSensorData();
  Serial.printf("Writing file: %s\r\n", path.c_str());

  File file = LittleFS.open(path.c_str(), FILE_APPEND);
  if(!file){
    Serial.println("- failed to open file for appending");
    File file = LittleFS.open(path.c_str(), FILE_WRITE);
    if(!file){
      Serial.println("- failed to open file for writing");
      return;
    }
  }

  if(file.print(csvData)){
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
  Serial.print(csvData);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() 
{
  while (Serial.available()) 
  {
    JY901.CopeSerialData(Serial.read()); //Call JY901 data cope function
  }
}


