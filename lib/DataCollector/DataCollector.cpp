#include "DataCollector.h"
#include "DebugServer.h"

void kmeans(int* data, int num_data, double centroids[]) {
    wPrintln("kmeans start");
    int min = INT_MAX,
        max = INT_MIN;
    if(!num_data)
        return;
    for(int x = 0; x< num_data; x++) {
        if(data[x]<min) 
            min = data[x];
        if(data[x]>max) 
            max = data[x];
    }
    double dmin = (double) min, dmax = (double) max;
    centroids[0] = dmin;
    centroids[1] = dmin + (dmax - dmin) / 2.6;
    centroids[2] = dmin + (dmax - dmin) * 0.68;

    while(true) {
        double means[3] = {0};
        double counts[3] = {0};
        for(int x = 0; x<num_data; x++) {
            int min_dist = INT_MAX, index = 0;
            for(int y = 0; y<3; y++) {
                int dist = abs(data[x] - centroids[y]);
                if(dist<min_dist) {
                    min_dist = dist;
                    index = y;
                }
            }
            means[index]+=(double)data[x];
            counts[index]++;
        }
        int zeros = 0;
        for(int x = 0; x<3; x++) {
            if(counts[x] == 0 || abs((means[x] / counts[x]) - centroids[x]) == 0) {
                zeros++;
                continue;
            }
            centroids[x] = means[x] / counts[x]; 
        }
        if(zeros == 3)
            break;
    }
    wPrintln("kmeans end");
    // return centroids;
}

int readAnalogValue(byte pin) {
    int sum = 0;
    for(int x = 0; x<10; x++)
        sum+=analogRead(pin);
    return sum / 10;
    // return analogRead(pin);
}

DataCollector::DataCollector(int* pins, int pinCount) {
    for(int x = 0; x<pinCount; x++) {
        pinMode(pins[x], INPUT);
        digitalWrite(pins[x], LOW);
    }
    int values[pinCount] = {0};
    sensors = values;
    _pins = pins;
    _pinC = pinCount;
}

void DataCollector::readSensors() {
    for(int x = 0; x<_pinC; x++) {
        sensors[x] = readAnalogValue(_pins[x]);
        if(_dataIndex < 1000)
            _data[_dataIndex++] = sensors[x];
    }
}

int DataCollector::getDataCount() {
    return _dataIndex + 1;
}

void DataCollector::cluster() {
    if(_dataIndex >=1000)
        return;

    double centers[3];
    kmeans(_data, _dataIndex, centers);

    borders[0] = (centers[0] + centers[1]) / 2.0;
    borders[1] = (centers[1] + centers[2]) / 2.0;
}

void DataCollector::classifyAll() {
    for(int x = 0; x<_pinC; x++) 
        sensors[x] = classify(sensors[x]);
}

byte DataCollector::classify(int value) {
    if(value<borders[0]) return 0;
    if(value<borders[1]) return 1;
    return 2;
}

void DataCollector::clearSensorValues() {
    _dataIndex = 0;
}