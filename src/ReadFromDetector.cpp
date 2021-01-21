#include "ReadFromDetector.h"

bool T0DataReader::ReadOneEvent(T0Data &t0Data)
{
    if (!fin.is_open() || !fin.good())
    {
        return false;
    }

    fin >> t0Data;
    fReadCounter++;
    return true;
}