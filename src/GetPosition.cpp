/*
#include "GetPosition.h"

int SquarePosition::fFiredThreshold = 2000;

int Position::fDataCounter = 0;
int Position::fXFiredCounter = 0;
int Position::fYFiredCounter = 0;
int Position::fTotalFiredCounter = 0;
int Position::fFailureData = 0;

Position::Position(CombinedData& Data):
data(Data)
{}

void Position::ResetData(CombinedData &Data){
    data = Data;
    Clear();
}

int Position::JudgePosition(int channelIndex){
    int stripindex = channelIndex / 2;
    return stripindex;
}

void Position::Clear()
{
    if(!fProcessed) return;
    fXPoints = 0;
    fYPoints = 0;
    fFillPoint = 0;
    fProcessed = 0;
    fCounted = 0;

    fXValue.clear();
    fXWeight.clear();
    fYValue.clear();
    fYWeight.clear();
}

bool Position::GetPosition(int &x, int &y)
{
    if(!data) return false;
    if(!fProcessed) Process();

    int xMaxPoint = 0;
    int xMaxWeight = 0;
    for(int x = 0; x < fXPoints; x++)
    {
        if(xMaxWeight < fXWeight[x])
        {
            xMaxWeight = fXWeight[x];
            xMaxPoint = fXValue[x];
        }
    }
    int yMaxPoint = 0;
    int yMaxWeight = 0;

    for (int y = 0; y < fYPoints; y++)
    {
        if (yMaxWeight < fYWeight[y])
        {
            yMaxWeight = fYWeight[y];
            yMaxPoint = fYValue[y];
        }
    }

    x = xMaxPoint;
    y = yMaxPoint;

    return true;
}

bool Position::GetPosition(int point, int &x, int &y, double &weight)
{
    if(point >= fFillPoint)
    {
        x = 0;
        y = 0;
        weight = 0;
        return false;
    }
    if (!fProcessed)
        Process();

    int xindex = point / fXPoints;
    int yindex = point % fXPoints;

    // cout << "xindex: "  << xindex << endl;
    // cout << "yindex: " << yindex << endl;


    x = fXValue[xindex];
    y = fYValue[yindex];
    // cout << "XWeight: " << fXWeight[xindex] << endl;
    // cout << "YWeight: " << fYWeight[yindex] << endl;
    weight = fXWeight[xindex] * fYWeight[yindex];
    // cout << "Weight : " << weight << endl;
    return true;
}

bool Position::GetPosition(int &xPoints, int *x, double *xWeight, int &yPoints, int *y, double *yWeight)
{
    if (!data)
        return false;
    if (!fProcessed)
        Process();

    xPoints = fXPoints;
    yPoints = fYPoints;

    x = new int[fXPoints];
    y = new int[fYPoints];
    xWeight = new double[fXPoints];
    yWeight = new double[fYPoints];

    for (int i = 0; i < xPoints; i++)
    {
        x[i] = fXValue[i];
        xWeight[i] = fXWeight[i];
    }
    for (int i = 0; i < yPoints; i++)
    {
        y[i] = fYValue[i];
        yWeight[i] = fYWeight[i];
    }

    return false;
}

void Position::Process()
{
    fProcessed = 1;
    AddCounter();
}

void Position::AddCounter()
{
    if(!fProcessed) return;
    if(fCounted)    return;

    fDataCounter ++;
    fXFiredCounter += fXPoints;
    fYFiredCounter += fYPoints;
    fTotalFiredCounter += fFillPoint;

    if(fFillPoint == 0)
    {
        fFailureData ++;
    }
    fCounted = 1;
}

void Position::ClearCounter()
{
    fDataCounter = 0;
    fXFiredCounter = 0;
    fYFiredCounter = 0;
    fTotalFiredCounter = 0;
    fFailureData = 0;
}

int Position::GetDataCounter()
{
    return fDataCounter;
}

int Position::GetXFiredCounter()
{
    return fXFiredCounter;
}

int Position::GetYFiredCounter()
{
    return fYFiredCounter;
}

int Position::GetTotalFiredCounter()
{
    return fTotalFiredCounter;
}

int Position::GetFailureDataCounter()
{
    return fFailureData;
}

double Position::GetMeanXFired()
{
    if(fDataCounter == 0)   return 0;
    return (double)fXFiredCounter / (double)fDataCounter;
}

double Position::GetMeanYFired()
{
    if(fDataCounter == 0)   return 0;
    return (double)fYFiredCounter / (double)fDataCounter;
}

double Position::GetMeanTotalFired()
{
    if(fDataCounter == 0)   return 0;
    return (double)fTotalFiredCounter / (double)fDataCounter;
}

double Position::GetMeanFailure()
{
    if(fDataCounter == 0)   return 0;
    return (double)fFailureData / (double)fDataCounter;
}

void Position::PrintCounterInfo()
{
    cout << "Data counter: " << fDataCounter << endl;
    cout << "XFired\t:\t" << fXFiredCounter << "\tMean: " << GetMeanXFired() << endl;
    cout << "YFired\t:\t" << fYFiredCounter << "\tMean: " << GetMeanYFired() << endl;
    cout << "Total Fired:\t" << fTotalFiredCounter << "\tMean: " << GetMeanTotalFired() << endl;
    cout << "Failure Fired:\t" << fFailureData << "\tRate: " << GetMeanFailure() << endl;
}

int SquarePosition::GetFillPoints()
{
    if(!data)
    {
        cerr << "Error: No data" << endl;
        return 0;
    }
    if(!fProcessed) 
    {
        Process();
    }
    return fFillPoint;
}

void SquarePosition::Process()
{
    Clear();

    FindFiredChannel();
    NormalizeWeight();
    GenerateInfo();
    Position::Process();
}

void SquarePosition::FindFiredChannel()
{
    int XBoardNum = data.Get_Layer_Number(0);
    int YBoardNum = data.Get_Layer_Number(1);

    int xChannelNum = 32 * XBoardNum;
    int yChannelNum = 32 * YBoardNum;

    int xMax = 0, xMaxChannel = -1;
    int yMax = 0, yMaxChannel = -1;

    // Find all fired adc
    // Remember, even channel and adjoint odd chennel should combined together
    for (int xchannel = 0; xchannel < xChannelNum; xchannel++)
    {
        int temp1 = data.GetADC(xchannel++, 0); // Get even channel
        int temp2 = data.GetADC(xchannel, 0);   // Get odd channel
        // continue;

        temp1 *= gDetectorInfo->GetADCGain(xchannel-1, 0);                             // Gain coeffience
        temp2 *= gDetectorInfo->GetADCGain(xchannel, 0);                           // Gain coeffience
        int temp = temp1 + temp2;               // Group total adc value

        if (temp > fFiredThreshold)
        {
            fXValue.push_back(xchannel/2);
            fXWeight.push_back(temp);
        }

    }


    for (int ychannel = 0; ychannel < yChannelNum; ychannel++)
    {
        int temp1 = data.GetADC(ychannel++, 1); // Get even channel
        int temp2 = data.GetADC(ychannel, 1);   // Get odd channel
        temp1 *= gDetectorInfo->GetADCGain(ychannel - 1, 1); // Gain coeffience
        temp2 *= gDetectorInfo->GetADCGain(ychannel, 1); // Gain coeffience
        int temp = temp1 + temp2;               // Group total adc value

        if (temp > fFiredThreshold)
        {
            fYValue.push_back(ychannel/2);
            fYWeight.push_back(temp);
        }
    }

}

void SquarePosition::NormalizeWeight()
{
    double sum = 0;
    // cout << "X size: " << fXWeight .size() << endl;
    for (int x = 0; x < fXWeight.size(); x++)
    {
        sum += fXWeight[x];
    }
    for (int x = 0; x < fXWeight.size(); x++)
    {
        fXWeight[x] /= sum;
    }
    sum = 0;
    for (int y = 0; y < fYWeight.size(); y++)
    {
        sum += fYWeight[y];
    }
    for (int y = 0; y < fYWeight.size(); y++)
    {
        fYWeight[y] /= sum;
    }
}

void SquarePosition::GenerateInfo()
{
    fXPoints = fXValue.size();
    fYPoints = fYValue.size();
    // cout << "Test: X fired Points: " << fXPoints << endl; 
    fFillPoint = fXPoints * fYPoints;
}

void SquarePosition::SetThreshold(int adc)
{
    SquarePosition::fFiredThreshold = adc;
}

int SquarePosition::GetThreshold()
{
    return SquarePosition::fFiredThreshold;
}

bool FillData(CombinedData data, TH2 *h, Position &pos, bool Verbose)
{
    pos.ResetData(data);
    int fillPoints = pos.GetFillPoints();
    if(Verbose)
        cout << "Test: Total Fill Points: " << fillPoints << endl;

    for (int point = 0; point < fillPoints; point++)
    {
        int x(0), y(0);
        double Weight;
        pos.GetPosition(point, x, y, Weight);
        h->Fill(x, y, 1);
        // h->Fill(x, y, Weight);
        if(Verbose)
        {
            cout << "Test: Point: " << point << " Weight: " << Weight << endl;
            cout << "x: " << x << endl;
            cout << "y: " << y << endl;

        }

    }
    if(Verbose)
        cout << endl;
    return true;
}


*/

