#define TCOMBIENDDATA_CXX
#include "TCombinedData.h"

ClassImp(TCombinedData);

int TCombinedData::fMemberCounter = 0;

TCombinedData::TCombinedData(const PreCombinedData& Ini):
fDataTime(Ini.fDataTime), fDataCounter(Ini.fDataCounter), fLayerNumber(Ini.fLayerNumber),
fControlFlag(0), PrimaryDataStartIndex(0)
{
    int DataCounterTemp = 0;
    fLayerFlags = new bool[fLayerNumber];
    fMac5Array = new UChar_t[fLayerNumber];

    for(int i = 0; i < fLayerNumber; i++)
    {
        bool temp = fLayerFlags[i] = Ini.fFlagArray[i];
        fMac5Array[i] = Ini.fMacArray[i];
        if(!temp)   continue;

        TObject* obj_temp = Ini.fLayerArray[i].get();
        fLayerData.Add(obj_temp);

        DataCounterTemp ++;
    }

    if(DataCounterTemp != fDataCounter)
    {
        cerr << "Warning: Real primary data inside this CData dismatch with data counter, check source code" << endl;
        fDataCounter = DataCounterTemp;
    }

    fMemberNo = fMemberCounter;
    fMemberCounter++;
}

TCombinedData::TCombinedData(const CombinedData &Ini):
TCombinedData(*Ini)
{}

TCombinedData::~TCombinedData()
{
    delete[] fLayerFlags;
    fLayerFlags = NULL;
    delete[] fMac5Array;
    fMac5Array = NULL;

    // May have bugs here
    if(fControlFlag)
    {
        for(int i = 0; i < fDataCounter; i++)
        {
            auto temp = (TPrimaryData*)(fLayerData.At(i));
            delete temp;
        }
    }
    fMemberCounter--;
    
}

void TCombinedData::Clear(Option_t *t = "")
{
    if(fLayerFlags){
        delete [] fLayerFlags;
        delete [] fMac5Array;
        fLayerFlags = NULL;
        fMac5Array = NULL;
    }
    fLayerData.Clear(t);
    this -> TObject::Clear(t);
    
    fLayerNumber = 0;
    fControlFlag = 0;
    fDataTime = 0;
    fDataCounter = 0;
}

void TCombinedData::ResetData(const PreCombinedData& Ini)
{
    Clear();

    fDataTime = Ini.fDataTime;
    fDataCounter = Ini.fDataCounter;
    fLayerNumber = Ini.fLayerNumber;
    fControlFlag = 0;


    fLayerFlags = new bool[fLayerNumber];
    fMac5Array = new UChar_t[fLayerNumber];

    int DataCounterTemp = 0;

    for(int x = 0; x < fLayerNumber; x++){
        bool temp = fLayerFlags[x] = Ini.fFlagArray[x];
        fMac5Array[x] = Ini.fMacArray[x];
        if(!temp)   continue;

        TObject* obj_temp = Ini.fLayerArray[x].get();
        fLayerData.Add(obj_temp);

        DataCounterTemp++;

        if(!obj_temp)
        {
            cerr << "Warning: Cannot find primary data inside this CData!" << endl;
        }
    }

    if (DataCounterTemp != fDataCounter)
    {
        cerr << "Warning: Real primary data inside this CData dismatch with data counter, check source code" << endl;
        fDataCounter = DataCounterTemp;
    }
}

void TCombinedData::ResetData(const CombinedData & Ini){
    ResetData(*Ini);
}