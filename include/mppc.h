//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Dec 23 15:56:48 2018 by ROOT version 6.14/04
// from TTree mppc/mppc
// found on file: mppc-muon-sample-0.root
//////////////////////////////////////////////////////////

#ifndef mppc_h
#define mppc_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "TSystem.h"
#include "TH1.h"
#include "TH1I.h"
#include "TDirectory.h"
#include "TF1.h"

#include <iostream>
#include <fstream>

// #include "Configure.h"


using namespace std;
// John Generate TChain file in folder
TChain *Generate_Chain(TChain *Chain = NULL);

class mppcHisto;

class mppc
{
public:
   mppc(TTree *tree = 0); // Construct function
   virtual ~mppc();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);
   virtual Bool_t Notify();
   virtual void Show(Long64_t entry = -1);
   virtual int GetEntries() { return fChain->GetEntries(); }

   // Add different versions of Loop function
   virtual void Loop();

   // private:
   int fReadCounter; //!
   int fEntries;     //!

   // Declaration of leaf types
   UChar_t mac5;
   UShort_t chg[32];
   UInt_t ts0;
   UInt_t ts1;
   UInt_t ts0_ref;
   UInt_t ts1_ref;

   // List of branches
   TBranch *b_mac5;    //!
   TBranch *b_chg;     //!
   TBranch *b_ts0;     //!
   TBranch *b_ts1;     //!
   TBranch *b_ts0_ref; //!
   TBranch *b_ts1_ref; //!

   TTree *fChain;  //!pointer to the analyzed TTree or TChain
   Int_t fCurrent; //!current Tree number in a TChain

};
#endif

