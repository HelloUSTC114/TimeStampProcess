#ifndef EVENT_H
#define EVENT_H

#include "TObject.h"
#include "TimeStamp.h"
#include "mppc.h"
#include "TClonesArray.h"

#include <iostream>

// TODO: complete how to convert data from mppc class
class Track : public TObject
{
public:
    TimeStamp fTime;

    Track() = default;
    Track(UChar_t mac5, UShort_t chg[32], UInt_t ts0, UInt_t ts1, UInt_t ts0_ref, UInt_t ts1_ref);

    void SetStampOffset(Int_t offset);
    void Clear();

    UChar_t fMac5;
    UShort_t fChg[32];
    UInt_t fTs0;
    UInt_t fTs1;
    UInt_t fTs0_ref;
    UInt_t fTs1_ref;

    ClassDef(Track, 1);
};

//TODO: complete this data struct.
class T0Data : public TObject
{
public:
    T0Data() = default;
    TimeStamp fTime;

    void SetStampOffset(Int_t offset);
    void Clear();

    ClassDef(T0Data, 1);
};

#define gEventTree (Event::GetCurrentTree())
class Event : public TObject
{
public:
    Event() = default;   // Default constructor
    ~Event();            // before destruction, this event should be filled into tree
    Event(Track *track); // Construct event data directly from track data; Event manages this track data, which means this track should be deleted if this event is deleted;

    Event(T0Data *t0); // Generate Event from a t0 data
    Event(Event &) = delete;

    Event &GenerateFromTrack(Track *track);    // Force Generate from track, instead of construct from track, to save time of construction. Careful about losing data, in case that it already contains data, this method will clear all data without saving
    Event &GenerateFromT0Data(T0Data *t0Data); // save time of construction

    TimeStamp fTime;   // time data
    TObjArray fTracks; // Tracker data array
    TObjArray fT0Data; // T0 data array
    // T0Data *fT0Data = NULL; //-> T0 data;

    int fNTracks = 0; // tracker count
    int fNT0Data = 0; // T0 Data count
    bool fValid = 0;  // if have at least one tracker data or one T0Data, return as 1;

    virtual bool operator==(const Event &b) const { return fTime == b.fTime; }
    virtual void Print() const { return; }

    virtual void Clear(); // Clear this event data

    virtual Int_t FillEvent(TTree *tree = gEventTree);                  // Fill this event to a tree
    virtual void TransferEvent(Event &event, TTree *tree = gEventTree); // Save & Clear this event first, transfer pointer from the second event, and then clear pointer management of the second event.

    virtual bool MergeEvent(Event &event); // If time stamps are the same, then merge two events and transfer pointer management; if not, do not change anything. Usually a main event on event loop and a free event temp data. But will consider all kinds of situation, such as two valid events

    static TTree *&GetCurrentTree(); // Default saved tree;
    ClassDef(Event, 1);
};

ostream &operator<<(ostream &os, const Event &a);

#ifdef __ROOTCLING__
#pragma link C++ class TimeData;
#pragma link C++ class TimeStamp;
#pragma link C++ class Track;
#pragma link C++ class T0Data;
#pragma link C++ class Event;
#pragma link C++ off all function;
#pragma link C++ off all global;
// #pragma link off all classes;
#endif

#endif