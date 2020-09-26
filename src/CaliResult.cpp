#include "CaliResult.h"
ClassImp(TBoardCaliResult);

double TBoardCaliResult::GetSignalWeight(int channel, double ADCValue) const
{
    if (fAmplitude[channel] == 0)
    {
        return 0;
    }
    return (ADCValue - fPed[channel]) / fAmplitude[channel];
}

CaliManager::CaliManager(string s)
{
    fBoards = gConfigure->BoardCounts();
    // cout << "Boards: " << fBoards << endl;
    // cout << "Mac: " << (int)gConfigure->GetMac(1) << endl;
    for (int i = 0; i < fBoards; i++)
    {
        fBoardCaliResults.push_back(NULL);
    }

    bool isCalibrated = JudgeIsCalibrated();
    bool canbeCalibrated = JudgeCanBeCalibrated();
    if (isCalibrated) // in case of extract from file
    {
        cout << "Extracting Calibrate result from File" << endl;
        // fCaliFile = new TFile("CaliResult.root");
        fCaliFile = new TFile(s.c_str());
        for (int i = 0; i < fBoards; i++)
        {
            auto temp = (TBoardCaliResult *)fCaliFile->Get(Form("CaliResultBoard%d", (int)gConfigure->GetMac(i)));
            if (temp == NULL)
            {
                cout << "Warning: Error getting board " << i << " mac: " << (int)gConfigure->GetMac(i) << " calibrate result" << endl;
                cout << "This board need recalibration from AllOrigin.root, and Calibrate result won't be saved in file." << endl;
                fCalibrateOrder.push_back(i);
            }
            fBoardCaliResults[i] = temp;
        }

        if (fCalibrateOrder.size() == 0)
        {
            cout << "Extract all calibrate result from CaliResult.root successfully." << endl;
            fIsCalibrated = 1;
            fCaliSuccess = 1;
        }
        else
        {
            for (int i = 0; i < fCalibrateOrder.size(); i++)
            {
                int boardNo = fCalibrateOrder[i];
                auto calitemp = CalibrateBoard(boardNo);
                if (calitemp == NULL)
                {
                    CaliException cali(boardNo);
                    throw cali;
                }
                fBoardCaliResults[boardNo] = calitemp;
            }
            fIsCalibrated = 1;
            fCaliSuccess = 1;
        }
    }
    else if (canbeCalibrated) // in case of hasn't been calibrated but have original file
    {
        cout << "Calibrating: " << endl;
        fCaliFile = new TFile("CaliResult.root", "recreate");
        fCanBeCalibrated = 1;
        for (int i = 0; i < fBoards; i++)
        {
            cout << "Board: " << i << endl;
            auto calitemp = CalibrateBoard(i);
            if (calitemp == NULL)
            {
                CaliException cali(i);
                throw cali;
            }
            fBoardCaliResults[i] = calitemp;
            fCaliFile->cd();
            calitemp->Write(Form("CaliResultBoard%d", (int)gConfigure->GetMac(i)));
        }
        fCaliSuccess = 1;
        fIsCalibrated = 0;
    }
    else
    {
        throw CaliException(-1);
    }
}

CaliManager::~CaliManager()
{
    fPlateStripFile->Close();
    fOriginalFile->Close();
    fCaliFile->Close();

    delete fCaliFile;
    fCaliFile = NULL;
    for (int i = 0; i < fBoards; i++)
    {
        delete fBoardCaliResults[i];
    }

    delete fOriginalFile;
    delete fPlateStripFile;
    fOriginalFile = NULL;
    fPlateStripFile = NULL;
}

// CaliManager::~CaliManager()
// {
//     fCaliFile->Close();
//     for (int i = 0; i < fBoards; i++)
//     {
//         delete fBoardCaliResults[i];
//         fBoardCaliResults[i] = NULL;
//     }
// }

bool CaliManager::JudgeCanBeCalibrated()
{
    ifstream fin;
    fin.open("AllOrigin.root");
    if (fin.is_open())
    {
        fin.close();
        return 1;
    }
    return 0;
}

bool CaliManager::JudgeIsCalibrated()
{
    ifstream fin;
    fin.open("CaliResult.root");
    if (fin.is_open())
    {
        fin.close();
        return 1;
    }
    return 0;
}

