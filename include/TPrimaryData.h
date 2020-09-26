#ifndef PRE_PRIMARY_DATA_H
#define PRE_PRIMARY_DATA_H

#include "TROOT.h"
#include "iostream"
#include "fstream"
#include "TObject.h"
#include <memory>

using namespace std;

class TPrimaryData : public TObject
{
    friend class TCombinedData;

public:
    TPrimaryData() = default;
    TPrimaryData(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no);
    TPrimaryData &operator=(const TPrimaryData &a) = default; //define assignment function.

    void Reset(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no);

    UChar_t Get_Mac() const { return mac5; }
    UInt_t Get_TDC_Value() const { return clock_no ? ts1 : ts0; } //Return right clock time.
    bool Get_Clock() const { return clock_no; }

    UInt_t Get_Reset_Time() const { return reset_time; }

    int operator[](int index) const
    {
        if ((index < 0) || (index > 31))
            return 0;
        return chg[index];
    }

    const UShort_t *const GetDataArray() const { return chg; }

    virtual void Show(bool Verbose = 0) const;

protected:
    UChar_t mac5;
    UShort_t chg[32];
    UInt_t ts0;
    UInt_t ts1;
    UInt_t ts0_ref;
    UInt_t ts1_ref;

    UInt_t reset_time;

    int clock_compensation;

    bool clock_no; //Record the valid clock to be used

    ClassDef(TPrimaryData, 1);
};

bool operator==(const TPrimaryData &a, const TPrimaryData &b);
ostream &operator<<(ostream &fout, const TPrimaryData &a);

class PrimaryDataPtr
{
    friend class TCombinedData;
    friend class PreCombinedData;

public:
    PrimaryDataPtr() = default;
    PrimaryDataPtr(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no);
    PrimaryDataPtr(TPrimaryData *);

    void Reset(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no);

    UChar_t Get_Mac() const;
    UInt_t Get_TDC_Value() const; //Return right clock time.
    bool Get_Clock() const;

    UInt_t Get_Reset_Time() const;

    virtual void Show(bool Verbose = 0) const;

    TPrimaryData &operator*() const { return *sp; }
    TPrimaryData *operator->() const { return &this->operator*(); }
    TPrimaryData *get() const { return sp.get(); }

    operator bool() const { return (bool)sp; }
    int operator[](int index) const
    {
        if (!sp)
            return 0;
        return (*sp)[index];
    }

private:
    shared_ptr<TPrimaryData> sp;
};

bool operator==(const PrimaryDataPtr &a, const PrimaryDataPtr &b);
ostream &operator<<(ostream &fout, const PrimaryDataPtr &a);

#endif
