#define mppc_cxx
#include "mppc.h"
using namespace std;


// A function to judge which board should be filled.





TChain * Generate_Chain(TChain *Chain)
{
   gSystem -> Exec("ls mppc*.root > .~filelist");

   auto ch = Chain;    
   if(ch == NULL)
   {
      ch = new TChain("mppc");
   }

   ifstream file_list(".~filelist");
   for(int i = 0; file_list . is_open() && file_list . eof() == false; i++)
   {
      string s_temp;
      file_list >> s_temp;
      if(s_temp.find(".root") == string::npos)
      {
         continue;
      }
      else if(s_temp . find("mppc") == string::npos)
      {
         continue;
      }
      else if(s_temp . find("Histo") != string::npos)
      {
         continue;
      }
      
      cout << "File: " << s_temp << " Read" << endl;
      ch -> Add(s_temp . c_str());

    }
    cout << "Totally get " << ch -> GetEntries() << " Entries" << endl;
    gSystem -> Exec("rm .~filelist");
    return ch;

}

void mppc::Loop()
{
   if (fChain == 0) return;


   // Record clock reset times to align data
   vector<int> Array_Clock_Reset;
   for(int i = 0; i < gConfigure->BoardCounts(); i++)
   {
      Array_Clock_Reset . push_back(0);
   }


   
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) 
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      // Judge board mac number;
      int board_no = gConfigure->GetBoardIndexByMac(mac5);
      if(board_no == -1)
         continue;
      
      // Record clock reset time
      if (Cut(ientry) < 0)
      {
         Array_Clock_Reset[board_no] ++;
         // Show();
         // cout << "clock reset time: " << Array_Clock_Reset[board_no] << endl;
         continue;
      }
      // Construct a pre primary data
      int reset_time = Array_Clock_Reset[board_no];
      PrimaryDataPtr data_temp(mac5, chg, ts0, ts1, ts0_ref, ts1_ref, reset_time, 0);
   }
}

bool mppc::ConstructOneData(PrimaryDataPtr &DataTemp, bool &ConstructFlag)
{
   if(fReadCounter >= fEntries){
      cout << "Has read all entries, please quit loop" << endl;
      ConstructFlag = 0;
      return false;
   }
   int nb = GetEntry(fReadCounter);
   fReadCounter++;
   if(nb < 0) return false;

   int index = gConfigure -> GetBoardIndexByMac(mac5);
   if(!ts0||!ts1){
      // cout << "Temp. Clock reset signal." << endl;
      ConstructFlag = false;
      gConfigure->IncrementClockByIndex(index);
      return true;
   }

   // cout << "mac5: " << (int)mac5 << '\t' <<ts0 << endl;
   DataTemp = PrimaryDataPtr(mac5, chg, ts0, ts1, ts0_ref, ts1_ref, gConfigure->GetClockByIndex(index), 0);
   ConstructFlag = true;
   return true;
}

TDirectory* GetBoardDir(TFile* file, UChar_t MAC)
{
   if(!file->IsWritable()) return NULL;
   file -> cd();

   string DirName = "Board" + to_string((int)MAC);
   auto DirTemp = file->GetDirectory(DirName.c_str());
   if(DirTemp) return DirTemp;

   DirTemp = file -> mkdir(DirName.c_str());
   return DirTemp;
}

TH1D** CreateNewBoardHist(UChar_t MAC)
{
   string NamePre = "h";
   string NameSuf = "Board" + to_string(MAC);
   
   TH1D** HistArray = new TH1D*[64];
   for(int i = 0; i < 32; i++)
   {
      string name = NamePre + "Ped" + to_string(i) + NameSuf;
      HistArray[i] = new TH1D(name.c_str(), name.c_str(), 4096, 0, 4096);
      name = NamePre + "Signal" + to_string(i) + NameSuf;
      HistArray[i+32] = new TH1D(name.c_str(), name.c_str(), 4096, 0, 4096);
   }
   return HistArray;
}

TH1D **CreateNewBoardHist(UChar_t MAC, string sHisto, bool nullDirectory)
{
   string NamePre = "h";
   string NameSuf = "Board" + to_string(MAC);

   TH1D **HistArray = new TH1D *[64];
   for (int i = 0; i < 32; i++)
   {
      string name = NamePre + sHisto + to_string(i) + NameSuf;
      HistArray[i] = new TH1D(name.c_str(), name.c_str(), 4096, 0, 4096);
      if(nullDirectory)
      {
         HistArray[i] -> SetDirectory(0);
      }
   }
   return HistArray;
}

