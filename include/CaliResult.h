#ifndef CALIRESULT_H
#define CALIRESULT_H

#include "Configure.h"

#include "TObject.h"
#include "TDirectory.h"
#include "TFile.h"
#include <fstream>
#include "TH1D.h"
#include "TF1.h"
#include <ostream>
#include "TCanvas.h"
#include "TStyle.h"

// class TCaliResult: public TObject
// {
// public:
//     TCaliResult() = default;
//     ~TCaliResult(){Clear();}
//     void Init(int nboards);

// private:
//     int fNBoards;
//     double *fCoefficient = NULL;   //[fNBoards]
//     void Clear();
// };

double TriDetectorFitFun(double *xs, double *par); // Erf function plus a small correction

double FitFunErf(double *xs, double *par);  // Only Erf function

double FitFunPhotonNum(double *xs, double *par);    // No charge Q, just distribution of Photon number


struct CaliException
{
    CaliException(int board) : fBoard(board) {}
    int fBoard;
};

class TBoardCaliResult : public TObject
{
    // friend class TCaliResult;
public:
    // private:
    UChar_t fMac5;
    double fPed[32];       //[32]  Pedestal of this channel
    double fAmplitude[32]; //[32] Mean value of CR signal amplitude

    double GetSignalWeight(int channel, double ADCValue) const; // return Amp Weight of this event for this channel
    ClassDef(TBoardCaliResult, 1);
};

class CaliManager
{
public:
    CaliManager(string s = "CaliResult.root");
    ~CaliManager();
    static CaliManager *&CurrentCaliManager();

    bool IsCalibrated() const { return fIsCalibrated; }
    bool CanBeCalibrated() const { return fCanBeCalibrated; }
    bool IsUseful() const { return fIsCalibrated || fCanBeCalibrated || fCaliSuccess; }

    void Show() const;
    ostream &Print(ostream &cout) const;

    const TBoardCaliResult *GetCaliResult(int boardInedx) const { return fBoardCaliResults[boardInedx]; }
    const TBoardCaliResult *GetCaliResultByMac(UChar_t boardMac) const { return fBoardCaliResults[gConfigure->GetBoardIndexByMac(boardMac)]; }
    double GetSignalWeight(int board, int channel, double ADCValue) const { return fBoardCaliResults[board]->GetSignalWeight(channel, ADCValue); }

private:
    int fBoards;
    bool fIsCalibrated = 0;    // Mark whether is write mode or not
    bool fCanBeCalibrated = 0; // Mark whether is read mode or not

    TFile *fCaliFile = NULL;
    vector<TBoardCaliResult *> fBoardCaliResults;
    vector<int> fCalibrateOrder;

    void RefreshState();
    bool JudgeCanBeCalibrated();
    bool JudgeIsCalibrated();

    bool fCaliSuccess = 0; // Mark whether all boards calibrated successfully

    TFile *fOriginalFile = NULL;
    TFile *fPlateStripFile = NULL;

    void CaliFromAllOriginFile(); // Calibrate boards from AllOrigin.root file, which contains all useful information, including pedestal and useful signal

    TBoardCaliResult *CalibrateBoard(int boardIndex); // Calibrate specific Board
};

double GetCRPeakSquare(TH1 *h, double cut = 300);
double GetCRPeakTri(TH1*h, double cut = 300);


void ExtractArgument(double QMax, double QSigma, double NMax, double beta);

double GetCRCaliPlate(TH1*h);



#define gCaliManager (CaliManager::CurrentCaliManager())

#ifdef __ROOTCLING__
#pragma link C++ class TBoardCaliResult;
#endif

#endif