TBoardCaliResult *CaliManager::CalibrateBoard(int boardIndex)
{
    gStyle->SetOptFit(0111);
    int mac5 = gConfigure->GetMac(boardIndex);
    BoardType type = gConfigure->GetDetectorTypeByMac(mac5);

    if (type == Plate && !fPlateStripFile)
    {
        ifstream fin("PlateSpectrum.root");
        if (fin.is_open() == false)
        {
            gSystem->Exec("make PlateSpectrum");
            gSystem->Exec("./PlateSpectrum");
        }
        fin.close();
        fPlateStripFile = new TFile("PlateSpectrum.root");
    }

    TDirectory *dirPlateStrip = NULL;
    if (type == Plate)
    {
        dirPlateStrip = fPlateStripFile->GetDirectory(Form("Board%d", mac5));
    }

    if (!fOriginalFile)
    {
        fOriginalFile = new TFile("AllOrigin.root");
    }
    auto dir = (TDirectory *)fOriginalFile->Get(Form("Board%d", mac5));
    auto caliResult = new TBoardCaliResult;

    TCanvas c("c", "c", 1);

    gSystem->Exec("mkdir Calibrate-Result");
    gSystem->Exec("cp ConfigFile Calibrate-Result");

    caliResult->fMac5 = mac5;
    for (int ch = 0; ch < 32; ch++)
    {
        cout << "Calibrating ch: " << ch << endl;
        auto hPed = (TH1D *)dir->Get(Form("hPed%dBoard%d", ch, mac5));
        auto hSignal = (TH1D *)dir->Get(Form("hSignal%dBoard%d", ch, mac5));

        hPed->Fit("gaus", "Q", "", 0, 500);

        auto pedfun = hPed->GetFunction("gaus");
        double pedtemp = pedfun->GetParameter(1);
        double pedSigma = pedfun->GetParameter(2);
        caliResult->fPed[ch] = pedtemp;

        if (type == Square)
        {
            caliResult->fAmplitude[ch] = GetCRPeakSquare(hSignal, pedtemp) - pedtemp;
        }
        else if (type == Tri)
        {
            caliResult->fAmplitude[ch] = GetCRPeakTri(hSignal, 450) - pedtemp; // cut is useful for fitting
        }
        else if (type == Plate) // Signal of Plate is too small to calibrate for each channel, instead, calibrate 2 adjoining channels
        {
            int gr = ch / 2;
            auto htemp = (TH1D *)dirPlateStrip->Get(Form("Strip%d", gr));
            if (ch % 2 == 0)
            {
                double amp = GetCRCaliPlate(htemp);
                cout << amp << endl;
                caliResult->fAmplitude[ch] = amp;
            }
            else
            {
                caliResult->fAmplitude[ch] = caliResult->fAmplitude[ch - 1];
            }

            hSignal = htemp;
        }

        c.SetLogy();
        c.cd();
        hSignal->Draw();
        c.SaveAs(Form("h%dBoard%dCaliResult.pdf", ch, mac5));
        gSystem->Exec(Form("mv h%dBoard%dCaliResult.pdf Calibrate-Result", ch, mac5));
    }

    return caliResult;
}

double GetCRPeakSquare(TH1 *h, double cut)
{
    h->Rebin(16);
    h->ResetStats();
    int maximumBin = h->GetMaximumBin();
    h->SetBinContent(h->FindBin(4095), 0);

    // First Set all bins before cut as 0
    // double temp = h->GetBinCenter(maximumBin) + 200;
    for (int i = 0; i < h->FindBin(cut); i++)
    {
        h->SetBinContent(i, 0);
    }
    h->ResetStats();
    maximumBin = h->GetMaximumBin();

    // Judge if can find maximum bin correctly
    for (int i = h->FindBin(cut); i < h->GetNbinsX(); i++)
    {
        if (maximumBin > i + 10)
        {
            break;
        }

        h->SetBinContent(i, 0);
        h->ResetStats();
        maximumBin = h->GetMaximumBin();
    }

    h->Fit("gaus", "Q", "", 0.5 * h->GetBinCenter(maximumBin), 1.5 * h->GetBinCenter(maximumBin));
    double peak = h->GetFunction("gaus")->GetParameter(1);
    double sigma = h->GetFunction("gaus")->GetParameter(2);
    cout << sigma << '\t' << peak << '\t' << sigma / TMath::Sqrt(peak) << endl;
    // cout << "JohnTest: (Q2)/#sigma2# " << (peak - cut) * (peak - cut) / (sigma * sigma) << endl;
    if (peak < 300 || peak > 4000)
    {
        cout << "Error while fitting CR signal:\t";
        cout << h->GetDirectory()->GetName() << '\t';
        cout << h->GetName() << '\t';
        cout << "Cut: " << cut << '\t';
        cout << endl;
        return -1;
    }
    return peak;
}

CaliManager *&CaliManager::CurrentCaliManager()
{
    static auto currentCaliManager = new CaliManager();
    return currentCaliManager;
}

void CaliManager::Show() const
{
    Print(cout);
}

ostream &CaliManager::Print(ostream &os) const
{
    for (int board = 0; board < fBoards; board++)
    {
        os << "Board: " << board << endl;
        os << "Mac: " << (int)fBoardCaliResults[board]->fMac5 << endl;
        for (int ch = 0; ch < 32; ch++)
        {
            os << "Channel: " << ch << endl;
            os << "Pedestal: " << fBoardCaliResults[board]->fPed[ch] << endl;
            os << "Amplitude: " << fBoardCaliResults[board]->fAmplitude[ch] << endl;
        }
    }
    return os;
}

double TriDetectorFitFun(double *xs, double *par)
{
    double Q = xs[0];
    double Factor = par[0];
    double QMax = par[1];
    double QSigma = par[2];

    double beta = QSigma * QSigma / 2 / QMax;

    double Term1 = 1.0 / QMax;
    double Term2 = Term1 * TMath::Erf((QMax - Q) / QSigma);
    double Term3 = Term1 * TMath::Exp(Q / beta) * (-1 + TMath::Erf((Q + QMax) / QSigma));
    // double Term4 = Term1 * Term3 * TMath::Erf((Q + QMax) / QSigma);

    // cout << Term3 << endl;
    return Factor * (Term1 + Term2 + Term3);
}

