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

bool sendStuff = false;
bool next = false;

DebugServer dServer(80);

Scheduler sched;
Task server_task(0, TASK_FOREVER, &server_loop, &sched);
Task main_task(0, TASK_FOREVER, &main_loop, &sched);

unsigned long int prev = 0;
int sensorPins[] = {32, 33, 34, 35, 36, 39};
// int sensorPins[] = {34, 35, 32, 33, 25, 26, 27, 14, 21, 22};

DriveLogic robot;
DataCollector sensors(sensorPins, 6);


void reactOnDebugCommands() {
  String command = dServer.getCommand(),
          arg = dServer.getArgument();
  if(command == "forward") 
    robot.forward();
  else if(command== "stop") 
    robot.stop();
  else if(command == "backward") 
    robot.backward();
  else if(command == "steer") {
    wPrintln("sending arg to servo: "+arg);
    robot.setActorSteering(arg.toInt());
  }
  else if(command == "send") 
    sendStuff = true;
  else if(command == "shut" && arg == "up") 
    sendStuff = false;
}

void setup() {
  Serial.begin(115200);

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
    sensors.readSensors();
  // sensors.cluster();
  // sensors.classifyAll();
  // robot.reactOnSensors(sensors.sensors);
    reactOnDebugCommands();

    if(sendStuff) {
      wPrint("[");
      for(int x = 0; x<6; x++)
        wPrint(String(sensors.sensors[x])+",");
      wPrintln("]");
    }

  }
}
