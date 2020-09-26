#define PRE_PRIMARY_DATA_CXX
#include "TPrimaryData.h"

ClassImp(TPrimaryData);

TPrimaryData::TPrimaryData(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no):
mac5(Mac5), ts0(Ts0), ts1(Ts1), ts0_ref(Ts0_ref), ts1_ref(Ts1_ref), clock_no(Clock_no), reset_time(Reset_Time)
{
    for(int channel = 0; channel < 32; channel ++)
    {
        chg[channel] = Chg[channel];
    }
}



void TPrimaryData::Reset(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no)
{
    mac5 = Mac5;
    ts0 = Ts0;
    ts1 = Ts1;
    ts0_ref = Ts0_ref;
    ts1_ref = Ts1_ref;

    reset_time = Reset_Time;
    for(int i = 0; i < 32; i++)
    {
        chg[i] = Chg[i];
    }
    clock_no = Clock_no;
}


bool operator==(const TPrimaryData &a, const TPrimaryData &b)
{
    if(TMath::Abs((Int_t)( a.Get_TDC_Value() - b.Get_TDC_Value() )) < 200)
    {
        return true;
    }
    else
        return false;
}


ostream & operator<<(ostream & fout, const TPrimaryData& a)
{
    fout << "Clock reset time: " << a.Get_Reset_Time() << '\t';
    fout << a.Get_TDC_Value();
    return fout;
}

void TPrimaryData::Show(bool Verbose) const
{
    cout << "==================>>>>>>>>" << endl;
    cout << "MAC:\t" << (int)this -> mac5 << endl;
    cout << "ts0:\t" << this -> ts0 << endl;
    cout << "ts1:\t" << this -> ts1 << endl;
    cout << "ts0_ref:\t" << this -> ts0_ref << endl;
    cout << "ts1_ref:\t" << this -> ts1_ref << endl;

    cout << "Clock Reset time:\t" << this -> reset_time << endl;

    if(Verbose)
    {
        cout << "Channel info: " << endl;
        for(int i = 0; i < 32; i++)
        {
            cout << "\tChannel: " << i << "\t" << this -> chg[i] << endl;
        }

    }

}



PrimaryDataPtr::PrimaryDataPtr(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no)
{
    sp = make_shared<TPrimaryData>(Mac5, Chg, Ts0, Ts1, Ts0_ref, Ts1_ref, Reset_Time, Clock_no);
}

PrimaryDataPtr::PrimaryDataPtr(TPrimaryData* ptr):
sp(ptr)
{}

void PrimaryDataPtr::Reset(UChar_t Mac5, UShort_t *Chg, UInt_t Ts0, UInt_t Ts1, UInt_t Ts0_ref, UInt_t Ts1_ref, UInt_t Reset_Time, bool Clock_no)
{
    if(!sp)
    {
        sp = make_shared<TPrimaryData>(Mac5, Chg, Ts0, Ts1, Ts0_ref, Ts1_ref, Reset_Time, Clock_no);
    }
    sp -> Reset(Mac5, Chg, Ts0, Ts1, Ts0_ref, Ts1_ref, Reset_Time, Clock_no);
}

UChar_t PrimaryDataPtr::Get_Mac()const
{
    if(!sp) return -1;
    return sp-> Get_Mac();
}
UInt_t PrimaryDataPtr::Get_TDC_Value() const  //Return right clock time.
{
    if(!sp) return -1;
    return sp-> Get_TDC_Value();
}
bool PrimaryDataPtr::Get_Clock() const
{
    if(!sp) return false;
    return sp-> Get_Clock();
}

UInt_t PrimaryDataPtr::Get_Reset_Time()const
{
    if(!sp) return (UInt_t) -1;
    return sp-> Get_Reset_Time();
}

void PrimaryDataPtr::Show(bool Verbose) const 
{
    if(!sp)  return;
    return sp-> Show(Verbose);
}



bool operator==(const PrimaryDataPtr &a, const PrimaryDataPtr &b)
{
    if(!a || !b)    return false;
    return *a == *b;
}

ostream & operator<<(ostream & fout, const PrimaryDataPtr& a)
{
    if(!a)  return fout;
    fout << *a;
    return fout;
}