// Have problem of memory leak
void mppc::DrawAllChannel(string s)
{
   auto file = new TFile(s.c_str(), "recreate");
   file -> cd();

   auto boardNum = gConfigure -> BoardCounts();
   TH1D*** BoardHistArray = new TH1D**[boardNum];
   TDirectory** BoardDirArray = new TDirectory*[boardNum];

   vector<double> Cut;

   for(int i = 0; i < boardNum; i++)
   {
      auto macTemp = gConfigure -> GetMac(i);   // Get Mac from gConfigure
      BoardHistArray[i] = CreateNewBoardHist(macTemp);
      BoardDirArray[i] = GetBoardDir(file, macTemp);
   }

   Long64_t nentries = fChain -> GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   // First loop aims at getting pedestal spectrum
   for(Long64_t jentry = 0; jentry < nentries; jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if(jentry%10000==0){cout << "Processing: " << jentry << '\t' << ientry << endl;}
      if(ientry < 0) break;
      nb = fChain -> GetEntry(jentry);
      if(nb<0)continue;
      nbytes += nb;


      // if(ts0) continue;

      int BoardIndex = gConfigure -> GetBoardIndexByMac(mac5);
      if (BoardIndex < 0)
      {
         cerr << "Error! Cannot find board, please check configuration file." << endl;
         cerr << "Board Mac5: " << (int)mac5 << endl;
         range_error err("Board Configuration Error");
         throw err;
      }

      auto HistArray = BoardHistArray[BoardIndex];
      for(int i = 0; i < 32; i++)
      {
         if(ts0==0)
            HistArray[i] -> Fill(chg[i]);
         else if(ts0!=0)
         {
            HistArray[i+32]->Fill(chg[i]);
         }
      }
   }

   // After getting pedestal spetrum, it should be fitted using gaus function, and cut signal spetrum with mean plus three sigma
   auto GausFitFun = new TF1("fit", "gaus", 0, 4096);
   for(int i = 0; i < boardNum; i++)
   {
      auto HistArray = BoardHistArray[i];
      for(int j = 0; j < 32; j++)
      {
         HistArray[j] -> Fit(GausFitFun, "QR", "", 0, 1000);
         auto cutTemp = GausFitFun->GetParameter(1) + 5 * GausFitFun -> GetParameter(2);
         Cut.push_back(cutTemp);
      }
   }

   // // second loop to get spetrucm using cut condition
   // nbytes = 0;
   // nb = 0;

   // for(Long64_t jentry = 0; jentry < nentries; jentry++)
   // {
   //    if(jentry%10000==0){cout << "Processing: " << jentry << endl;}
   //    Long64_t ientry = LoadTree(jentry);
   //    if(ientry < 0) break;
   //    nb = fChain -> GetEntry(jentry);
   //    if(nb<0)continue;
   //    nbytes += nb;


   //    if(!ts0)  continue;
   //    int boardIndex = gConfigure->GetBoardIndexByMac(mac5);
   //    auto HistArray = BoardHistArray[boardIndex];
   //    for(int i = 0; i < 32; i++)
   //    {
   //       if(chg[i]<=Cut[boardIndex*32+i]) continue;
   //       HistArray[i+32]->Fill(chg[i]);
   //    }
   // }

   // Save all board info
   for(int i = 0; i < boardNum; i++)
   {
      UChar_t mac = gConfigure -> GetMac(i);
      auto dir = GetBoardDir(file, mac);
      auto histArray = BoardHistArray[i];
      for(int i = 0; i < 32; i++)
      {
         dir -> cd();
         histArray[i] -> Write();
         histArray[i+32] -> Write();
         histArray[i] -> SetDirectory(dir);
         histArray[i+32] -> SetDirectory(dir);
      }
   }


   delete GausFitFun;
   GausFitFun = NULL;
   file -> Close();
   delete file;
   file = NULL;

}

