// 需要预留增益系数的接口，接口具体如何设计：
// 1. 增益系数与Configuration有关，因此需要在Configuration.h 里面加一个Class
// 2. 在这里应该判断应用的是哪个增益系数，因为可能会跨板找最大值，因此需要隐藏具体板的增益系数，而只保留探测器整体的增益系数，因此这个Class必须能够对同一层内不同板的增益系数进行比较

#ifndef GETPOSITION_H
#define GETPOSITION_H

#include "CombineData.h"
#include "CaliResult.h"
#include "Configure.h"
#include "TH2.h"
#include "SortWeight.h"
#include "TGraph.h"
#include "TF1.h"

/*

class CombinedData;
class Position
{
public:
    Position() = default;
    Position(CombinedData&);
    void ResetData(CombinedData &);
    int GetXPointsNum();    // Return fired X points
    int GetYPointsNum();    // Return fired Y points

    virtual bool GetPosition(int &x, int &y);   // Get the most important Point
    virtual bool GetPosition(int point, int &x, int &y, double &weight);  // Get specific Point and its weight
    virtual bool GetPosition(int &xPoints, int *x, double * xWeight, int &yPoints, int* y, double* yWeight);   // Get all coor value information

    virtual int GetFillPoints() = 0;

    // Get Counter information
    static void ClearCounter();
    static int GetDataCounter();
    static int GetXFiredCounter();
    static int GetYFiredCounter();
    static int GetTotalFiredCounter();
    static int GetFailureDataCounter();
    static double GetMeanXFired();
    static double GetMeanYFired();
    static double GetMeanTotalFired();
    static double GetMeanFailure();
    static void PrintCounterInfo();


protected:
    // Remember, there's no need to copy all data to memory. Thus, only need a operator or a function to access those data in order.
    CombinedData data;  // This is a smart pointer

    int fXPoints = 0;   // Total X fired Points
    int fYPoints = 0;   // Total Y fired Points
    int fFillPoint = 0; // Calculate total fired points

    vector<int> fXValue;    //  Record Position Number
    vector<double> fXWeight;    // Calculate its weight
    vector<int> fYValue;
    vector<double> fYWeight;

    bool JudgePoint(int Point, int xCoor, int yCoor);   // Convert Point index into x coor value and y coor value
    int JudgePosition(int channelIndex);    // Convert channel index into coordinates
    void Clear();
    virtual void Process() = 0;     // Add Counter and set fProcessed at 1

    bool fProcessed = 0;
    
private:



    // Counter part of Position class
    bool fCounted = 0;
    static int fDataCounter;
    static int fXFiredCounter;
    static int fYFiredCounter;
    static int fTotalFiredCounter;
    static int fFailureData;
    void AddCounter();

};

class SquarePosition : public Position{
public:
    static void SetThreshold(int);
    static int GetThreshold();
    virtual int GetFillPoints() override;

private:
    virtual void Process() override;
    void FindFiredChannel();
    void NormalizeWeight();
    void GenerateInfo();

    static int fFiredThreshold;
};

bool FillData(CombinedData data, TH2* h, Position &pos, bool Verbose = 0);
*/

// Above are previously programming code, which aims at Processing multi boards situation

const double TriangleThickness = 1.05; // Define TriangleThickness as 10.5 cm
const double TriangleLength = TriangleThickness*2; // Define TriangeLLength as 21 cm

class CombinedData;

class VPosition
{
public:
    VPosition() = default;
    virtual double GetPosition(CombinedData &data, UChar_t mac5, double &eventTotalCharge) = 0;   // Should also return event charge
};

class GetPositionForSquareSingle : public VPosition
{
public:
    GetPositionForSquareSingle() = default;
    double GetPosition(CombinedData &data, UChar_t mac5, double &) override;
};

// class GetPositionForSquareSingleSimple : public VPosition
class GetPositionForSquareSingleSimple
{
public:
    GetPositionForSquareSingleSimple() = default;
    static double GetPosition(CombinedData &data, UChar_t mac5, double &, int &NFiredStrip);
};

class GetPositionForTriangle
{
public:
    GetPositionForTriangle() = default;
    double GetPosition(CombinedData &data, UChar_t mac5, double &, int &NFiredStrip, double slopeinverse = 0);
    double GetPositionCorrection(CombinedData &data, UChar_t mac5, double &, int &NFiredStrip, double slopeinverse = 0);
    static bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray);
    static bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray, vector<double>&WeightArray);

    bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5);

private:
    vector<int> fFiredGr;
    vector<double> fFiredGrWeight;
    bool fFiredFlag;
};

class GetPositionForPlate
{
public:
    GetPositionForPlate() = default;
    double GetPosition(CombinedData &data, UChar_t mac5, double &eventWeight, int &NFiredStrip);
    static bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray);
    static bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5, vector<int> &GrArray, vector<double> &WeightArray);

    bool GetFiredTwoStrip(CombinedData &data, UChar_t mac5);

private:
    vector<int> fFiredGr;
    vector<double> fFiredGrWeight;
    bool fFiredFlag;
    static TF1* fGausFun;
    
};

double SqrtMean(double ch1Weight, double Ch2Weight);
#endif