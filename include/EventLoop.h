#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Loop.h"
#include "Event.h"
using namespace std;

class EventLoop : public Loop<Event>
{
public:
    EventLoop(int loopSize);
    ~EventLoop();
    virtual int Add(Event &value) override;

    int ProcessOneEvent(Event &value);

    int ForceSaveAll();

    // virtual int ProcessTimeData(const TimeData &a);

    // bool SearchTimeData(const TimeStamp &a, int &index) const;

protected:
    // void Reset();   // without saving any data
};

#endif