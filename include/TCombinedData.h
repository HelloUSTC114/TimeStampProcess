#ifndef TCOMBIENDDATA_H
#define TCOMBIENDDATA_H
#include "TObject.h"

#include "CombineData.h"
#include "TPrimaryData.h"
#include "TRef.h"
#include "TRefArray.h"

class PreCombinedData;
class CombinedData;
// Copy operation of TCombinedData is forbidened.
class TCombinedData : public TObject // No duty to clear memory, just act as I/O interface
{
    friend class PreCombinedData;
    // friend class FileManager;
public:
    TCombinedData()
    {
        fLayerFlags = NULL;
        fControlFlag = true;
        fMac5Array = NULL;
    }
    TCombinedData(const TCombinedData &) = delete;
    TCombinedData(const PreCombinedData &);
    TCombinedData(const CombinedData &);

    TCombinedData &operator=(TCombinedData &) = delete;

    void Clear(Option_t *) override;

    void ResetData(const PreCombinedData &);
    void ResetData(const CombinedData &);

    inline TPrimaryData *GetData(int index) const;
    UChar_t GetMac5(int index) const { return fMac5Array[index]; }

    int DataNum() const { return fDataCounter; }

    UInt_t StartIndex() const { return PrimaryDataStartIndex; }

    bool SetDataStartIndex(int primaryDataStartIndex)
    {
        PrimaryDataStartIndex = primaryDataStartIndex;
        return true;
    }

    const TRefArray &GetLayerDataRef() { return fLayerData; }

    ~TCombinedData();

private:
    int fMemberNo;             //!
    static int fMemberCounter; //!
    int fLayerNumber;
    int fDataCounter;

    bool *fLayerFlags;   //[fLayerNumber]
    UChar_t *fMac5Array; //[fLayerNumber]

    UInt_t PrimaryDataStartIndex; // Primary data tree start entry

    TRefArray fLayerData; // Ref to TPrimaryData

    bool fControlFlag; //! Ensure only one Combined data can get control of pre_primary_data;

    int fDataTime;

    ClassDef(TCombinedData, 1);
};

inline TPrimaryData *TCombinedData::GetData(int index) const
{
    if (index >= fDataCounter)
        return NULL;
    TPrimaryData *temp = (TPrimaryData *)fLayerData.At(index);
    return temp;
}

#ifdef __ROOTCLING__
#pragma link C++ class TGainCoefficient;
#endif

#endif