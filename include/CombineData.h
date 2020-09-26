
#ifndef COMBINE_DATA_H
#define COMBINE_DATA_H
#include "memory"
#include "TCombinedData.h"
#include "TDetectorInfo.h"
#include "TPrimaryData.h"
#include "Configure.h"

#include <fstream>

class TCombinedData;
class TDetectorInfo;
class DetectorConfig;

class PreCombinedData
{
    friend class TCombinedData;
    friend class BoardsManager;
    friend fstream &operator<<(fstream &, const PreCombinedData &);
    friend ostream &operator<<(ostream &, const PreCombinedData &);

public:
    PreCombinedData(int X);
    PreCombinedData(const DetectorConfig &);
    PreCombinedData(TCombinedData &);
    PreCombinedData(const TDetectorInfo &);

    bool Combine(PrimaryDataPtr &b, const DetectorConfig *detector);
    bool Combine(int Index, PrimaryDataPtr &b); // Index, X_Y are board information that should be provided by board_manager

    void Show(int Index);
    void Show(bool ALL); //Show all or just valid boards

    int Get_Time() const { return fDataTime; }
    int Get_Counter() const { return fDataCounter; }
    int Get_Layer_Number() const { return fLayerNumber; }

    inline PrimaryDataPtr GetData(int index) const;

    int Write(const char *name);

    int GetADC(int index) const;
    int GetADC(int boardIndex, int index) const;

    UChar_t GetMac5(int index) const { return fMacArray[index]; }

    bool GetValidation(int index) const { return fFlagArray[index]; }

    const vector<bool> &GetFlagArray() const { return fFlagArray; }

private:
    vector<PrimaryDataPtr> fLayerArray;
    vector<bool> fFlagArray;
    vector<UChar_t> fMacArray;

    int fLayerNumber;
    int fDataTime;
    int fDataCounter;
};

fstream &operator<<(fstream &, const PreCombinedData &);
ostream &operator<<(ostream &, const PreCombinedData &);

class CombinedData
{
    friend class TCombinedData;
    friend fstream &operator<<(fstream &, const CombinedData &);
    friend ostream &operator<<(ostream &, const CombinedData &);

public:
    CombinedData() = default;
    CombinedData(int X);
    CombinedData(const DetectorConfig &detector);
    CombinedData(TCombinedData &);
    CombinedData(const TDetectorInfo &);

    bool Combine(int Index, PrimaryDataPtr &b); // Index, X_Y are board information that should be provided by board_manager
    bool Combine(PrimaryDataPtr &b, const DetectorConfig *detector);

    void Show(int Index, bool X_Y);
    void Show(bool ALL); //Show all or just valid boards

    int Get_Time() const
    {
        if (sp)
            return sp->Get_Time();
        else
            return -1;
    }
    int Get_Counter() const
    {
        if (sp)
            return sp->Get_Counter();
        else
            return 0;
    }
    int Get_Layer_Number() const
    {
        if (sp)
            return sp->Get_Layer_Number();
        else
            return 0;
    }

    PreCombinedData &operator*() const { return *sp; }
    PreCombinedData *operator->() const { return &this->operator*(); }

    operator bool() const { return (bool)sp; }

    int Write(const char *name = 0) const { return sp->Write(name); }
    PrimaryDataPtr GetData(int index) const { return sp->GetData(index); }

    int GetADC(int index) const
    {
        if (!sp)
            return 0;
        return sp->GetADC(index);
    }
    int GetADC(int boardIndex, int index) const
    {
        if (!sp)
            return 0;
        return sp->GetADC(boardIndex, index);
    }

    void Clear() { sp = NULL; }

    UChar_t GetMac5(int index) const { return sp->GetMac5(index); }
    bool GetValidation(int index) const { return sp->GetValidation(index); }
    const vector<bool> &GetFlagArray() const { return sp->GetFlagArray(); }

private:
    shared_ptr<PreCombinedData> sp;
};

fstream &operator<<(fstream &, const CombinedData &);
ostream &operator<<(ostream &, const CombinedData &);

inline PrimaryDataPtr PreCombinedData::GetData(int index) const
{
    if (index >= fLayerNumber)
        return PrimaryDataPtr();
    return fLayerArray[index];
}

// class test
// {
// public:
//     test();
//     vector<double> *test0;
// };
// #ifdef __ROOTCLING__
// #pragma link C++ class vector<double>;
// #pragma link C++ class test;
// #endif

#endif
