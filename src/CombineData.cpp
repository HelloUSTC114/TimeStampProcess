// #define WASTE
#ifndef WASTE

#define COMBINE_DATA_CXX
#include "CombineData.h"
#include "TCombinedData.h"

PreCombinedData::PreCombinedData(int X):
fLayerNumber(X)
{
    for(int i = 0; i < fLayerNumber; i++)
    {
        fFlagArray.push_back(0);
        PrimaryDataPtr temp;
        fLayerArray.push_back(temp);
        fMacArray.push_back(-1);
    }
    fDataTime = 0;
    fDataCounter = 0;


}

PreCombinedData::PreCombinedData(const DetectorConfig& detector):
fLayerNumber(detector.GetTotalBoardNumber())
{
    for(int i = 0; i < fLayerNumber; i++)
    {
        fFlagArray.push_back(0);
        PrimaryDataPtr temp;
        fLayerArray.push_back(temp);
        fMacArray.push_back(detector.GetMac(i));
    }
    fDataTime = 0;
    fDataCounter = 0;
}


PreCombinedData::PreCombinedData(TCombinedData &Ini):       // Construct PreCombinedData from root file(TCombinedData)
fDataCounter(Ini.fDataCounter), fDataTime(Ini.fDataTime), fLayerNumber(Ini.fLayerNumber)
{
    if(!Ini.fControlFlag)    // If Preprimary data address is not under control of TCOmbined data, abort construct process;
    {
        cout << "Reconstruct from original root data, abort!" << endl;
        fDataCounter = 0;
        return;
    }

    int x_data_counter_temp = 0;
    for(int x = 0; x < fLayerNumber; x++)
    {
        bool bool_temp = Ini.fLayerFlags[x];
        fFlagArray.push_back(bool_temp);

        UChar_t mac_temp = Ini.fMac5Array[x];
        fMacArray.push_back(mac_temp);
        
        if(!bool_temp)
        {
            PrimaryDataPtr temp(NULL);
            fLayerArray.push_back(temp);
            continue;
        }

        // Warning:::::::::!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ******************************************
        // There should be a warning here, in case of primarydata hasn't been read yet
        // It's a very serious problem that will cause memory leak.
        // ******************************************

        TPrimaryData* objtemp = (TPrimaryData*)Ini.fLayerData.At(x_data_counter_temp);
        PrimaryDataPtr temp(objtemp);

        fLayerArray.push_back(temp);
        x_data_counter_temp++;
        if(objtemp == NULL)
        {
            cerr << "Warning! Primary data hasn't been read into memory for this combined Data" << endl;
            x_data_counter_temp --;
            fFlagArray[x] = 0;  // Set this flag at 0 to invalidate this data
            fDataCounter --;    // Decrement fDatacounter to avoid trouble in following process
        }
    }

    Ini.fControlFlag = false;

}

PreCombinedData::PreCombinedData(const TDetectorInfo &Info):
PreCombinedData(Info.GetXLayerNum())
{}

bool PreCombinedData::Combine(int Index, PrimaryDataPtr &b)       // Need to add judgement of whether index is beyond layer number
{
    if(fFlagArray[Index])
    {
        cout << "Combine Failure, data has been occupied" << endl;
        return false;
    }

    fMacArray[Index] = b.Get_Mac();
    
    // cout << "mac: " << (int)b.Get_Mac() << endl;
    // cout << "Time: " << b.Get_TDC_Value() << endl;
    // cout << "Index: " << Index << endl;
    // cout << "size: " << fLayerArray.size() << endl;
    // cout << "Board Counter: " << gConfigure -> BoardCounts() << endl;
    
    fLayerArray[Index] = b;
    fFlagArray[Index] = 1;
    fMacArray[Index] = b.Get_Mac();

    fDataTime = (fDataCounter * fDataTime + b.Get_TDC_Value()) / (fDataCounter + 1);
    fDataCounter++; // Important, fDatacounter should increment after all processes
    // Show(true);

    return true;

}

bool PreCombinedData::Combine(PrimaryDataPtr &b, const DetectorConfig *detector)
{
    return Combine(detector->GetBoardIndexByMac( b.Get_Mac()), b);
}

void PreCombinedData::Show(int Index)
{
    cout << "Layer " << Index << " Board" << endl;
    cout << "Mac5: " << (int)fMacArray[Index] << endl;
    if(Index > fLayerNumber)
    {
        cout << "Error! Out of layer range!" << endl;
        return;
    }
    cout << "validation: " << fFlagArray[Index] << endl;
    return;

}

