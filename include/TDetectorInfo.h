#ifndef TDetectorInfo_H
#define TDetectorInfo_H

#include "FileManager.h"
#include "TObject.h"
#include "Configure.h"
#include <ostream>

class TGainCoefficient;

class TDetectorInfo : public TObject
{
public:
    TDetectorInfo() { X_layer_mac = Y_layer_mac = NULL; };
    TDetectorInfo(int detectorNo);
    ~TDetectorInfo();

    int GetXLayerNum() const { return X_layer_no; }
    int GetYLayerNum() const { return Y_layer_no; }

    int GetMac5(bool XY, int index) const;

    int GetADCGain(int adcChan, bool XY);
    void Print(ostream &os = cout) const;

    static TDetectorInfo *&CurrentDetectorInfo();

private:
    int detector_no;
    int X_layer_no;
    int Y_layer_no;

    UChar_t *X_layer_mac; //[X_layer_no]
    UChar_t *Y_layer_mac; //[Y_layer_no]

    TGainCoefficient *fXLayerGain = NULL; //![X_layer_no] There's a bug here
    TGainCoefficient *fYLayerGain = NULL; //![Y_layer_no]
    // TGainCoefficient *fYLayerGain2;    //->

    void GenerateGainCoeffient();
    void ClearGainCoeffient();

    ClassDef(TDetectorInfo, 1);
};

#define gDetectorInfo (TDetectorInfo::CurrentDetectorInfo())

class TGainCoefficient : public TObject // Board Gain Coefficient
{
    friend class TDetectorInfo;

public:
    TGainCoefficient() = default;
    TGainCoefficient(UChar_t mac5);
    TGainCoefficient(UChar_t mac5, string filename);

    void SetMac(UChar_t mac5);
    void SetMac(UChar_t mac5, string filename);

    void Print(ostream &os = cout) const;

private:
    UChar_t fMac5;         // Mac5
    double fCoeffient[32]; // Gain Coeffient of Board

    bool fMacSet = 0; //!

    double GetCoeffient(int channel) const;

    ClassDef(TGainCoefficient, 1);
};

#endif