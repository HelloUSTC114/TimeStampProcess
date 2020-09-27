#include "Loop.h"

int Search_Index_Max = 5;

void LoopIndex::Refresh()
{
    index = index % loop_size;
    index = (index >= 0) ? index : index + loop_size;
}

int LoopIndex::Refresh(int a) const
{
    a = a % loop_size;
    a = (a >= 0) ? a : a + loop_size;
    return a;
}

int LoopIndex::operator+(int a) const
{
    int temp = this->index + a;
    return this->Refresh(temp);
}

int LoopIndex::operator-(int a) const
{
    int temp = this->index - a;
    return this->Refresh(temp);
}

int LoopIndex::operator=(int a)
{
    index = a;
    Refresh();
    return index;
}
bool LoopIndex::operator==(int a) const
{
    a = a % loop_size;
    a = (a >= 0) ? a : a + loop_size;

    return index == a;
}
int LoopIndex::operator++()
{
    index++;
    Refresh();
    return index;
}
int LoopIndex::operator++(int)
{
    int temp = index++;
    Refresh();
    return temp;
}
int LoopIndex::operator+=(int a)
{
    index += a;
    Refresh();
    return index;
}
int LoopIndex::operator--()
{
    index--;
    Refresh();
    return index;
}
int LoopIndex::operator--(int)
{
    int temp = index--;
    Refresh();
    return temp;
}
int LoopIndex::operator-=(int a)
{
    index -= a;
    Refresh();
    return index;
}