#include "GetPosition.h"

TF1 *GetPositionForPlate::fGausFun = new TF1("fGaus", "gaus", -15, 15);

double GetPositionForSquareSingle::GetPosition(CombinedData &data, UChar_t mac5, double &eventWeight)
{
    int boardIndex = gConfigure->GetBoardIndexByMac(mac5);
    auto array = data.GetData(boardIndex)->GetDataArray();
    double weightarray[32]{0};

    for (int ch = 0; ch < 32; ch++)
    {
        if (!gConfigure->JudgeValidChannel(mac5, ch))
            continue;
        double adcValue = array[ch];
        weightarray[ch] = gCaliManager->GetSignalWeight(boardIndex, ch, adcValue);
    }

    // double grWeightAr[32]{0};

    vector<ChWeight> GrVector;
    for (int gr = 0; gr < 16; gr++)
    {
        int ch1 = gr * 2;
        int ch2 = ch1 + 1;
        if (weightarray[ch1] < 0.3 && weightarray[ch2] < 0.3)
        {
            continue;
        }
        double grWeight = weightarray[gr * 2] + weightarray[gr * 2 + 1];

        GrVector.push_back(make_pair(gr, grWeight));
    }

    // Sort Group by weight
    SortByDescend(GrVector); // Descend

    int NGrSize = GrVector.size();
    double sum = 0;
    double weightsum = 0;
    for (int i = 0; i < NGrSize; i++)
    {
        double ch = GrVector[i].first;
        double wei = GrVector[i].second;
        sum += ch * wei;
        weightsum += wei;
    }

    double mean = sum / weightsum;

    double accum = 0.0;
    for (int i = 0; i < NGrSize; i++)
    {
        double ch = GrVector[i].first;
        double wei = GrVector[i].second;

        accum += (ch - mean) * (ch - mean) * wei;
    }
    accum /= weightsum;

    double ssquare = accum / NGrSize; //方差
    double dev = TMath::Sqrt(ssquare);

    if (NGrSize == 0)
    {
        cout << "=========================================" << endl;
        cout << "Mac5: " << (int)mac5 << endl;
        cout << "Error, all channels signal are too small." << endl;
        cout << "Group\tCh1\tCh2" << endl;
        for (int gr = 0; gr < 16; gr++)
        {
            int ch1 = gr * 2;
            int ch2 = ch1 + 1;
            cout << gr << '\t' << array[ch1] << '\t' << array[ch2] << endl;
        }
        eventWeight = -1;
        return -1;
    }
    if (dev > 2)
    {
        cout << "-----------------------------------------------" << endl;
        cout << "Warning: fired channels are seperated too much!" << endl;
        cout << "mean: " << mean << endl;
        cout << "accum: " << accum << endl;
        cout << "ssquare: " << ssquare << endl;
        cout << "dev: " << dev << endl;
        cout << "Group\tWeight\tCh1\tCh2" << endl;
        for (int i = 0; i < NGrSize; i++)
        {
            int group = GrVector[i].first;
            int ch1 = group * 2;
            int ch2 = ch1 + 1;
            cout << group << '\t' << GrVector[i].second << '\t' << array[ch1] << '\t' << array[ch2] << endl;
        }
        cout << "-----------------------------------------------" << endl;

        double weightDevi = GrVector[0].second / GrVector[1].second;
        double chDevi = TMath::Abs(GrVector[0].first - GrVector[1].first);
        if (weightDevi > 2)
        {
            eventWeight = GrVector[0].second;
            return GrVector[0].first;
        }
        else
        {
            eventWeight = -1;
            return -1;
        }
    }

    double GrSum = 0;
    double WeightSum = 0;

    for (int i = 0; i < NGrSize; i++)
    {
        GrSum += GrVector[i].first;
        WeightSum += GrVector[i].second;
    }
    eventWeight = WeightSum;

    return GrSum / WeightSum;
}

