#include "DebugServer.h"

void wPrint(String msg) {
  _DEBUG_BUFFER += msg;
}

void wPrintln(String msg) {
  wPrint(msg+"\n");
}

// WebServer* _server;

WebServer* DebugServer::_server = new WebServer(80);

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
  ptr+="                    let output = document.getElementById('output');\n";
  ptr+="                    output.value+=value+'\\n';\n";
  ptr+="                    output.scrollTop = output.scrollHeight;\n";
  ptr+="                    var xhttp = new XMLHttpRequest();\n";
  ptr+="                    xhttp.onreadystatechange = function() {\n";
  // ptr+="alert('answer: '+this.responseText);\n";
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
  wPrintln("got write request");
  if(_server->hasArg("value")) {
    wPrintln("value is present");
    _process_command(_server->arg("value"));
  }
  wPrintln("done with write");
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
  wPrintln(" >>> got command: "+command+" <<< ");
}