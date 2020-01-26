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
    void init();
    void loop();
    String getCommand();
    String getArgument();

    private:
    static void _handle_onConnect();
    static void _handle_onRead();
    static void _handle_onWrite();
    static void _handle_onNotFound();
    static void _process_command(String);
    static String _getMainPage();
    static WebServer* _server;
    static String _command;
    static String _argument;
};

#endif //DEBUGSERVER