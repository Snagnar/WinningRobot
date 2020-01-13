#include "DebugServer.h"

void wPrint(String msg) {
  _DEBUG_BUFFER += msg;
}

void wPrintln(String msg) {
  wPrint(msg+"<br>");
}

DebugServer::DebugServer(int port) {
    server = new WebServer(port);
    server->on("/", _handle_onConnect);
    server->on("/read", _handle_onRead);
    server->onNotFound(_handle_onNotFound);
    server->begin();
}

String DebugServer::_getMainPage(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr+="  <html>\n";
  ptr+="    <head> \n";
  ptr+="        <style> \n";
  ptr+=" .content { ";
	ptr+=" font-family: Courier New,Courier,Lucida Sans Typewriter,Lucida Typewriter,monospace; ";
  ptr+=" }";
  ptr+="        </style>\n";
  ptr+="        <script lang=\"JavaScript\">\n";
  ptr+="            var lcounter = 0;\n";
  ptr+="            function getStuff() {\n";
  ptr+="                var xhttp = new XMLHttpRequest();\n";
  ptr+="                xhttp.onreadystatechange = function() {\n";
  ptr+="                    if (this.readyState == 4 && this.status == 200 && this.responseText.length > 0) {\n";
  ptr+="                        var d = new Date();";
  ptr+="                        document.getElementById(\"content\").innerHTML += '<div id = \"'+lcounter+'\">'+d.toLocaleTimeString()+'  -->  '+this.responseText+'</div>';\n";
  ptr+="                        document.getElementById(lcounter).scrollIntoView();\n";
  ptr+="                        lcounter++;\n";
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


void DebugServer::_handle_onConnect() {
  server->send(200, "text/html", _getMainPage()); 
}

void DebugServer::_handle_onRead() {
  if(_DEBUG_BUFFER.indexOf("<br>") >= 0) {
    server->send(200, "text/plain", buffer);
    buffer = "";
  }
  else
    server->send(200, "text/plain", "");
}

void DebugServer::_handle_onNotFound(){
  server->send(404, "text/plain", "Not found");
}

void DebugServer::_loop() {
  yield();
  server->handleClient();
}