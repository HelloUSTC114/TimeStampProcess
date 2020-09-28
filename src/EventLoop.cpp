#include "EventLoop.h"

EventLoop::EventLoop(int loopSize) : Loop<Event>(loopSize)
{
}

int EventLoop::Add(Event &value)
{
    int index = fLoopIndex();
    loop_ptr[index].Clear();
    loop_ptr[index].TransferEvent(value);

    flag_ptr[index] = true;

    fLoopIndex++;

    if (event_num < loop_size)
    {
        event_num++;
    }

    Refresh_Full_Flag();
    return index;
}

int EventLoop::ProcessOneEvent(Event &value)
{
    int index = 0;
    Search(value, index);
    if (index < 0)
    {
        loop_ptr[fLoopIndex()].FillEvent();
        Add(value);
        return 0;
    }
    loop_ptr[Get_Last_Event_Index(index)].MergeEvent(value);
}

EventLoop::~EventLoop()
{
    // ForceSaveAll();
    // Loop<Event>::~Loop();
}

int EventLoop::ForceSaveAll()
{
    for (int i = 0; i < event_num; i++)
    {
        loop_ptr[Get_First_Event_Index(i)].FillEvent();
    }
    return event_num;
}

// void EventLoop::Reset()
// {

// }

// bool EventLoop::SearchTimeData(const TimeStamp &a, int &index) const
// {
//     auto lambda = [](const Event &event, const TimeStamp &timeStamp) -> bool {
//         return event.fTime == timeStamp;
//     };

//     return Search<TimeStamp>(a, index, lambda);
// }