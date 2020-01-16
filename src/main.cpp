#include "DebugServer.h"
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

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(100);
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

// only work with millis() delay here, delay() seems to crash the task scheduler
void main_loop() {
  yield();
  if((millis()-prev) > 1000) {
    prev = millis();
    wPrintln("hello guys");
  }
}