#ifndef READFROMDETECTOR_H
#define READFROMDETECTOR_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "Event.h"

class T0DataReader
{
public:
    T0DataReader(std::string filename);

    bool ReadOneEvent(T0Data &t0Data);

private:
    std::ifstream fin;
    int fReadCounter = 0;

};

#endif