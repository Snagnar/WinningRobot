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

int stopTimeout = 0;
unsigned long int stopStart = 0;

DebugServer dServer(80);

Scheduler sched;
Task server_task(0, TASK_FOREVER, &server_loop, &sched);
Task main_task(0, TASK_FOREVER, &main_loop, &sched);

unsigned long int prev = 0;
unsigned long int prev2 = 0;

int sensorPins[] = {32, 33, 34, 35, 36, 39};

byte mode = 0;
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
  else if(command == "clear" && arg == "all")
    sensors.clearSensorValues();
  else if(command == "command") {
    mode = 0;
  }
  else if(command == "start") {
    sensors.clearSensorValues();
    robot.activateSpiral();
    prev = millis();
    prev2 = millis();
    mode = 1;
  }
  else if(command == "speed")
    robot.setActorSpeed(arg.toInt());
  else if(command == "forwardFor") {
    robot.forward();
    stopTimeout = arg.toInt();
    stopStart = millis();
    wPrintln("got timeout: "+String(stopTimeout)+" and timestamp: "+String(stopStart));
  }
  else if(command == "backwardFor") {
    robot.backward();
    stopTimeout = arg.toInt();
    stopStart = millis();
    wPrintln("got timeout: "+String(stopTimeout)+" and timestamp: "+String(stopStart));
  }
  else if(command == "robotDebug") {
    if(arg == "start") robot.activateDebug();
    if(arg == "stop") robot.deactivateDebug();
  }
  else if(command == "fRotation") {
    robot.forward();
    robot.rotate(arg.toInt());
  }
  else if(command == "setDeg") {
    robot.setDPMSPSD(arg.toDouble());
  }
  else if(command == "setMSS") {
    robot.setMaxSteerPerMs(arg.toDouble());
  }
  else if(command == "setMSA") {
    robot.setMaxSteerAngle(arg.toDouble());
  }
  else if(command == "spiral") {
    robot.activateSpiral();
    robot.driveSpiral();
  }
  else if(command == "nospiral") {
    robot.deactivateSpiral();
  }
}

void command_mode() {
  if(robot.rotating() && (millis() - prev2) > 200) {
    if(robot.rotating())
      robot.performRotationStep();
    if(!robot.rotating())
      robot.stop();
    prev2 = millis();
  }
  if((millis()-prev) > 200) {
    prev = millis();
    sensors.readSensors();
    // robot.reactOnSensors(sensors.sensors);
    reactOnDebugCommands();

    // wPrintln("diff: "+String(millis() - stopStart)+" cond: "+String(stopTimeout > 0 && (millis() - stopStart) > stopTimeout));

    if(stopTimeout > 0 && (millis() - stopStart) > stopTimeout) {
      wPrintln("stopping...");
      robot.stop();
      stopTimeout = 0;
    }

    if(sendStuff) {
      wPrintln("datapoints: "+String(sensors.getDataCount()));
      wPrint("raw: [");
      for(int x = 0; x<6; x++)
        wPrint(String(sensors.sensors[x])+",");
      wPrintln("]");

      sensors.cluster();
      wPrintln("border 1: "+String(sensors.borders[0])+" border 2: "+String(sensors.borders[1]));
      sensors.classifyAll();
      wPrint("classified: [");
      for(int x = 0; x<6; x++)
        wPrint(String(sensors.sensors[x])+",");
      wPrintln("]");
    }
  }
}

void explore_mode() {
  if((millis() - prev2) > 20) {
    robot.driveSpiral();
    prev2 = millis();
  }
  if((millis() - prev) > 100) {
    prev = millis();
    sensors.readSensors();
    reactOnDebugCommands();
    wPrintln("datas :"+String(sensors.getDataCount()));
    if(sensors.getDataCount() > 1000) {
      mode = 2;
      // robot.forward();
      sensors.cluster();
      robot.stop();
      robot.setActorSteering(90);
      robot.activateSpiral();
       wPrintln("now done with mode 1");
      // robot.stop();
      // robot.setActorSteering(90);
      // mode = 0;
    }
  }
}

void follow_the_light_mode() {
  if(robot.rotating() && (millis() - prev2) > 10) {
    if(robot.rotating())
      robot.performRotationStep();
    prev2 = millis();
  }
  if((millis() - prev) > 30) {
    prev = millis();
    sensors.readSensors();
    sensors.classifyAll();
    reactOnDebugCommands();
    robot.reactOnSensors(sensors.sensors);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(2000);
  dServer.init();
  Serial.println("HTTP server started");
  wPrintln("Starting stuff...");
  robot.setActorSteering(90);
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
  if(mode == 0) command_mode();
  else if(mode == 1) explore_mode();
  else if(mode == 2) follow_the_light_mode();
}
