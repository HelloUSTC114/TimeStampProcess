#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Loop.h"
#include "Event.h"
using namespace std;

class EventLoop : public Loop<Event>
{
public:
    EventLoop(int loopSize);
    virtual int Add(const Event &value) override;

protected:
    int fLoopSize = 0;
};

#endif