#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "RtypesCore.h"
#include "TMath.h"
#include "TObject.h"

#define gHWidth (TimeStamp::GlobalHWidth())

class TimeStamp : public TObject
{
public:
    bool operator==(const TimeStamp &) const;
    TimeStamp &operator=(const TimeStamp &);

    double &GlobalHWidth() { return fHWidth; }

    void SetTimeStamp(int timestamp) { fTimeStamp = timestamp; }
    void SetOffset(int offset) { fOffset = offset; }
    void SetTime(int timestamp, int offset);

    void ClearStamp();
    bool IsValid() const { return fValid; }

    // private:
    double fTimeStamp;
    double fOffset;

    bool fValid = 0;
    int fAvergeCount = 0; // Counter for average numbers;

    static double fHWidth; // global Half width for time stamp.

    ClassDef(TimeStamp, 1)
};

#endif