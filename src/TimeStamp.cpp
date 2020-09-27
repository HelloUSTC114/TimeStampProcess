#include "TimeStamp.h"

ClassImp(TimeStamp)

    double TimeStamp::fHWidth = 100; // set global Half width at 100 ns in default

bool TimeStamp::operator==(const TimeStamp &b) const
{
    if (TMath::Abs((fTimeStamp - fOffset) - (b.fTimeStamp - b.fOffset)) <= fHWidth)
    {
        return true;
    }
    return false;
}

TimeStamp &TimeStamp::operator=(const TimeStamp &b)
{
    if (b.IsValid())
    {
        fTimeStamp = (fTimeStamp - fOffset) * (double)fAvergeCount + (b.fTimeStamp - b.fOffset) * (double)b.fAvergeCount;
        fTimeStamp = fTimeStamp / (double)(fAvergeCount + b.fAvergeCount);

        fAvergeCount += b.fAvergeCount;

        fValid = 1;
        fOffset = 0;
    }
    return *this;
}

void TimeStamp::ClearStamp()
{
    fOffset = 0;
    fTimeStamp = 0;
    fValid = 0;
    fAvergeCount = 0;
}

void TimeStamp::SetTime(int timeStamp, int offset)
{
    fTimeStamp = timeStamp;
    fOffset = offset;
    fValid = 1;
    fAvergeCount = 1;
}