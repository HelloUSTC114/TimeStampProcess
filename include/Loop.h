#ifndef LOOP_H
#define LOOP_H
#include <iostream>
using namespace std;

extern int Search_Index_Max;

// This file defines Loop index class and loop template class

class LoopIndex
{
public:
    LoopIndex(int Loop_Size, int Index = 0) : loop_size(Loop_Size), index(Index){};
    void Refresh();
    int Refresh(int a) const;

    int Index() const { return index; }

    // Define Operations of loop index, which acts as loop
    int operator=(int a);
    int operator+(int a) const;
    int operator-(int a) const;

    bool operator==(int a) const;
    int operator++();
    int operator++(int);
    int operator+=(int a);
    int operator--();
    int operator--(int);
    int operator-=(int a);
    int operator()() const { return Index(); }

    int GetLoopSize() const { return loop_size; }

private:
    int loop_size;
    int index = 0;
};

template <typename T>
class Loop
{
public:
    Loop(int Loop_Size);
    ~Loop();

    virtual int Add(const T &value);                                                 // Copy value to loop space, return array index
    virtual const T &operator[](LoopIndex Index) const { return loop_ptr[Index()]; } // Access element by real event index
    virtual const T &operator[](int index) const { return loop_ptr[index]; }         // Access element by array index

    virtual int Index() const { return fLoopIndex(); }
    virtual bool Is_Full() const { return full_flag; }

    // Search Element. Do not copy element. Constant functions.
    virtual int Search(const T &a) const;
    virtual bool Search(const T &a, int &index) const;
    virtual bool Search(const T &a, int *index, int &number) const; // Index is the last _th event.

    virtual bool Search(const T &a, int *index, int &number, bool (*funptr)(const T &, const T &)) const; // Index is the last _th event.
    virtual bool Search(const T &a, int &index, bool (*funptr)(const T &, const T &)) const;              // Serarch using lambda expression

    virtual const T &Get_Last_Event(int a) const;
    virtual int Get_Last_Event_Index(int a) const;

    virtual const T &Get_First_Event(int a) const;
    virtual int Get_First_Event_Index(int a) const;

    virtual void Print() const;
    virtual void Show(int Event_Index) const
    {
        cout << "Event:\t" << Event_Index << "\t" << Get_Last_Event(Event_Index) << endl;
    };

protected:
    int loop_size = 0;

    T *loop_ptr = 0;
    bool *flag_ptr = 0;

    bool full_flag = 0;
    int event_num = 0; // Record event number in the loop, in case there's some problem when destruct element

    bool Refresh_Full_Flag();

    LoopIndex fLoopIndex; // The next position to add a new data
};
// #endif

// #ifdef LOOP_CXX

template <typename T>
Loop<T>::Loop(int Loop_Size) : loop_size(Loop_Size),
                               fLoopIndex(loop_size)
{
    loop_ptr = new T[loop_size];
    flag_ptr = new bool[loop_size];
    for (int i = 0; i < loop_size; i++)
    {
        flag_ptr[i] = false;
    }
}

template <typename T>
Loop<T>::~Loop()
{
    delete[] loop_ptr;
    delete[] flag_ptr;

    loop_ptr = NULL;
    flag_ptr = NULL;
}

template <typename T>
int Loop<T>::Add(const T &value) // Copy value to Loop space
{
    int index = fLoopIndex();
    loop_ptr[index] = value;
    flag_ptr[index] = true;

    fLoopIndex++;

    if (event_num < loop_size)
    {
        event_num++;
    }

    Refresh_Full_Flag();
    return index;
}

template <typename T>
bool Loop<T>::Refresh_Full_Flag()
{
    int index = fLoopIndex();

    if (full_flag == 0)
    {
        for (int i = 0; i < loop_size; i++)
        {
            if (flag_ptr[index] == 0)
                return false;
        }
        full_flag = true;
        return true;
    }
    return true;
}

template <typename T>
void Loop<T>::Print() const
{
    for (int i = 0; i < event_num; i++)
    {
        cout << "Event Index: " << i;
        cout << " Loop Index: " << fLoopIndex - (i + 1) << endl;

        cout << Get_Last_Event(i) << endl;
    }
}

template <typename T>
const T &Loop<T>::Get_Last_Event(int a) const
{
    if (a >= loop_size)
    {
        cout << "Warning: Out of loop" << endl;
        cout << "Loop size: " << loop_size << endl;
    }

    int index_temp = Get_Last_Event_Index(a);

    if (a >= event_num)
    {
        cout << "Warning: Out of Event numbers" << endl;
        cout << "Event numbers: " << event_num << endl;
    }

    return loop_ptr[index_temp];
}

template <typename T>
int Loop<T>::Get_Last_Event_Index(int a) const
{
    return fLoopIndex - (a + 1);
}

template <typename T>
int Loop<T>::Get_First_Event_Index(int a) const
{
    if (full_flag)
    {
        return fLoopIndex + a;
    }
    return a;
}

template <typename T>
const T &Loop<T>::Get_First_Event(int a) const
{

    if (a >= loop_size)
    {
        cout << "Warning: Out of loop" << endl;
        cout << "Loop size: " << loop_size << endl;
    }

    int index_temp = Get_First_Event_Index(a);

    if (a >= event_num)
    {
        cout << "Warning: Out of Event numbers" << endl;
        cout << "Event numbers: " << event_num << endl;
    }

    return loop_ptr[index_temp];
}

template <typename T>
int Loop<T>::Search(const T &a) const
{
    int temp;
    auto flag = Search(a, temp);

    return temp;
}

template <typename T>
bool Loop<T>::Search(const T &a, int &index) const
{
    for (int i = 0; i < event_num; i++)
    {
        if (Get_Last_Event(i) == a)
        {
            index = i;
            return true;
        }
    }
    index = -1;
    return false;
}

template <typename T>
bool Loop<T>::Search(const T &a, int *index, int &number) const
{
    int save_index = 0;

    for (int i = 0; i < event_num; i++)
    {
        if (save_index >= Search_Index_Max)
            break;

        if (Get_Last_Event(i) == a)
        {
            index[save_index] = i;
            save_index++;
        }
    }
    number = save_index;

    if (save_index == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

template <typename T>
bool Loop<T>::Search(const T &a, int *index, int &number, bool (*funptr)(const T &, const T &)) const
{
    int save_index = 0;

    for (int i = 0; i < event_num; i++)
    {
        if (save_index >= Search_Index_Max)
            break;

        if (funptr(a, Get_Last_Event(i)))
        {
            index[save_index] = i;
            save_index++;
        }
    }
    number = save_index;

    if (save_index == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

template <typename T>
bool Loop<T>::Search(const T &a, int &index, bool (*funptr)(const T &, const T &)) const
{
    for (int i = 0; i < event_num; i++)
    {
        if (funptr(a, Get_Last_Event(i)))
        {
            index = i;
            return true;
        }
    }
    index = -1;
    return false;
}

#endif