double GetPositionForSquareSingleSimple::GetPosition(CombinedData &data, UChar_t mac5, double &eventWeight, int &NFired)
{
    int boardIndex = gConfigure->GetBoardIndexByMac(mac5);
    auto array = data.GetData(boardIndex)->GetDataArray();
    double weightarray[32]{0};

    for (int ch = 0; ch < 32; ch++)
    {
        if (!gConfigure->JudgeValidChannel(mac5, ch))
            continue;
        double adcValue = array[ch];
        weightarray[ch] = gCaliManager->GetSignalWeight(boardIndex, ch, adcValue);
    }

    // double grWeightAr[32]{0};

    vector<ChWeight> GrVector;
    vector<ChWeight> GrVector2;
    for (int gr = 0; gr < 16; gr++)
    {
        if (!gConfigure->JudgeValidChannel(mac5, gr * 2))
            continue;
        double grWeight = weightarray[gr * 2] + weightarray[gr * 2 + 1];
        double grWeight2 = weightarray[gr * 2] * weightarray[gr * 2 + 1];
        grWeight /= 2.0;
        grWeight2 = TMath::Sqrt(grWeight2);

        if (grWeight < 0.1)
            continue;

        // cout << weightarray[gr * 2] << '\t' << weightarray[gr * 2 + 1] << "\tSum:\t" << grWeight << "\tproduct:\t" << grWeight2 << endl;
        GrVector.push_back(make_pair(gr, grWeight));
        GrVector2.push_back(make_pair(gr, grWeight2));
    }

    // Sort Group by weight
    SortByDescend(GrVector);  // Descend
    SortByDescend(GrVector2); // Descend

    double WeightSum = 0;
    int NGrSize = GrVector.size();
    NFired = NGrSize;

    if (NGrSize >= 4)
    {
        // cout << "Warning: Fired Channels are too many!" << endl;
        // for_each(GrVector.begin(), GrVector.end(), [](std::pair<int,double> onepair) { cout << onepair.first << '\t' << onepair.second << endl; });
        return -1;
    }

    for (int i = 0; i < NGrSize; i++)
    {
        WeightSum += GrVector2[i].second;
    }
    eventWeight = WeightSum;

    if (NGrSize == 0)
    {
        return -1;
    }
    return GrVector[0].first;
}