void PreCombinedData::Show(bool ALL)
{
    cout << "Detector: " << endl;
    cout << "*************************************************" << endl;
    cout << "Layer: " << endl;

    for(int i = 0; i < fLayerNumber; i++)
    {
        if(!fFlagArray[i]&&!ALL)
            continue;
        cout << "Board\t" << i << "\tValidation:\t" << fFlagArray[i] << endl;
        cout << "Mac5:\t" << (int) fMacArray[i] << endl;
        if(!fFlagArray[i])
            continue;
        fLayerArray[i] -> Show(ALL);
    }
    cout << "*************************************************" << endl;
}

int PreCombinedData::Write(const char * name = 0)
{
    using namespace std;

    TCombinedData combine_temp(*this);
    combine_temp.Write(name);

    for(int x = 0; x < fLayerNumber; x++)
    {
        if(!fFlagArray[x])   continue;
        fLayerArray[x]->Write("DataX");
    }
    return 1;
}

int PreCombinedData::GetADC(int index) const
{
    if(index < 0) return 0;

    int BoardCount = index/32;
    int ChannelCount = index%32;
    if(BoardCount > fLayerNumber) return 0;
    if(!fFlagArray[BoardCount]) return 0;
    return fLayerArray[BoardCount][ChannelCount];
}

int PreCombinedData::GetADC(int boardIndex, int index) const
{
    if (index < 0)
        return 0;

    int BoardCount = boardIndex;
    int ChannelCount = index % 32;
    if (BoardCount > fLayerNumber)
        return 0;
    if (!fFlagArray[BoardCount])
        return 0;
    return fLayerArray[BoardCount][ChannelCount];
}



fstream& operator<<(fstream& fout, const PreCombinedData &a)
{
    fout << "X Layer numbers: " << a.fLayerNumber << endl;
    fout << "X Data array length: " << a.fLayerArray.size() << endl;
    fout << "X Data counter: " << a . fDataCounter << endl;


    fout << "Data time: " << a.fDataTime << endl;
    fout << "Data Counter: " << a.fDataCounter << endl;
    return fout;
}

ostream& operator<<(ostream& fout, const PreCombinedData &a)
{
    fout << "X Layer numbers: " << a.fLayerNumber << endl;
    fout << "X Data array length: " << a.fLayerArray.size() << endl;
    fout << "X Data counter: " << a . fDataCounter << endl;



    fout << "Data time: " << a.fDataTime << endl;
    fout << "Data Counter: " << a.fDataCounter << endl;
    return fout;
}


CombinedData::CombinedData(int X)
{
    sp = make_shared<PreCombinedData>(X);
}

CombinedData::CombinedData(const DetectorConfig& detector)
{
    sp = make_shared<PreCombinedData>(detector);
}


CombinedData::CombinedData(TCombinedData& Ini)
{
    sp = make_shared<PreCombinedData>(Ini);
}

CombinedData::CombinedData(const TDetectorInfo &Info)
{
    sp = make_shared<PreCombinedData>(Info);
}

bool CombinedData::Combine(int Index, PrimaryDataPtr &b)
{
    if(!sp)
    {
        cout << "Error! Combined data is invalid!" << endl;
        return false;
    }
    return sp -> Combine(Index, b);

}

bool CombinedData::Combine(PrimaryDataPtr &b, const DetectorConfig *detector)
{
    if (!sp)
    {
        cout << "Error! Combined data is invalid!" << endl;
        return false;
    }
    return sp->Combine(b, detector);
}

void CombinedData::Show(int Index, bool X_Y)
{
    if(!sp) return;
    sp -> Show(Index);
}

void CombinedData::Show(bool ALL)    //Show all or just valid boards
{
    if(!sp) return;
    sp -> Show(ALL);
}


fstream& operator<<(fstream& fout, const CombinedData &a)
{
    if(!a)
    {
        cout << "Error! Combined data is invalid" << endl;
        return fout;
    }
    fout << *a;

    return fout;
}
ostream& operator<<(ostream& fout, const CombinedData &a)
{
    if(!a)
    {
        cout << "Error! Combined data is invalid" << endl;
        return fout;
    }
    fout << *a;

    return fout;
}

// test::test()
// {
//     test0 = new vector<double>;
//     for(int i = 0; i  < 4; i++)
//     {
//         test0->push_back(i);
//     }
// }



#endif
