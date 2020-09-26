#include "FileManager.h"

FileManager::FileManager():
fFile(NULL), fTree(NULL), branch_combined(NULL), branch_primary(NULL), fWritable(false), fReadable(false), combine_temp(NULL), primary_temp(NULL), PrimaryDataSaved(0)
{}

FileManager::~FileManager()
{
    Clear();
}

void FileManager::Clear()
{
    if(combine_temp)
    {
        delete combine_temp;
        combine_temp = NULL;
    }
    if(primary_temp)
    {
        delete primary_temp;
        primary_temp = NULL;
    }
    fWritable = fReadable = false;


    if(!fFile&&!fTree&&!branch_combined&&!branch_primary)
    {
        return;
    }
    if(fFile)
    {
        fFile -> Close();   // If TTree is deleted before file was closed, error will be produced, maybe because there's something in memory that havn't been written to disk.
        fFile -> Clear();
        delete fFile;
        fFile = NULL;
    }

    if(fTree&&fFile)
    {
        // fTree -> Clear();    // It seems that after closing file, fTree will be released.
        // delete fTree;
        fTree = NULL;
        branch_combined = NULL;
        branch_primary = NULL;
    }
    else if(fTree && !fFile)
    {
        delete fTree;
        fTree = NULL;
    }

}

bool FileManager::Initiate(string FileName, Option_t *Option)
{
    Clear();
    if(!TClassTable::GetDict("TCombinedData")) gSystem -> Load("libTest.so");

    combine_temp = new TCombinedData();
    primary_temp = new TPrimaryData();

    bool WriteFlag = 0;
    if(FileName != "")
    {
        fFile = new TFile(FileName.c_str(), Option);
        WriteFlag = fFile -> IsWritable();
    }

    if(WriteFlag)
    {
        fFile -> cd();
        fTree = new TTree("CombinedData", "SiPM data after rearrangement");
        branch_combined = fTree -> Branch("ComDataBranch", "TCombinedData", &combine_temp);
        branch_primary = fTree -> Branch("PriDataBranch", "TPrimaryData", &primary_temp);
        fWritable = true;
        fReadable = false;
        return true;
    }
    else
    {
        // Judge whether fFile is fixed or not
        if(fFile)
        {
            fFile -> cd();
            fTree = (TTree*) fFile -> Get("CombinedData");
            if(!fTree)
            {
                cout << "Seems file doesn't contain data tree, please check." << endl;
                Clear();
                return false;
            }
        }

        // If File name is not input
        if(!fFile)
        {
            vector<string> keyWords{"Detector", ".root"};
            auto ch = JohnGeneral::GenerateChain("CombinedData", keyWords);
            fTree = ch;
            if(ch->GetFileNumber() == 0)
            {
                cout << "Error! Not found any Detector*.root file." << endl;
                Clear();
                return false;
            }
        }  

        branch_combined = fTree -> GetBranch("ComDataBranch");
        branch_primary = fTree -> GetBranch("PriDataBranch");

        if(branch_combined&&branch_primary){
            fWritable = false;
            fReadable = true;
            return true;
        }
        else{
            cout << "Seems tree doesn't contain all branches, please check." << endl;
            Clear();
            return false;
        }
    }
}

bool FileManager::Fill(TCombinedData *&Combinedata)
{
    if(!fWritable) return false;
    Combinedata -> SetDataStartIndex(PrimaryDataSaved);
    // cout << "PrimaryDataSaved: " << PrimaryDataSaved << endl;

    branch_combined -> SetAddress(&Combinedata);
    branch_combined -> Fill();
    // cout << "Branch entries: " << branch_combined -> GetEntries() << endl;
    for(int i = 0; i < Combinedata -> DataNum(); i++)
    {
        auto pritemp = Combinedata->GetData(i);
        Fill(pritemp);
    }

    branch_combined -> SetAddress(&combine_temp);
    return true;
}

bool FileManager::Fill(const CombinedData & Combinedata)
{
    cout << "Temp: Filling Start Index: " << PrimaryDataSaved << endl;
    
    if(!fWritable) return false;
    auto TCDataTemp = new TCombinedData(Combinedata);
    cout << "Temp: " << "TCombinedData PID: " << TProcessID::GetProcessWithUID(TCDataTemp) << endl;
    for(int i = 0; i < Combinedata->Get_Counter();i++)
    {
        auto primaryTemp1 = Combinedata->GetData(i).get();
        auto primaryTemp2 = TCDataTemp->GetData(i);
        cout << "Temp: " << '\t';
        cout << primaryTemp1 << '\t' << primaryTemp2 << '\t' << (primaryTemp2==primaryTemp1) << endl;
        cout << "Temp: " << "TPrimaryData PID: " << TProcessID::GetProcessWithUID(primaryTemp1) << endl;

    }
    bool flag = Fill(TCDataTemp);

    delete TCDataTemp;
    return flag;
}

bool FileManager::Fill(TPrimaryData *&Primarydata)
{
    if(!fWritable) return false;
    branch_primary -> SetAddress(&Primarydata);
    branch_primary -> Fill();
    branch_primary -> SetAddress(&primary_temp);

    PrimaryDataSaved++;
    return true;
}

Int_t FileManager::Write()
{
    if(!fWritable) return -1;
    return fFile -> Write();
}

Int_t FileManager::Write(TDetectorInfo *detector)
{
    if(!detector)
        return -1;
    fFile -> cd();
    return detector -> Write("DetectorInfo");
}

TDetectorInfo* FileManager::GetDetectorInfo()
{
    return dynamic_cast<TDetectorInfo*>(Get("DetectorInfo"));
}


void FileManager::Print()
{
    cout << "fFile: --------------" << endl;
    cout << fFile << endl;
    fFile -> Dump();

    cout << "fTree: --------------" << endl;
    cout << fTree << endl;
    fTree -> Dump();
    
    
}

CombinedData FileManager::GetCombineData(int entry)
{
    if(!fReadable){
        return CombinedData();
    }

    auto data = new TCombinedData();
    branch_combined -> SetAddress(&data);
    branch_combined -> GetEntry(entry);


    // Warninig: !!!!!!!!!!!!!!!!!!!!!
    // If this primary data doesn't fit what stashed inside TCombinedData, then there will be a problem of memory leak
    // Warning ***********************

    for(int i = 0; i < data -> DataNum(); i++)
    {
        auto pri_temp = new TPrimaryData();
        branch_primary -> SetAddress(&pri_temp);
        UInt_t index = data -> StartIndex() + i;
        branch_primary -> GetEntry(index);
    }

    CombinedData combineDataTest(*data);
    delete data;
    return combineDataTest;
}


FileManager *& FileManager::CurrentFileManager()
{
    static auto currentFileManager = new FileManager();
    return currentFileManager;
}