double GetPositionForTriangle::GetPosition(CombinedData &data, UChar_t mac5, double &eventWeight, int &NFired, double kinverse)
{
    bool FireJudge = GetFiredTwoStrip(data, mac5);
    if (!FireJudge)
    {
        return -1;
    }

    if (fFiredGr.size() == 1)
    {
        return fFiredGr[0] * TriangleLength / 2.0;
    }

    if (fFiredGr.size() < 1)
    {
        return -1;
    }
    int Gr0 = fFiredGr[0];
    int Gr1 = fFiredGr[1];
    double Charge0 = fFiredGrWeight[0]; // Lower channel
    double Charge1 = fFiredGrWeight[1]; // Larger channel

    double h2overh1 = Charge1 / Charge0; // Proportion of this 2 channel
    double Result = ((Gr0 * Charge0) + (Gr1 * Charge1)) / (Charge0 + Charge1);

    Result *= TriangleLength / 2.0;

    eventWeight = Charge0 + Charge1;
    NFired = fFiredGr.size();
    // Result = h2Overh1;
    // Result = (1 + 1.0 / 2 * kinverse - 1.0 / 2 * h2overh1) / (1 + h2overh1) * TriangleThickness;
    return Result;
}

double GetPositionForTriangle::GetPositionCorrection(CombinedData &data, UChar_t mac5, double &eventWeight, int &NFired, double kinverse)
{
    bool FireJudge = GetFiredTwoStrip(data, mac5);
    if (!FireJudge)
    {
        return -1;
    }

    if (fFiredGr.size() < 1)
        return -1;
    if (fFiredGr.size() == 1)
    {
        return fFiredGr[0] * TriangleLength / 2.0;
    }

    int Gr0 = fFiredGr[0];
    int Gr1 = fFiredGr[1];
    double Charge0 = fFiredGrWeight[0]; // Lower channel
    double Charge1 = fFiredGrWeight[1]; // Larger channel

    ChWeight GrPair[2];
    GrPair[0] = ChWeight(Gr0, Charge0);
    GrPair[1] = ChWeight(Gr1, Charge1);
    // Find Larger group
    int LargerGroupIndex = (GrPair[0].first > GrPair[1].first) ? 0 : 1;
    int SmallGroupIndex = (GrPair[0].first < GrPair[1].first) ? 0 : 1;
    // Find Scintillator upper, it's even number in this case
    int UpperGroupIndex = (GrPair[0].first % 2 == 0) ? 0 : 1;
    int LowerGroupIndex = (GrPair[0].first % 2 != 0) ? 0 : 1;

    double Result0 = ((Gr0 * Charge0) + (Gr1 * Charge1)) / (Charge0 + Charge1);

    // double CorrectionCoefficient = (GrPair[SmallGroupIndex].second - GrPair[LargerGroupIndex].second) * 1.0 / 2.0 * kinverse;
    double CorrectionCoefficient = (GrPair[SmallGroupIndex].second - GrPair[LargerGroupIndex].second) * 1.0 / 2.0 * kinverse;
    // double CorrectionTerm = (GrPair[LowerGroupIndex].first - GrPair[UpperGroupIndex].first) * CorrectionCoefficient;
    double CorrectionTerm = 1.0 / 2.0 * kinverse * (GrPair[UpperGroupIndex].second - GrPair[LowerGroupIndex].second);

    CorrectionTerm /= (Charge0 + Charge1);
    double Result = Result0 + CorrectionTerm;

    // cout << "kInverse: " << kinverse << endl;
    // cout << "LargerGroup: " << GrPair[LargerGroupIndex].first << endl;
    // cout << "EvenGroup: " << GrPair[UpperGroupIndex].first << endl;
    // cout << "Correction Coefficient: " << CorrectionCoefficient << endl;
    // cout << "Correction Term: " << CorrectionTerm << endl;
    // cout << endl;

    // double h2overh1 = Charge1 / Charge0; // Proportion of this 2 channel

    eventWeight = Charge0 + Charge1;
    NFired = fFiredGr.size();
    // Result = h2Overh1;
    // Result = (1 + 1.0 / 2 * kinverse - 1.0 / 2 * h2overh1) / (1 + h2overh1) * TriangleThickness;
    Result0 *= TriangleLength / 2.0;
    Result *= TriangleLength / 2.0;
    return Result;
}