double FitFunErf(double *xs, double *par)
{
    double Q = xs[0];
    double Factor = par[0];
    double QMax = par[1];
    double QSigma = par[2];

    double beta = QSigma * QSigma / 2 / QMax;

    double Term1 = 1.0 / QMax;
    double Term2 = Term1 * TMath::Erf((QMax - Q) / QSigma);
    // double Term3 = Term1 * TMath::Exp(2 * Q * QMax / QSigma / QSigma) * (-1 + TMath::Erf((Q + QMax) / QSigma));
    double Term3 = 0;
    // double Term4 = Term1 * Term3 * TMath::Erf((Q + QMax) / QSigma);

    return Factor * (Term1 + Term2 + Term3);
}

double FitFunPhotonNum(double *xs, double *par)
{
    double n = xs[0];
    double Factor = par[0];
    double NMax = par[1];

    // double beta = QSigma * QSigma / 2 / NMax;

    double Term1 = 1.0 / NMax;
    double Term2 = Term1 * TMath::Erf((NMax - n) / TMath::Sqrt(2 * NMax));
    double Term3 = Term1 * TMath::Exp(n) * (-1 + TMath::Erf((n + NMax) / TMath::Sqrt(2 * NMax)));
    // double Term4 = Term1 * Term3 *;

    double y = Factor * (Term1 + Term2 + Term3);

    return y;
}

double GetCRPeakTri(TH1 *h, double cut)
{
    auto fitfun = new TF1("fitfun", TriDetectorFitFun, 0, 4096, 3);
    fitfun->SetParName(0, "Counts");
    fitfun->SetParName(1, "QMax");
    fitfun->SetParName(2, "QSigma");

    fitfun->SetParameter(0, 10000);
    fitfun->SetParameter(1, 3000);
    fitfun->SetParameter(2, 300);
    fitfun->SetRange(0, 4096);
    h->Rebin(16);
    h->SetBinContent(h->FindBin(4095), 0);

    h->GetXaxis()->SetRangeUser(cut, 4096);

    h->Fit(fitfun, "Q", "", cut, 4096);

    double result;
    double QMax;
    double QSigma;
    QMax = fitfun->GetParameter(1);
    QSigma = fitfun->GetParameter(2);

    // h->Fit(fitfun,"Q","",QMax-2*QSigma, 4096);
    result = fitfun->GetParameter(1);
    QMax = fitfun->GetParameter(1);
    QSigma = fitfun->GetParameter(2);

    double NMax, beta;
    ExtractArgument(QMax, QSigma, NMax, beta);
    delete fitfun;

    return result;
}

void ExtractArgument(double QMax, double QSigma, double NMax, double beta)
{
    double temp = QMax / QSigma;
    NMax = temp * temp * 2;
    beta = QMax / NMax;
    cout << "QMax: " << QMax << '\t' << "QSigma: " << QSigma << endl;
    cout << "NMax: " << NMax << '\t' << "beta: " << beta << endl;
    return;
}

double FitFunErf2(double *xs, double *par)
{
    double result = par[0] * (1 + TMath::Erf((par[0] - xs[0]) / par[1]));
    return result;
}

double GetCRCaliPlate(TH1 *h)
{
    auto fitfun = new TF1("Erf", FitFunErf2, 0, 4096, 3);
    gStyle->SetOptFit(0111);

    fitfun->SetParameter(0, 500);
    fitfun->SetParameter(1, 1000);
    fitfun->SetParameter(2, 1000);
    
    fitfun->SetParLimits(1,0,4096);
    fitfun->SetParLimits(2,0,4096);
    
    fitfun->SetRange(0, 4096);

    h->Rebin(8);
    h->GetXaxis()->SetRangeUser(300, 4096);

    int NFitTimes = 1;
    double FitStart = h->GetBinCenter(h->GetMaximumBin()) + 100;
    double Fraction = 2.0 / 3;
    double FitEnd = 0;
    for (int i = h->FindBin(300); i < h->GetNbinsX(); i++)
    {
        if (h->GetBinContent(i) == 0)
        {
            FitEnd = h->GetBinCenter(i) + 500;
            break;
        }
        FitEnd = h->GetBinCenter(i);
    }

    for (int i = 0; i < NFitTimes; i++)
    {
        double endfit = Fraction * 4096;
        double interval = endfit - FitStart;
        double start = FitStart + (double)i / NFitTimes * interval;

        h->Fit(fitfun, "Q", "", start, FitEnd);
        double result = fitfun->GetParameter(1);
    }

    double mean = fitfun->GetParameter(1);
    double sigma = fitfun->GetParameter(2);
    cout << "Fit Mean: " << mean << endl;
    cout << "Fit Sigma: " << sigma << endl;

    delete fitfun;

    return mean + 3 * sigma;
}