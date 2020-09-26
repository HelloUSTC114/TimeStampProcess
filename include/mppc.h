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

#include "Configure.h"

#include "TPrimaryData.h"

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
   void DrawAllChannel(string s = "AllChannel.root");
   void DrawAllOrigin(string s = "AllChannel.root");

   bool ConstructOneData(PrimaryDataPtr &, bool &); // bool means whether it's a event or time clock

   // private:
   int fBoardNum;    //!  total Board number
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

   friend class mppcHisto;
};

class mppcHisto
{
public:
   mppcHisto(mppc *);
   ~mppcHisto();

   void AnalyzeQuantile(UChar_t mac5, int group);

private:
   mppc *fMppc;
   TH1D ***fBoardHistArray;
   bool fHistFlag;

   double fQuantiles1[100], fQuantiles2[100];
   static double fProbSum[100];
   int fBoardIndex = 0;
   int fCh1 = 0, fCh2 = 0;

   TH1D *h1_quantile[100];
   TH1D *h2_quantile[100];

   int JudgeQuantile(double number, double *quantiles);

   void FillOrigin();
   void GetQuantile(UChar_t mac5, int group);
   void BuildQuantileHisto();
   void SaveAndPrint(string sFile = "test.root");
};

TDirectory *GetBoardDir(TFile *, UChar_t);
TH1D **CreateNewBoardHist(UChar_t);                                 // For DrawSignal
TH1D **CreateNewBoardHist(UChar_t, string, bool nullDirectory = 0); // For DrawOrigin
#endif

#ifdef mppc_cxx
mppc::mppc(TTree *tree) : fChain(0)
{

   // Construct fBoardNum & Reset clock counter;
   fBoardNum = gConfigure->BoardCounts();
   fReadCounter = 0;
   fEntries = 0;
   if (tree == 0)
   {
      auto chain = new TChain("mppc");
      Generate_Chain(chain);
      tree = chain;
   }
   if (tree)
      Init(tree);
   if (b_mac5)
      fEntries = fChain->GetEntries();
}

mppc::~mppc()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Int_t mppc::GetEntry(Long64_t entry)
{
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}
Long64_t mppc::LoadTree(Long64_t entry)
{
   if (!fChain)
      return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0)
      return centry;
   if (fChain->GetTreeNumber() != fCurrent)
   {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void mppc::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree)
      return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("mac5", &mac5, &b_mac5);
   fChain->SetBranchAddress("chg", chg, &b_chg);
   fChain->SetBranchAddress("ts0", &ts0, &b_ts0);
   fChain->SetBranchAddress("ts1", &ts1, &b_ts1);
   fChain->SetBranchAddress("ts0_ref", &ts0_ref, &b_ts0_ref);
   fChain->SetBranchAddress("ts1_ref", &ts1_ref, &b_ts1_ref);
   Notify();
}

Bool_t mppc::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void mppc::Show(Long64_t entry)
{
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t mppc::Cut(Long64_t entry)
{
   if ((ts0 == 0) || (ts1 == 0))
   {
      return -1;
   }
   return 1;
}

#endif