bool GetPositionForTriangle::GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray)
{
    vector<double> WeightArray;
    GetFiredTwoStrip(data, mac5, GrArray, WeightArray);
}

bool GetPositionForTriangle::GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray, vector<double> &WeightArray)
{
    GrArray.clear();
    WeightArray.clear();

    int boardIndex = gConfigure->GetBoardIndexByMac(mac5);
    if (!data.GetValidation(boardIndex))
    {
        return false;
    }

    auto array = data.GetData(boardIndex)->GetDataArray();

    double weightarray[32]{0};

    for (int ch = 0; ch < 32; ch++)
    {
        if (!gConfigure->JudgeValidChannel(mac5, ch))
            continue;
        double adcValue = array[ch];
        weightarray[ch] = gCaliManager->GetSignalWeight(boardIndex, ch, adcValue);
        // cout << "Channel: " << ch << '\t' << adcValue << '\t' << weightarray[ch] << endl;
    }

    vector<ChWeight> GrVector;
    vector<ChWeight> GrVector2;

    for (int gr = 0; gr < 16; gr++)
    {
        int ch1 = gr * 2;
        int ch2 = ch1 + 1;
        if (!gConfigure->JudgeValidChannel(mac5, ch1) || !(gConfigure->JudgeValidChannel(mac5, ch2)))
            continue;
        double grWeight = weightarray[gr * 2] + weightarray[gr * 2 + 1];
        double grWeight2 = weightarray[gr * 2] * weightarray[gr * 2 + 1];
        grWeight /= 2.0;
        grWeight2 = TMath::Sqrt(grWeight2);

        if (grWeight < 0.1)
            continue;

        // cout << weightarray[gr * 2] << '\t' << weightarray[gr * 2 + 1] << "\tSum:\t" << grWeight << "\tproduct:\t" << grWeight2 << endl;
        GrVector.push_back(make_pair(gr, grWeight));
        GrVector2.push_back(make_pair(gr, grWeight2));
    }

    SortByDescend(GrVector);  // Descend
    SortByDescend(GrVector2); // Descend

    for (int i = 0; i < GrVector2.size(); i++)
    {
        GrArray.push_back(GrVector2[i].first);
        WeightArray.push_back(GrVector2[i].second);
    }
    if (GrArray.size() < 2)
    {
        return true;
    }
    if (TMath::Abs(GrArray[0] - GrArray[1]) > 1)
    {
        return false;
    }
    return true;
}

