#include "Event.h"
ClassImp(Track);
ClassImp(T0Data);
ClassImp(Event);

TClonesArray *Event::fgTracks = 0;

ostream &operator<<(ostream &os, const Event &a)
{
    return os;
}

void Event::Clear()
{
    fTime.ClearStamp();

    // Clear tracker data
    fTracks.Clear(); // Also call Track::Clear
    fNTracks = 0;

    // Clear T0 Data;
    delete fT0Data;
    fT0Data = NULL;

    fValid = 0;
}

Event::Event(Track *track)
{
    fTracks.Add(track);
    fNTracks = 1;

    fTime = track->fTime;

}