#ifndef EVENT_H
#define EVENT_H

#include "TObject.h"
#include "TimeStamp.h"
#include "mppc.h"
#include "TClonesArray.h"

// TODO: complete how convert data from mppc class
class Track : public TObject
{
public:
    TimeStamp fTime;

    ClassDef(Track, 1);
};

//TODO: complete this data struct.
class T0Data : public TObject
{
public:
    TimeStamp fTime;

    ClassDef(T0Data, 1);
};

class Event : public TObject
{
public:
    Event() = default;
    Event(Track *track); // Construct event data directly from track data; Event manages this track data, which means this track should be deleted if this event is deleted;

    Event(T0Data *t0);
    Event(Event &) = delete;

    TimeStamp fTime;   // time data
    TObjArray fTracks; // Tracker data array
    T0Data *fT0Data;   //-> T0 data;

    int fNTracks = 0; // tracker count
    bool fValid = 0;  // if have at least one tracker data and one T0Data, return as 1;

    virtual bool operator==(const Event &b) const { return fTime == b.fTime; }
    virtual void Print() const { return; }

    virtual void Clear(); // Clear this event data

    virtual void TransferEvent(const Event &event); // Clear this event first, and then transfer pointer manager from event to this Event, and clear pointer of event.

    virtual bool MergeEvent(const Event &event); // Merge two events, usually a main event on event loop and a free event temp data. But will consider all kinds of situation, such as two valid events

    ClassDef(Event, 1);
};

ostream &operator<<(ostream &os, const Event &a);

#endif