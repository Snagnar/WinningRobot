#ifndef DATACOLLECTOR
#define DATACOLLECTOR

#include <Arduino.h>

class DataCollector {
    public:
    DataCollector(int*, int);
    
    void readSensors();
    void cluster();
    byte classify(int);
    void classifyAll();
    int getDataCount();
    int* sensors;
    double borders[2];

    private:
    int* _pins;
    int _pinC;
    int _data[1000];
    int _dataIndex;
};

#endif //DATACOLLECTOR