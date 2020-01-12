#include <WiFi.h>
#include <WebServer.h>
#include <TaskScheduler.h>

/* Put your SSID & Password */
const char* ssid = "WinningRobot";  // Enter SSID here
const char* password = "SyntacticSugar";  //Enter Password here

void server_loop();
void main_loop();

Scheduler sched;
Task server_task(0, TASK_FOREVER, &server_loop, &sched);
Task main_task(0, TASK_FOREVER, &main_loop, &sched);

String buffer = "";
int counter =0;

WebServer server(80);

void wPrint(String msg) {
  buffer += String(millis())+"   --->   "+msg;
}

void wPrintln(String msg) {
  wPrint(msg+"<br>");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr+="  <html>\n";
  ptr+="    <head> \n";
  ptr+="        <style> \n";
  ptr+="        </style>\n";
  ptr+="        <script lang=\"JavaScript\">\n";
  ptr+="var lcounter = 0;\n";
  ptr+="            function getStuff() {\n";
  ptr+="                var xhttp = new XMLHttpRequest();\n";
  ptr+="                xhttp.onreadystatechange = function() {\n";
  ptr+="                    if (this.readyState == 4 && this.status == 200) {\n";
  ptr+="                        document.getElementById(\"content\").innerHTML += '<div id = \"'+lcounter+'\">'+this.responseText+'</div>';\n";
  ptr+="document.getElementById(lcounter).scrollIntoView();\n";
  ptr+="lcounter++;\n";
  ptr+="                    }\n";
  ptr+="                };\n";
  ptr+="                xhttp.open(\"GET\", \"/read\", true);\n";
  ptr+="                xhttp.send();\n";
  ptr+="            }\n";
  ptr+="        </script>\n";
  ptr+="    </head>\n";
  ptr+="    <body onload = 'setInterval(getStuff, 500);'> \n";
  ptr+="        <div class='background'></div>\n";
  ptr+="        <div class='content' id=\"content\">\n";
  ptr+="        \n";
  ptr+="        </div>\n";
  ptr+="    </body>\n";
  ptr+="</html>\n";
  return ptr;
}


void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_OnRead() {
  server.send(200, "text/plain", buffer); 
  buffer = "";
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/read", handle_OnRead);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  wPrintln("Starting stuff...");
  server_task.enable();
  main_task.enable();
  sched.startNow();
}

void server_loop() {
  yield();
  server.handleClient();
}

void main_loop() {
  yield();
  // lets put our code here...
}

void loop() {
  yield();
  sched.execute();
}