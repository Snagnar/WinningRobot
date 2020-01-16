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
    static void _handle_onConnect();
    static void _handle_onRead();
    static void _handle_onNotFound();
    static String _getMainPage();
    static WebServer* _server;
};

#endif //DEBUGSERVER