// Have problem of memory leak
void mppc::DrawAllOrigin(string s)
{
   auto file = new TFile(s.c_str(), "recreate");
   file->cd();

   auto boardNum = gConfigure->BoardCounts();
   TH1D ***BoardHistArray = new TH1D **[boardNum];
   TDirectory **BoardDirArray = new TDirectory *[boardNum];

   vector<double> Cut;

   for (int i = 0; i < boardNum; i++)
   {
      auto macTemp = gConfigure->GetMac(i); // Get Mac from gConfigure
      BoardHistArray[i] = CreateNewBoardHist(macTemp, "Origin");
      BoardDirArray[i] = GetBoardDir(file, macTemp);
   }

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   // First loop aims at getting pedestal spectrum
   for (Long64_t jentry = 0; jentry < nentries; jentry++)
   {
      if(jentry%10000==0) cout << "Processing: " << jentry << endl;

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0)
         break;
      nb = fChain->GetEntry(jentry);
      if(nb < 0) continue;
      nbytes += nb;

      if (!ts0)
         continue;

      int BoardIndex = gConfigure->GetBoardIndexByMac(mac5);
      if(BoardIndex < 0)
      {
         cerr << "Error! Cannot find board, please check configuration file." << endl;
         cerr << "Board Mac5: " << (int)mac5 << endl;
         range_error err("Board Configuration Error");
         throw err;
      }
      auto HistArray = BoardHistArray[BoardIndex];
      for (int i = 0; i < 32; i++)
      {
         HistArray[i]->Fill(chg[i]);
      }
   }

   // Save all board info
   for (int i = 0; i < boardNum; i++)
   {
      UChar_t mac = gConfigure->GetMac(i);
      auto dir = GetBoardDir(file, mac);
      auto histArray = BoardHistArray[i];
      for (int i = 0; i < 32; i++)
      {
         dir->cd();
         histArray[i]->Write();
         histArray[i] -> SetDirectory(dir);
      }
   }

   file->Close();
   delete file;
   file = NULL;
}

mppcHisto::mppcHisto(mppc* Main):
fMppc(Main), fHistFlag(0), fBoardHistArray(NULL)
{
   fBoardHistArray = new TH1D**[fMppc->fBoardNum];
   for(int i = 0; i < fMppc->fBoardNum; i++)
   {
      auto mac = gConfigure -> GetMac(i);
      fBoardHistArray[i] = CreateNewBoardHist(mac, "Origin", 1);
      // cout << fBoardHistArray[i] << endl;
      // cout << fBoardHistArray[i][0]->GetName() << endl;
      // cout << fBoardHistArray[i][0]->GetDirectory() << endl;
   }
   fHistFlag = 1;
}

mppcHisto::~mppcHisto()
{
   if(!fHistFlag) return;
   for(int i = 0; i < fMppc->fBoardNum; i++)
   {
      for(int j = 0; j < 32; j++)
      {
         fBoardHistArray[i][j] -> Delete();
         fBoardHistArray[i][j] = NULL;
      }
      delete[] fBoardHistArray[i];
      fBoardHistArray[i] = NULL;
   }
   delete[] fBoardHistArray;
}

void mppcHisto::FillOrigin()
{
   long nentries = fMppc -> fChain -> GetEntriesFast();
   // for (int jentry = 0; jentry < 10000; jentry++)
   for(int jentry = 0; jentry < nentries; jentry++)
   {
      long ientry = fMppc -> LoadTree(jentry);
      if(ientry < 0)
         break;
      int nb = fMppc->fChain -> GetEntry(jentry);
      if(!fMppc -> ts0)
         continue;

      int BoardIndex = gConfigure -> GetBoardIndexByMac(fMppc->mac5);
      if(BoardIndex < 0)
      {
         cerr << "Error! Cannot find board, please check configuration file." << endl;
         cerr << "Board Mac5: " << (int)fMppc->mac5 << endl;
         range_error err("Board Configuration Error");
         throw err;
      }
      auto HistArray = fBoardHistArray[BoardIndex];
      for(int i = 0; i < 32; i++)
      {
         HistArray[i] -> Fill(fMppc -> chg[i]);
      }
   }
}

double mppcHisto::fProbSum[100]{0};

void mppcHisto::GetQuantile(UChar_t mac5, int group)
{
   fBoardIndex = gConfigure -> GetBoardIndexByMac(mac5);
   fCh1 = 2 * group;
   fCh2 = 2 * group + 1;

   auto HistArray = fBoardHistArray[fBoardIndex];
   for(int i = 0; i < 100; i++)
   {
      fProbSum[i] = (double)i / 100;
   }
   auto h1 = HistArray[fCh1];
   auto h2 = HistArray[fCh2];
   h1 -> GetQuantiles(100, fQuantiles1, fProbSum);
   h2 -> GetQuantiles(100, fQuantiles2, fProbSum);

   cout << "----------------ch1---------------" << endl;
   cout << "Ch1 information: " << fCh1 << endl;
   cout << "ch1 Quantiles: " << endl;
   cout << "Sum\tQuantile" << endl;
   for (int i = 0; i < 100; i++)
   {
      cout << (double)i / 100 << "\t" << fQuantiles1[i] << endl;
   }

   cout << "----------------ch2---------------" << endl;
   cout << "Ch2 information: " << fCh2 << endl;
   cout << "ch2 Quantiles: " << endl;
   cout << "Sum\tQuantile" << endl;
   for (int i = 0; i < 100; i++)
   {
      cout << (double)i / 100 << "\t" << fQuantiles2[i] << endl;
   }
}