bool GetPositionForTriangle::GetFiredTwoStrip(CombinedData &data, UChar_t mac5)
{
    return GetFiredTwoStrip(data, mac5, fFiredGr, fFiredGrWeight);
}

double SqrtMean(double ch1, double ch2)
{
    return TMath::Sqrt(ch1 * ch2);
}

bool GetPositionForPlate::GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray, vector<double> &WeightArray)
{
    // GrArray.clear();
    // WeightArray.clear();

    // int boardIndex = gConfigure->GetBoardIndexByMac(mac5);
    // if (!data.GetValidation(boardIndex))
    // {
    //     return false;
    // }

    // auto array = data.GetData(boardIndex)->GetDataArray();

    // double weightarray[32]{0};

    // for (int ch = 0; ch < 32; ch++)
    // {
    //     if (!gConfigure->JudgeValidChannel(mac5, ch))
    //         continue;
    //     double adcValue = array[ch];
    //     weightarray[ch] = gCaliManager->GetSignalWeight(boardIndex, ch, adcValue);
    //     // cout << "Channel: " << ch << '\t' << adcValue << '\t' << weightarray[ch] << endl;
    // }

    // vector<ChWeight> GrVector;
    // vector<ChWeight> GrVector2;

    // for (int gr = 0; gr < 16; gr++)
    // {
    //     int ch1 = gr * 2;
    //     int ch2 = ch1 + 1;
    //     if (!gConfigure->JudgeValidChannel(mac5, ch1) || !(gConfigure->JudgeValidChannel(mac5, ch2)))
    //         continue;
    //     double grWeight = weightarray[gr * 2] + weightarray[gr * 2 + 1];
    //     double grWeight2 = weightarray[gr * 2] * weightarray[gr * 2 + 1];
    //     grWeight /= 2.0;
    //     grWeight2 = TMath::Sqrt(grWeight2);

    //     if (grWeight < 0.1)
    //         continue;

    //     // cout << weightarray[gr * 2] << '\t' << weightarray[gr * 2 + 1] << "\tSum:\t" << grWeight << "\tproduct:\t" << grWeight2 << endl;
    //     GrVector.push_back(make_pair(gr, grWeight));
    //     GrVector2.push_back(make_pair(gr, grWeight2));
    // }

    // SortByDescend(GrVector);  // Descend
    // SortByDescend(GrVector2); // Descend

    // for (int i = 0; i < GrVector2.size(); i++)
    // {
    //     GrArray.push_back(GrVector2[i].first);
    //     WeightArray.push_back(GrVector2[i].second);
    // }
    // if (GrArray.size() < 2)
    // {
    //     return true;
    // }
    // if (TMath::Abs(GrArray[0] - GrArray[1]) > 1)
    // {
    //     return false;
    // }
    // return true;

    return GetPositionForTriangle::GetFiredTwoStrip(data, mac5, GrArray, WeightArray);
}

