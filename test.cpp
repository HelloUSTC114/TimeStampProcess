#include "EventLoop.h"
#include "Event.h"

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TBranch.h"

#include "mppc.h"
#include "TClassTable.h"
#include "TSystem.h"

int main()
{
    if (!TClassTable::GetDict("Event"))
    {
        gSystem->Load("lib/libReadFile.so");
    }

    auto file = new TFile("TestTreeWrite.root", "recreate");
    auto tree = new TTree("EventTree", "Event tree test");
    gEventTree = tree;

    auto saveEvent = new Event();
    auto tempEvent = new Event();
    auto branch = tree->Branch("Event", "Event", &saveEvent);
    // tree->AutoSave();

    UInt_t mac5 = 0;
    UShort_t chg[32]{0};
    UInt_t ts0 = 0;
    UInt_t ts1 = ts0, ts0_ref = 1e9, ts1_ref = 1e9;

    Track *trackArray[100][4]{0};

    EventLoop loop(20);
    loop.Print();

    for (int i = 0; i < 100; i++)
    {
        cout << i << '\t' << " Loop" << endl;
        saveEvent->Clear();
        tempEvent->Clear();
        for (int j = 0; j < 4; j++)
        {
            auto tTrack = new Track(mac5++, chg, ts0, ts1, ts0_ref, ts1_ref);
            trackArray[i][j] = tTrack;

            loop.ProcessOneEvent(tempEvent->GenerateFromTrack(tTrack));
            // auto mergeflag = saveEvent->MergeEvent(tempEvent->GenerateFromTrack(tTrack));
            // cout << "Merge flag: " << mergeflag << endl;
        }

        // cout << "Event: " << i << '\t' << *saveEvent << endl;
        // saveEvent->FillEvent();
        // cout << "Test: Finish" << endl
        //      << endl;

        ts0 += 300;
    }

    loop.ForceSaveAll();
    tree->Write();
    file->Close();

    return 0;
}