void mppcHisto::BuildQuantileHisto()
{
   string sHist1 = (string) "Histo";
   string sHist2 = (string) "Histo";
   string sChannel1 = to_string(fCh1);
   string sChannel2 = to_string(fCh2);
   sHist1 += sChannel2;
   sHist2 += sChannel1;

   for (int i = 0; i < 100; i++)
   {
      string sHistName1 = sHist1 + "Ch" + sChannel1 + "Quantile" + to_string(i);
      string sHistName2 = sHist2 + "Ch" + sChannel2 + "Quantile" + to_string(i);
      h1_quantile[i] = new TH1D(sHistName1.c_str(), sHistName1.c_str(), 4096, 0, 4096);
      h2_quantile[i] = new TH1D(sHistName2.c_str(), sHistName2.c_str(), 4096, 0, 4096);
   }
   
   long nentries = fMppc->fChain->GetEntriesFast();
   // for (int jentry = 0; jentry < 10000; jentry++)
   for (int jentry = 0; jentry < nentries; jentry++)
   {
      long ientry = fMppc->LoadTree(jentry);
      if (ientry < 0)
         break;
      int nb = fMppc->fChain->GetEntry(jentry);
      if (!fMppc->ts0)
         continue;

      int BoardIndex = gConfigure->GetBoardIndexByMac(fMppc->mac5);
      if(fBoardIndex != BoardIndex)
         continue;

      // cout << "John: Test " << jentry << endl;
      int number1 = JudgeQuantile(fMppc->chg[fCh1], fQuantiles1); // Judge quantile of ch1, than fill ch2 value to h1_quantile
      int number2 = JudgeQuantile(fMppc->chg[fCh2], fQuantiles2);

      // cout << "number1: " << number1 << endl;
      // cout << "number2: " << number2 << endl;
      h1_quantile[number1]->Fill(fMppc->chg[fCh2]);
      h2_quantile[number2]->Fill(fMppc->chg[fCh1]);
   }
}

int mppcHisto::JudgeQuantile(double number, double *quantiles)
{
   for(int i = 0; i < 100; i++)
   {
      if(number < quantiles[i + 1])
         return i;
   }
   return 99;
}

void mppcHisto::SaveAndPrint(string sFile)
{
   ofstream fout1("quantiles1.txt");
   ofstream fout2("quantiles2.txt");
   auto file = new TFile(sFile.c_str(), "recreate");
   auto dir1 = file -> mkdir("ch1");
   auto dir2 = file -> mkdir("ch2");

   for(int i = 0; i < 100; i++)
   {
      h1_quantile[i] -> SetDirectory(dir1);
      h2_quantile[i] -> SetDirectory(dir2);
      dir1 -> cd();
      h1_quantile[i] -> Write();
      dir2 -> cd();
      h2_quantile[i] -> Write();

   }
   file -> Close();

   cout << "----------------ch1---------------" << endl;
   cout << "Ch1 information: " << fCh1 << endl;
   cout << "ch1 Quantiles: " << endl;
   cout << "Sum\tQuantile" << endl;
   for (int i = 0; i < 100; i++)
   {
      cout << (double)i / 100 << "\t" << fQuantiles1[i] << endl;
      fout1 << (double)i / 100 << "\t" << fQuantiles1[i] << endl;
   }

   cout << "----------------ch2---------------" << endl;
   cout << "Ch2 information: " << fCh2 << endl;
   cout << "ch2 Quantiles: " << endl;
   cout << "Sum\tQuantile" << endl;
   for (int i = 0; i < 100; i++)
   {
      cout << (double)i / 100 << "\t" << fQuantiles2[i] << endl;
      fout2 << (double)i / 100 << "\t" << fQuantiles2[i] << endl;
   }
   fout1.close();
   fout2.close();
}

void mppcHisto::AnalyzeQuantile(UChar_t mac5, int group)
{
   FillOrigin();
   GetQuantile(mac5, group);
   BuildQuantileHisto();
   SaveAndPrint();
}