bool GetPositionForPlate::GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray)
{
    vector<double> WeightArray;
    return GetFiredTwoStrip(data, mac5, GrArray, WeightArray);
}

bool GetPositionForPlate::GetFiredTwoStrip(CombinedData &data, UChar_t mac5)
{
    return GetFiredTwoStrip(data, mac5, fFiredGr, fFiredGrWeight);
}

double GetPositionForPlate::GetPosition(CombinedData &data, UChar_t mac5, double &eventWeight, int &NFired)
{
    bool FireJudge = GetFiredTwoStrip(data, mac5);
    if (!FireJudge)
    {
        // return -1;
    }

    if (fFiredGr.size() == 1)
    {
        return fFiredGr[0] * 16.0 / 15.0;
    }

    if (fFiredGr.size() < 1)
    {
        return -1;
    }
    double Result = -1;
    NFired = fFiredGr.size();

    auto xGr = new double[NFired];
    auto yWeight = new double[NFired];
    int NPoints = 0;

    double xmin = fFiredGr[0], xmax = xmin;

    eventWeight = 0;
    for (int i = 0; i < NFired; i++)
    {
        double x = fFiredGr[i];

        xmin = (xmin > x) ? x : xmin;
        xmax = (xmax > x) ? xmax : x;

        double y = fFiredGrWeight[i];
        eventWeight += y;
        if (y > 0.3)
        {
            xGr[NPoints] = x;
            yWeight[NPoints] = y;
            NPoints++;
        }
    }

    if (NPoints == 0)
    {
        return -1;
    }
    // if (NPoints == 1)
    // {
    //     Result = xGr[0];
    // }
    // if (NPoints == 2)
    // {
    //     Result = (xGr[0] * yWeight[0] + xGr[1] * yWeight[1]) / (yWeight[0] + yWeight[1]);
    // }
    // if (NPoints >= 3)
    // {
    //     Result = (xGr[0] * yWeight[0] + xGr[1] * yWeight[1] + xGr[2] * yWeight[2]) / (yWeight[0] + yWeight[1] + yWeight[2]);
    // }

    if (xGr[0] < 1 || xGr[0] > 14)
    {
        Result = xGr[0];
    }
    else
    {
        int xMid = xGr[0];
        // cout << "Mid: " << xMid << endl;
        int xLeft = xMid - 1;
        int xRight = xMid + 1;

        int LeftIndex = -1, RightIndex = -1;
        for (int i = 0; i < NPoints; i++)
        {
            if (xGr[i] == xLeft)
            {
                LeftIndex = i;
            }
            if (xGr[i] == xRight)
            {
                RightIndex = i;
            }
        }
        // cout << "LeftIndex: " << LeftIndex << endl;
        // cout << "RightIndex: " << RightIndex << endl;
        int Points = 1;
        double SumPosition = xMid*yWeight[0];
        double SumWeight = yWeight[0];
        if (LeftIndex > 0)
        {
            Points++;
            SumPosition += xLeft * yWeight[LeftIndex];
            SumWeight += yWeight[LeftIndex];
            // cout << "xLeft: " << xLeft << endl;
            // cout << "Left Weight: " << yWeight[LeftIndex] << endl;
        }
        if (RightIndex > 0)
        {
            Points++;
            SumPosition += xRight * yWeight[RightIndex];
            SumWeight += yWeight[RightIndex];
        }

        // cout << "SumPosition: " << SumPosition << endl;
        // cout << "SumWeight: " << SumWeight << endl;
        SumPosition /= SumWeight;
        Result = SumPosition;
        // cout << "Result: " << Result << endl;
    }

    // // cout << "NPoints: " << NPoints << endl;
    // for (int i = 0; i < NPoints; i++)
    // {
    //     cout << i << '\t' << xGr[i] << '\t' << yWeight[i] << endl;
    // }

    if (Result < 0 || Result > 15)
    {
        return -1;
    }

    delete[] xGr;
    delete[] yWeight;

    return Result * 16.0 / 15.0;
}