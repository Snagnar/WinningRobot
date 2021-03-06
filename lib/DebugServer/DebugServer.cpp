#include "DebugServer.h"

void wPrint(String msg) {
  _DEBUG_BUFFER += msg;
}

void wPrintln(String msg) {
  wPrint(msg+"\n");
}

void splitAt(char separator, const char input[], size_t inputSize, char first[], char second[]) {
  for(int x = 0; x<inputSize; x++) {
    if(input[x] == separator) {
      strncpy(second, &input[x+1], inputSize - x);
      first[x] = '\0';
      return;
    }
    first[x] = input[x];
  }
  first[inputSize] = '\0';
  second[0] = '\0';
}

// WebServer* _server;

WebServer* DebugServer::_server = new WebServer(80);

String DebugServer::_command = "none";
String DebugServer::_argument = "none";

DebugServer::DebugServer(byte port) {
}

void DebugServer::init() {
  _server->on("/", _handle_onConnect);
  _server->on("/read", _handle_onRead);
  _server->on("/write", _handle_onWrite);
  _server->onNotFound(_handle_onNotFound);
  _server->begin();
}

String DebugServer::_getMainPage(){
  // String ptr = "<!DOCTYPE html> <html>\n";
  // ptr+="  <html>\n";
  // ptr+="    <head> \n";
  // ptr+="        <style> \n";
  // ptr+=" .content { ";
	// ptr+=" font-family: Courier New,Courier,Lucida Sans Typewriter,Lucida Typewriter,monospace; ";
  // ptr+=" }";
  // ptr+="        </style>\n";
  // ptr+="        <script lang=\"JavaScript\">\n";
  // ptr+="            var lcounter = 0;\n";
  // ptr+="            function getStuff() {\n";
  // ptr+="                var xhttp = new XMLHttpRequest();\n";
  // ptr+="                xhttp.onreadystatechange = function() {\n";
  // ptr+="                    if (this.readyState == 4 && this.status == 200 && this.responseText.length > 0) {\n";
  // ptr+="                        var d = new Date();";
  // ptr+="                        document.getElementById(\"content\").innerHTML += '<div id = \"'+lcounter+'\">'+d.toLocaleTimeString()+'  -->  '+this.responseText+'</div>';\n";
  // ptr+="                        document.getElementById(lcounter).scrollIntoView();\n";
  // ptr+="                        lcounter++;\n";
  // ptr+="                    }\n";
  // ptr+="                };\n";
  // ptr+="                xhttp.open(\"GET\", \"/read\", true);\n";
  // ptr+="                xhttp.send();\n";
  // ptr+="            }\n";
  // ptr+="        </script>\n";
  // ptr+="    </head>\n";
  // ptr+="    <body onload = 'setInterval(getStuff, 500);'> \n";
  // ptr+="        <div class='background'></div>\n";
  // ptr+="        <div class='content' id=\"content\">\n";
  // ptr+="        \n";
  // ptr+="        </div>\n";
  // ptr+="    </body>\n";
  // ptr+="</html>\n";
  // return ptr;

  String ptr="<!DOCTYPE html> <html>\n";
  ptr+="      <html>\n";
  ptr+="        <head> \n";
  ptr+="            <style> \n";
  ptr+=" .content {\n";
  ptr+=" font-family: Courier New,Courier,Lucida Sans Typewriter,Lucida Typewriter,monospace }\n";
  ptr+=" .background  { position: absolute; top:0; left:0; bottom:0; right:0; z-index: 0; background: #324250;\n";
  ptr+="  height: 100%; width: 100%; }\n";
  ptr+="  .controls {z-index: 2; position: relative}\n";
  ptr+="  .debug {resize: none; width: 95%; height: 800px; z-index: 1; position: relative; top: 30; left: 30 ; bottom: 300; right: 30; background: #ffffff}\n";
  ptr+="    \n";
  ptr+="            </style>\n";
  ptr+="            <script lang=\"JavaScript\">\n";
  ptr+="                var lcounter = 0;\n";
  ptr+="                function getStuff() {\n";
  ptr+="                    var xhttp = new XMLHttpRequest();\n";
  ptr+="                    xhttp.onreadystatechange = function() {\n";
  ptr+="                        if (this.readyState == 4 && this.status == 200 && this.responseText.length > 0) {\n";
  ptr+="                            var d = new Date();\n";
  ptr+="                            let output = document.getElementById('output');\n";
  ptr+=" let line = this.responseText.split('\\n');\n";
ptr+="  this.responseText.split('\\n').forEach(function (item) {\n";
ptr+="    output.value+=''+d.toLocaleTimeString()+'  -->  '+item+'\\n';\n";
ptr+="});\n";
  ptr+="                            output.scrollTop = output.scrollHeight;\n";
  ptr+="                        }\n";
  ptr+="                    };\n";
  ptr+="                    xhttp.open('GET', '/read', true);\n";
  ptr+="                    xhttp.send();\n";
  ptr+="                }\n";
  ptr+="\n";
  ptr+="                function sendStuff() {\n";
  ptr+="                    let value = document.getElementById('sendInput').value;\n";
  ptr+=" document.getElementById('sendInput').value = '';\n";
  ptr+="                    let output = document.getElementById('output');\n";
  ptr+="                    output.value+=value+'\\n';\n";
  ptr+="                    output.scrollTop = output.scrollHeight;\n";
  ptr+="                    var xhttp = new XMLHttpRequest();\n";
  ptr+="                    xhttp.onreadystatechange = function() {\n";
  ptr+="                    };\n";
  ptr+="                    xhttp.open('GET', '/write?value='+value, true);\n";
  ptr+="                    xhttp.send();\n";
  ptr+="                }\n";
  ptr+="            </script>\n";
  ptr+="        </head>\n";
  ptr+="        <body onload = 'setInterval(getStuff, 500);'> \n";
  ptr+="            <div class='background'></div>\n";
  ptr+="            <div class='content' id='content'>\n";
  ptr+="            <textarea class='debug' id='output' readonly></textarea>\n";
  ptr+="            <br><br>\n";
  ptr+="            <div class='controls'>\n";
  ptr+="                <input type='text' id = 'sendInput' class = 'controls'>\n";
  ptr+="                <input type='button' value='send' onclick='sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='clear' onclick='document.getElementById(\"output\").value = \"\"' class = 'controls'> \n";
  ptr+="                <input type='button' value='forward' onclick='document.getElementById(\"sendInput\").value = \"forward\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='backward' onclick='document.getElementById(\"sendInput\").value = \"backward\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='stop' onclick='document.getElementById(\"sendInput\").value = \"stop\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='sensorValues' onclick='document.getElementById(\"sendInput\").value = \"send\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='noSensorValues' onclick='document.getElementById(\"sendInput\").value = \"shut up\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='steer 30 deg' onclick='document.getElementById(\"sendInput\").value = \"steer 30\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='steer 60 deg' onclick='document.getElementById(\"sendInput\").value = \"steer 60\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='steer 90 deg' onclick='document.getElementById(\"sendInput\").value = \"steer 90\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='steer 120 deg' onclick='document.getElementById(\"sendInput\").value = \"steer 120\"; sendStuff();' class = 'controls'> \n";
  ptr+="                <input type='button' value='steer 150 deg' onclick='document.getElementById(\"sendInput\").value = \"steer 150\"; sendStuff();' class = 'controls'> \n";
  ptr+="\n";
  ptr+="            </div>\n";
  ptr+="            </div>\n";
  ptr+="        </body>\n";
  ptr+="    </html>\n";
  return ptr;
}


void DebugServer::_handle_onConnect() {
  _server->send(200, "text/html", _getMainPage()); 
}

void DebugServer::_handle_onRead() {
  if(_DEBUG_BUFFER.indexOf("\n") >= 0) {
    _server->send(200, "text/plain", _DEBUG_BUFFER);
    _DEBUG_BUFFER = "";
  }
  else
    _server->send(200, "text/plain", "");
}

void DebugServer::_handle_onWrite() {
  if(_server->hasArg("value"))
    _process_command(_server->arg("value"));
  _server->send(200, "text/plain", "");
}

void DebugServer::_handle_onNotFound() {
  _server->send(404, "text/plain", "Not found");
}

void DebugServer::loop() {
  yield();
  _server->handleClient();
}

void DebugServer::_process_command(String command) {
  command.trim();
  char commandPart[command.length()];
  char argumentPart[command.length()];
  splitAt(' ', command.c_str(), command.length(), commandPart, argumentPart);
  _command = String(commandPart);
  _argument = String(argumentPart);
  wPrintln("received command: >"+_command+"< and argument: >"+_argument+"<");
}

String DebugServer::getCommand() {
  String result = _command;
  _command = "none";
  return result;
}

String DebugServer::getArgument() {
  return _argument;
}