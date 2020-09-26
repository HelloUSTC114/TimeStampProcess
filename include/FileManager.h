/* 
This file contains class FileManager, which manages IO of data of a detector
*/



#ifndef TFILE_MANAGER_H
#define TFILE_MANAGER_H
#include "TObject.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TDirectoryFile.h"
#include "TClassTable.h"
#include "TSystem.h"

#include "TCombinedData.h"
#include "CombineData.h"
#include "TPrimaryData.h"
#include "TDetectorInfo.h"
#include "TChain.h"
#include "General.h"

class TCombinedData;
class CombinedData;
class TDetectorInfo;
class FileManager
{
public:
    FileManager();
    ~FileManager();

    
    bool Initiate(string FileName="Detector.root", Option_t *Option="recreate");    // No matter what the previous file is, close it and create new one. If it's writable, generate a new TTree, TBranch
    void Clear();       // Clear TTree, TBranch, no duty to  write them into file, just close file.

    bool Fill(TCombinedData *& Combinedata);
    bool Fill(const CombinedData & Combinedata);
    bool Fill(TPrimaryData *& Primarydata);


    CombinedData GetCombineData(int);

    virtual Int_t Write();
    virtual Int_t Write(TDetectorInfo *detector);

    bool Writable(){return fWritable;}
    bool Readable(){return fReadable;}

    UInt_t GetEntries(){if(fReadable) return branch_combined->GetEntries(); return 0;}
    TDetectorInfo *GetDetectorInfo();   // Must not be inline function because the use of dynamic cast operator, this means that TDetectorInfo class must be a complete class

    void Print();

    static FileManager *&CurrentFileManager();
    TObject* Get(const char * name){if(!fFile) return NULL; return fFile->Get(name);}

    int GetCurrentIndex() const{return PrimaryDataSaved;}
    TCombinedData * GetCombinedData() const{return combine_temp;}


private:
    TFile* fFile;
    TTree* fTree;
    TBranch * branch_combined;
    TBranch * branch_primary;

    UInt_t PrimaryDataSaved;

    bool fWritable;
    bool fReadable;
    TCombinedData *combine_temp;     // combine data used to initiate branch address
    TPrimaryData *primary_temp;  // primary data used to initiate branch address

};

#define gFileManager (FileManager::CurrentFileManager())

#endif