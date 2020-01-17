#include "DebugServer.h"
#include "DataCollector.h"
#include "DriveLogic.h"
#include <WiFi.h>
#include <TaskScheduler.h>

/* Put your SSID & Password */
const char* ssid = "WinningRobot";  // Enter SSID here
const char* password = "SyntacticSugar";  //Enter Password here

void server_loop();
void main_loop();

DebugServer dServer(80);

Scheduler sched;
Task server_task(0, TASK_FOREVER, &server_loop, &sched);
Task main_task(0, TASK_FOREVER, &main_loop, &sched);

unsigned long int prev = 0;
int sensorPins[] = {32, 33, 34, 35, 36, 39};
// int sensorPins[] = {34, 35, 32, 33, 25, 26, 27, 14, 21, 22};

DriveLogic robot;
DataCollector sensors(sensorPins, 6);

void setup() {
  // Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(2000);
  dServer.init();
  Serial.println("HTTP server started");
  wPrintln("Starting stuff...");
  server_task.enable();
  main_task.enable();
  sched.startNow();
}

void loop() {
  sched.execute();
}

void server_loop() {
  yield();
  dServer.loop();
}

int mode = 0;
// only work with millis() delay here, delay() seems to crash the task scheduler
void main_loop() {
  yield();
  if((millis()-prev) > 1000) {
    prev = millis();
    wPrintln("data points: "+String(sensors.getDataCount()));
    if(mode == 0) robot.forward();
    else if((mode) == 1) robot.stop();
    else if((mode) == 2) robot.backward();
    else if((mode) == 3) robot.stop();
    mode++;
    mode %=4;
    robot.rotate(mode*55);
    sensors.readSensors();
    wPrint("[");
    for(int x = 0; x<6; x++)
      wPrint(String(sensors.sensors[x])+",");
    wPrintln("]");
  }
}

// #include "DebugServer.h"
// #include "DataCollector.h"
// #include "DriveLogic.h"
// #include <WiFi.h>
// #include <TaskScheduler.h>

// #include <Arduino.h>


// DebugServer dServer(80);

// void setup() {
//   Serial.begin(115200);
//   //   // Serial.begin(115200);
//   // Wifi.begin();k
//   WiFi.softAP(ssid, password);
//   delay(2000);
//   dServer.init();
//   Serial.println("HTTP server started");
//   // wPrintln("Starting stuff...");
// }

// void loop() {
//   Serial.println("here");
//   delay(1000);
// }