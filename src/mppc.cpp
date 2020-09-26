#include "mppc.h"
using namespace std;

// A function to judge which board should be filled.

TChain *Generate_Chain(TChain *Chain)
{
   gSystem->Exec("ls mppc*.root > .~filelist");

   auto ch = Chain;
   if (ch == NULL)
   {
      ch = new TChain("mppc");
   }

   ifstream file_list(".~filelist");
   for (int i = 0; file_list.is_open() && file_list.eof() == false; i++)
   {
      string s_temp;
      file_list >> s_temp;
      if (s_temp.find(".root") == string::npos)
      {
         continue;
      }
      else if (s_temp.find("mppc") == string::npos)
      {
         continue;
      }
      else if (s_temp.find("Histo") != string::npos)
      {
         continue;
      }

      cout << "File: " << s_temp << " Read" << endl;
      ch->Add(s_temp.c_str());
   }
   cout << "Totally get " << ch->GetEntries() << " Entries" << endl;
   gSystem->Exec("rm .~filelist");
   return ch;
}

mppc::mppc(TTree *tree) : fChain(0)
{

   // Construct fBoardNum & Reset clock counter;
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


void mppc::Loop()
{
   if (fChain == 0)
      return;

}
