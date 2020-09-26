#include "EventLoop.h"

int EventLoop::Add(const Event &value)
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