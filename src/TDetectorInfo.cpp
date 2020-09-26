#include "TDetectorInfo.h"

ClassImp(TDetectorInfo);
ClassImp(TGainCoefficient);

TDetectorInfo::TDetectorInfo(int DetectorNo) : detector_no(DetectorNo)
{
    X_layer_no = gConfigure->GetBoardNumberX(detector_no);
    Y_layer_no = gConfigure->GetBoardNumberY(detector_no);

    X_layer_mac = gConfigure->MacArray(detector_no, 0);
    Y_layer_mac = gConfigure->MacArray(detector_no, 1);

    GenerateGainCoeffient();
}

TDetectorInfo::~TDetectorInfo()
{
    delete[] X_layer_mac;
    delete[] Y_layer_mac;
    X_layer_mac = Y_layer_mac = NULL;

    ClearGainCoeffient();
}

TDetectorInfo *&TDetectorInfo::CurrentDetectorInfo()
{
    static TDetectorInfo *currentDetectorInfo = gFileManager->GetDetectorInfo();
    if (!currentDetectorInfo)
    {
        cout << "Creating Detector information." << endl;
        currentDetectorInfo = new TDetectorInfo(0);
    }
    return currentDetectorInfo;
}

int TDetectorInfo::GetADCGain(int ch, bool XY)
{
    int board = ch / 32;
    int adcCh = ch % 32;
    if (XY == 0)
    {
        if (board >= X_layer_no)
        {
            return 0;
        }
        return fXLayerGain->GetCoeffient(adcCh);
    }
    if (board >= Y_layer_no)
    {
        return 0;
    }
    return fYLayerGain->GetCoeffient(adcCh);
}

void TDetectorInfo::GenerateGainCoeffient()
{
    if (fXLayerGain || fYLayerGain)
    {
        ClearGainCoeffient();
    }
    fXLayerGain = new TGainCoefficient[X_layer_no];
    fYLayerGain = new TGainCoefficient[Y_layer_no];
    for (int i = 0; i < X_layer_no; i++)
    {
        fXLayerGain[i].SetMac(X_layer_mac[i]);
    }
    for (int i = 0; i < Y_layer_no; i++)
    {
        fYLayerGain[i].SetMac(Y_layer_mac[i]);
    }
}

void TDetectorInfo::ClearGainCoeffient()
{
    if (fXLayerGain)
    {
        delete[] fXLayerGain;
        fXLayerGain = NULL;
    }
    if (fYLayerGain)
    {
        delete[] fYLayerGain;
        fXLayerGain = NULL;
    }
}

void TDetectorInfo::Print(ostream &os) const
{
    os << "Detector Information: " << endl;
    os << "Detector number: " << detector_no << endl;

    os << "X Layer: " << endl;
    os << "X Layer number: " << X_layer_no << endl;
    for (int i = 0; i < X_layer_no; i++)
    {
        os << "Mac5: " << X_layer_mac[i] << endl;
        fXLayerGain[i].Print();
    }

    os << "Y Layer: " << endl;
    os << "Y Layer number: " << Y_layer_no << endl;
    for (int i = 0; i < Y_layer_no; i++)
    {
        os << "Mac5: " << Y_layer_mac[i] << endl;
        fYLayerGain[i].Print();
    }
}

int TDetectorInfo::GetMac5(bool XY, int index) const
{
    if (!XY) // XY == 0, for X
    {
        if (index >= X_layer_no)
            return -1;
        return X_layer_mac[index];
    }

    if (index >= Y_layer_no)
        return -1;
    return Y_layer_mac[index];
}

TGainCoefficient::TGainCoefficient(UChar_t mac5, string filename) : fMac5(mac5)
{
    ifstream fin(filename);
    if (fin.is_open() == false)
    {
        for (int i = 0; i < 32; i++)
        {
            fCoeffient[i] = 1;
            return;
        }
    }

    string temp;
    for (int i = 0; i < 32; i++)
    {
        if (fin.eof() == true)
        {
            fCoeffient[i] = 1;
        }
        else
        {
            fin >> temp;
            double convert(0);
            try
            {
                convert = stof(temp);
            }
            catch (invalid_argument a)
            {
                cerr << "invalid value" << endl;
                fCoeffient[i] = 1;
                continue;
            }
            fCoeffient[i] = convert;
        }
    }
    fin.close();

    fMacSet = 1;
}

TGainCoefficient::TGainCoefficient(UChar_t mac5) : TGainCoefficient(mac5, (string) "Mac-" + to_string(mac5) + "-GainConfig.txt")
{
    fMacSet = 1;
}

void TGainCoefficient::SetMac(UChar_t mac5, string filename)
{
    if (fMacSet)
    {
        cerr << "Error! Reset mac" << endl;
        return;
    }

    fMac5 = mac5;

    ifstream fin(filename);
    if (fin.is_open() == false)
    {
        // cerr << "Warning: Use default gain coefficience." << endl;
        for (int i = 0; i < 32; i++)
        {
            fCoeffient[i] = 1;
        }
        return;
    }

    string temp;
    for (int i = 0; i < 32; i++)
    {
        if (fin.eof() == true)
        {
            fCoeffient[i] = 1;
        }
        else
        {
            fin >> temp;
            double convert(0);
            try
            {
                convert = stof(temp);
            }
            catch (invalid_argument a)
            {
                cerr << "invalid value" << endl;
                fCoeffient[i] = 1;
                continue;
            }
            fCoeffient[i] = convert;
        }
    }
    fin.close();

    fMacSet = 1;
}

void TGainCoefficient::SetMac(UChar_t mac5)
{
    SetMac(mac5, (string) "Mac-" + to_string(mac5) + "-GainConfig.txt");
}

double TGainCoefficient::GetCoeffient(int channel) const
{
    return fCoeffient[channel];
}

void TGainCoefficient::Print(ostream &os) const
{
    os << "Mac5: " << (int)fMac5 << endl;
    for (int i = 0; i < 32; i++)
    {
        os << "channel: " << i << " Gain: " << GetCoeffient(i) << endl;
    }
}