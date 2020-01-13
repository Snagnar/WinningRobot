#ifndef DEBUGSERVER
#define DEBUGSERVER

#include <Arduino.h>
#include <WebServer.h>

static String _DEBUG_BUFFER = "";

void wPrint(String);
void wPrintln(String);

class DebugServer {
    public:
    DebugServer(byte);
    void loop();

    private:
    void _handle_onConnect();
    void _handle_onRead();
    void _handle_onNotFound();
    String _getMainPage();
    WebServer* _server;
};

#endif //DEBUGSERVER