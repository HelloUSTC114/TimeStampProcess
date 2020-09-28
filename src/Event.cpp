#include "Event.h"
ClassImp(Track);
ClassImp(T0Data);
ClassImp(Event);

Track::Track(UChar_t mac5, UShort_t chg[32], UInt_t ts0, UInt_t ts1, UInt_t ts0_ref, UInt_t ts1_ref)
{
    fMac5 = mac5;
    memcpy(fChg, chg, sizeof(UShort_t) * 32);
    fTs0 = ts0;
    fTs1 = ts1;
    fTs0_ref = ts0_ref;
    fTs1_ref = ts1_ref;

    fTime.SetTime(ts0, 0);
}

ostream &operator<<(ostream &os, const Event &a)
{
    os << "TimeStam: " << a.fTime.fTimeStamp - a.fTime.fOffset << endl;
    return os;
}

TTree *&Event::GetCurrentTree()
{
    static TTree *currentTree = NULL;
    return currentTree;
}

Event::Event(Track *track)
{
    // Generate Event from a track data, just asign this track to the event, and set t0 data as null pointer.
    fTracks.Add(track);
    fNTracks = 1;
    fTime = track->fTime;
    fValid = 1;
}

Event::Event(T0Data *t0)
{
    fT0Data.Add(t0);
    fNT0Data = 1;
    fTime = t0->fTime;
    fValid = 1;
}

Event &Event::GenerateFromTrack(Track *track)
{
    Clear();
    fTracks.Add(track);
    fNTracks = 1;
    fTime = track->fTime;
    fValid = 1;
    return *this;
}

Event &Event::GenerateFromT0Data(T0Data *t0Data)
{
    Clear();
    fT0Data.Add(t0Data);
    fNT0Data = 1;
    fTime = t0Data->fTime;
    fValid = 1;
    return *this;
}

Event::~Event()
{
    Clear();
}

void Event::Clear()
{
    if (!fValid)
        return;

    fTime.ClearStamp();

    // Clear tracker data
    for (int i = 0; i < fTracks.GetEntries(); i++)
    {
        delete fTracks[i];
    }
    fTracks.Clear(); // Also call Track::Clear
    fNTracks = 0;

    // Clear T0 Data;
    for (int i = 0; i < fT0Data.GetEntries(); i++)
    {
        delete fT0Data[i];
    }
    fT0Data.Clear();
    fNT0Data = 0;

    fValid = 0;
}

void Event::TransferEvent(Event &event, TTree *tree)
{
    FillEvent(tree);
    Clear();

    MergeEvent(event);
}

bool Event::MergeEvent(Event &event)
{
    if (fValid)
    {
        if (!(fTime == event.fTime))
        {
            return false;
        }
    }

    // Merge time stamp, this can be handled by TimeStamp itself
    cout << "fTime: " << fTime.fAvergeCount << '\t' << fTime.fTimeStamp - fTime.fOffset << endl;
    fTime = event.fTime;
    cout << "fTime: " << fTime.fAvergeCount << '\t' << fTime.fTimeStamp - fTime.fOffset << endl;

    // Merge T0
    for (int i = 0; i < event.fT0Data.GetEntries(); i++)
    {
        fT0Data.Add(event.fT0Data[i]);
    }
    fNT0Data += event.fNT0Data;

    fValid |= event.fValid;
    // Merge fTracks
    for (int i = 0; i < event.fTracks.GetEntries(); i++)
    {
        fTracks.Add(event.fTracks[i]);
    }
    fNTracks += event.fNTracks;

    // Clear management of event
    event.fTracks.Clear();
    event.fNTracks = 0;
    event.fT0Data.Clear();
    event.fNT0Data = 0;
    event.fValid = 0;
    event.fTime.ClearStamp();

    return true;
}

Int_t Event::FillEvent(TTree *tree)
{
    if (!fValid)
    {
        return -1;
    }

    if (!gFile->IsWritable())
    {
        return -1;
    }

    if (!tree)
    {
        return -1;
    }

    auto branch = tree->GetBranch("Event");
    if (!branch)
    {
        return -1;
    }

    auto pEvent = (Event *)branch->GetAddress();
    auto tEvent = this;
    branch->SetAddress(&tEvent);
    auto rvalue = tree->Fill();

    branch->SetAddress(&pEvent);
    return rvalue;
}