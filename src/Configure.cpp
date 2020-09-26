#define Configure_cxx
#include "Configure.h"

int Loop_Size = 200;
int Search_Index_Max = 5;

DetectorConfig::DetectorConfig() : fDetectorCount(0), fLayerCount(0), fBoardCount(0), Search_Index_Max(5)
{
    // cout << "Construct detector config by default constructor" << endl;
    PrintConfig(cout);
}

DetectorConfig::DetectorConfig(string filename) : fDetectorCount(1), fLayerCount(fDetectorCount * 2), fNBoardLayerArray{1, 1}, fBoardMacArray{0, 85}, fBoardCount(2), fConfigFileName(filename)
{
    ReadConfig();
    cout << "Read configuration file successfully." << endl;
    PrintConfig(cout);
}

void DetectorConfig::ResetConfig(string filename)
{
    Clear();
    fConfigFileName = filename;
    ReadConfig();
    PrintConfig(cout);
}

int DetectorConfig::GetBoardIndexByMac(UChar_t Mac) const
{
    for (int i = 0; i < fBoardCount; i++)
    {
        if (fBoardMacArray[i] == Mac)
        {
            return i;
        }
    }
    return -1;
}

int DetectorConfig::GetBoardNumberX(int detector) const
{
    if (detector >= fDetectorCount)
    {
        cout << "Error: Input detector number is larger than detector limits, please check input!" << endl;
        range_error r("Range Error");
        throw r;
    }
    return fNBoardLayerArray[detector * 2];
}

int DetectorConfig::GetBoardNumberY(int detector) const
{
    if (detector >= fDetectorCount)
    {
        cout << "Error: Input detector number is larger than detector limits, please check input!" << endl;
        range_error r("Range Error");
        throw r;
    }
    return fNBoardLayerArray[detector * 2 + 1];
}

int DetectorConfig::GetBoardNumber(int detector, bool X_Y) const
{
    return (X_Y) ? GetBoardNumberY(detector) : GetBoardNumberX(detector);
}

int DetectorConfig::GetBoardNumber(int detector) const
{
    return GetBoardNumberX(detector) + GetBoardNumberY(detector);
}

int DetectorConfig::GetMacStartIndexX(int detector) // Get Mac start index.
{
    int LayerCut = detector * 2;
    int AddTemp = 0;
    for (int i = 0; i < detector; i++)
    {
        AddTemp += GetBoardNumber(i);
    }
    return AddTemp;
}

int DetectorConfig::GetMacStartIndexY(int detector)
{
    return GetMacStartIndexX(detector) + fNBoardLayerArray[2 * detector];
}

int DetectorConfig::GetMacStartIndex(int detector, bool X_Y)
{
    return (X_Y) ? GetMacStartIndexY(detector) : GetMacStartIndexX(detector);
}

int DetectorConfig::GetIndex(int detector, bool X_Y, int boardNo)
{
    int startIndex = GetMacStartIndex(detector, X_Y);
    int boardTotal = GetBoardNumber(detector, X_Y);
    if (boardNo >= boardTotal)
    {
        return -1;
    }
    return startIndex + boardNo;
}

UChar_t *DetectorConfig::MacArray(int detector, bool X_Y)
{
    UChar_t *macaddress(NULL);
    int Start_Index = GetMacStartIndex(detector, X_Y);
    int Mac_Number = GetBoardNumber(detector, X_Y);
    macaddress = new UChar_t[Mac_Number];
    for (int i = 0; i < Mac_Number; i++)
    {
        macaddress[i] = fBoardMacArray[Start_Index + i];
    }
    return macaddress;
}

DetectorConfig *&DetectorConfig::CurrentDetectorConfig()
{
    static DetectorConfig *currentDetectorConfig = new DetectorConfig("ConfigFile");
    return currentDetectorConfig;
}

void DetectorConfig::ReadConfig()
{
    fBoardCount = 0;
    ifstream fin;
    fin.open(fConfigFileName);
    if (fin.is_open() == false)
    {
        cerr << "Error: Configuration file does not exist, using default config" << endl;
        PrintConfig(cerr);
        fin.close();
        return;
    }
    stringstream ss;
    string line;
    string word;

    int tDetectorCount;
    int tDetectorCountVerify = 0;

    int tLayerNo;
    vector<int> tBoardNo;
    vector<UChar_t> tBoardMac;
    vector<BoardType> tBoardType;

    try
    {
        getline(fin, line);
        ss.clear();
        ss.str(line);
        ss >> word;
        tDetectorCount = stoi(word);

        for (int detector = 0; fin.eof() == false; detector++)
        {
            bool DetectorFlag = 0;     // Judge whether there is a detector flag
            double DetectorHeight = 0; // Stash height of this detector

            // Read lines until find symbol --
            while (fin.good() && fin.is_open())
            {
                getline(fin, line);
                ss.clear();
                ss.str(line);
                ss >> word;
                if (word == to_string(detector))
                {
                    ss >> word;
                    if (word == "--")
                    {
                        DetectorFlag = 1;
                        ss >> word;
                        bool heightflagTemp = 1;
                        if (word == "Height:")
                        {
                            try
                            {
                                ss >> word;
                                DetectorHeight = stod(word);
                                bool heightflagTemp = 1;
                            }
                            catch (invalid_argument err)
                            {
                                cout << "Warning: Wrong input height" << endl;
                            }
                        }
                        else
                        {
                            heightflagTemp = 0;
                            cout << "John: test" << word << endl;
                            cout << "Warning: detector " << detector << ": Not input detector height information" << endl;
                        }
                        if (heightflagTemp)
                        {
                            cout << "Get height successfully:Detector " << detector << "\t" << DetectorHeight << endl;
                            fDetectorHeightArray.push_back(DetectorHeight);
                            fHeightFlagArray.push_back(1);
                        }
                        else
                        {
                            fDetectorHeightArray.push_back(0);
                            fHeightFlagArray.push_back(0);
                        }
                    }
                    break;
                }
            }

            if (!DetectorFlag)
            {
                break;
            }
            // Read 4 lines, which contains board and layer information
            // Read x layer
            // Board number
            tDetectorCountVerify++;
            getline(fin, line);
            ss.clear();
            ss.str(line);
            ss >> word;
            int xlayer = stoi(word);
            tBoardNo.push_back(xlayer);
            fBoardCount += xlayer;

            getline(fin, line);
            ss.clear();
            ss.str(line);
            // Board mac
            for (int board = 0; board < xlayer; board++)
            {
                ss >> word;
                if (word == "#")
                {
                    invalid_argument err("err");
                    throw err;
                }
                UChar_t temp = stoi(word);
                tBoardMac.push_back(temp);
                auto boardtype = JudgeDetectorType(word);
                tBoardType.push_back(boardtype);
            }
            // Read y layer
            // Board number
            getline(fin, line);
            ss.clear();
            ss.str(line);
            ss >> word;
            int ylayer = stoi(word);
            fBoardCount += ylayer;

            tBoardNo.push_back(ylayer);
            getline(fin, line);
            ss.clear();
            ss.str(line);
            // Board mac
            for (int board = 0; board < ylayer; board++)
            {
                ss >> word;
                if (word == "#")
                {
                    invalid_argument err("err");
                    throw err;
                }
                UChar_t temp = stoi(word);
                tBoardMac.push_back(temp);
                auto boardtype = JudgeDetectorType(word);
                tBoardType.push_back(boardtype);
            }
        }

        if (tDetectorCountVerify != tDetectorCount)
        {
            cout << "Warning: Dismatch between input detector count and scanned detector count, use the less one" << endl;
        }
    }
    catch (invalid_argument err)
    {
        cerr << "Invalid input" << endl;
        cerr << "Using default configuration" << endl;
        PrintConfig(cerr);
        return;
    }

    fDetectorCount = tDetectorCountVerify > tDetectorCount ? tDetectorCount : tDetectorCountVerify;
    fLayerCount = tLayerNo;
    fNBoardLayerArray = tBoardNo;
    fBoardMacArray = tBoardMac;
    fBoardTypeArray = tBoardType;

    for (int i = 0; i < fBoardCount; i++)
    {
        fBoardClockCounter.push_back(0);
        vector<int> chTemp;
        fInvalidCh.push_back(chTemp);
    }
    if (HeightIsAllSet())
    {
        RefreshHeightData();
    }
}

BoardType DetectorConfig::JudgeDetectorType(string word)
{
    if (word.find("Squ") != word.npos)
    {
        return Square;
    }
    else if (word.find("Tri") != word.npos)
    {
        return Tri;
    }
    else if (word.find("Pla") != word.npos)
    {
        return Plate;
    }
    else
    {
        return Default;
    }
}

string DetectorConfig::JudgeDetectorType(BoardType type)
{
    if(type == Square)
    {
        return "Square";
    }
    else if(type == Tri)
    {
        return "Triangle";
    }
    else if(type == Plate)
    {
        return "Plate";
    }
    else if(type == Default)
    {
        return "Default";
    }
    return "";
}

bool DetectorConfig::HeightIsAllSet() const
{
    bool value = 1;
    if (fDetectorCount == 0)
        return false;
    for (int i = 0; i < fHeightFlagArray.size(); i++)
    {
        value = value && fHeightFlagArray[i];
    }
    return value;
}

ostream &DetectorConfig::OutputHeightMissedDetector(ostream &os) const
{
    for (int i = 0; i < fHeightFlagArray.size(); i++)
    {
        if (!fHeightFlagArray[i])
        {
            os << "Detector " << i << " Height Info Missing!" << endl;
        }
    }
    return os;
}

bool DetectorConfig::RefreshHeightData()
{
    if (!HeightIsAllSet())
        return false;
    fHeightToDetector.clear();
    fDetectorToHeight.clear();

    fHeightToDetector = SortByAscend(fDetectorHeightArray);
    fDetectorToHeight = sortByAscend(fHeightToDetector);
    return true;
}

ostream &DetectorConfig::OutputHeightInformation(ostream &os) const
{
    if (!HeightIsAllSet())
    {
        os << "Detector Information has not all set yet!" << endl;
        return OutputHeightMissedDetector(os);
    }
    os << "Detector Height Information: " << endl;

    for (int i = 0; i < fDetectorCount; i++)
    {
        os << "Detector No.: " << i << endl;
        os << "Detector Height: " << fDetectorHeightArray[i] << endl;
        os << "Layer:(0 is the lowest) " << fDetectorToHeight[i] << endl
           << endl;
    }
    for (int i = 0; i < fDetectorCount; i++)
    {
        os << "Layer No.: " << i << endl;
        os << "Layer Height: " << fDetectorHeightArray[fHeightToDetector[i]] << endl;
        os << "Detector No.: " << fHeightToDetector[i] << endl
           << endl;
    }
    return os;
}

double DetectorConfig::GetHeightByDetecIndex(int index) const
{
    return fDetectorHeightArray[index];
}

double DetectorConfig::GetHeightByMac5(UChar_t mac5) const
{
    int index = GetBoardIndexByMac(mac5);
    return GetHeightByDetecIndex(index);
}

double DetectorConfig::GetHeightByBoardIndex(int boardindex) const
{
    int detecindex = GetDetectorNumByBoard(boardindex);
    return GetHeightByDetecIndex(detecindex);
}

bool DetectorConfig::SetHeightByDetecIndex(int index, double height)
{
    if (index < 0 || index > fDetectorCount)
        return false;
    if (fHeightFlagArray[index])
    {
        cout << "Warning:  Height of Detector" << index << " has already been set. Resetting." << endl;
    }
    fDetectorHeightArray[index] = height;
    fHeightFlagArray[index] = 1;
    RefreshHeightData();
    return true;
}

int DetectorConfig::GetDetectorNumByBoard(int boardindex) const
{
    int index = 0;
    int boardindextemp = boardindex;
    if (boardindex < 0 || boardindex >= fBoardCount)
    {
        return -1;
    }
    for (int detec = 0; detec < fDetectorCount; detec++)
    {
        boardindextemp -= fNBoardLayerArray[detec * 2];
        boardindextemp -= fNBoardLayerArray[detec * 2 + 1];
        if (boardindextemp < 0)
        {
            return detec;
        }
    }

    return -1;
}

int DetectorConfig::GetXYByBoardIndex(int index) const
{
    if (index < 0 || index > fBoardCount)
        return -1;
    for (int detec = 0; detec < fDetectorCount; detec++)
    {
        index -= fNBoardLayerArray[detec * 2];
        if (index < 0)
            return 0;
        index -= fNBoardLayerArray[detec * 2];
        if (index < 0)
            return 1;
    }
    return -1;
}

int DetectorConfig::GetXYByMac5(UChar_t mac5) const // Get X/Y detector by mac5
{
    int index = GetBoardIndexByMac(mac5);
    return GetXYByBoardIndex(index);
}

int DetectorConfig::GetDetectorNumByMac5(UChar_t mac5) const
{
    int index = 0;
    for (int detec = 0; detec < fDetectorCount; detec++)
    {
        for (int bo = 0; bo < GetBoardNumber(detec); bo++)
        {
            if (GetMac(index) == mac5)
            {
                return detec;
            }
            index++;
        }
    }
    return -1;
}

void DetectorConfig::PrintConfig(ostream &os)
{
    os << fDetectorCount << "# Total detector count" << endl
       << endl;

    for (int detector = 0; detector < fDetectorCount; detector++)
    {
        os << detector << "\t--\t";
        if (fHeightFlagArray[detector])
        {
            os << "Height:\t" << fDetectorHeightArray[detector] << "\t";
        }
        os << "# Detector " << detector << " info" << endl; //", should end with --, height should be just after --, and space must be put before after Height: and height value" << endl;

        int xlayer = GetBoardNumber(detector, 0);
        os << xlayer << " # XLayer board number" << endl;
        UChar_t *xarray = MacArray(detector, 0);
        for (int i = 0; i < xlayer; i++)
        {
            os << (int)xarray[i] << " ";
        }
        delete xarray;
        xarray = NULL;
        os << "# Board Mac, use space to seperate different boards" << endl;

        int ylayer = GetBoardNumber(detector, 1);
        os << ylayer << " # YLayer board number" << endl;
        UChar_t *yarray = MacArray(detector, 1);
        for (int i = 0; i < ylayer; i++)
        {
            os << (int)yarray[i] << " ";
        }
        delete yarray;
        yarray = NULL;
        os << "# Board Mac, use space to seperate different boards" << endl
           << endl;
    }
}

void DetectorConfig::Clear()
{
    fDetectorCount = 0;
    fLayerCount = 0;

    fDetectorHeightArray.clear();
    fHeightToDetector.clear();
    fHeightFlagArray.clear();
    fDetectorToHeight.clear();

    fNBoardLayerArray.clear();
    fBoardMacArray.clear();
    fBoardTypeArray.clear();
    fInvalidCh.clear();
    fBoardClockCounter.clear();
    fBoardCount = 0;
}

// bool DetectorConfig::AddDetector(int detector, const TDetectorInfo &info)
// {
//     if (detector != fDetectorCount)
//     {
//         return false;
//     }

//     fDetectorCount++;
//     fLayerCount += 2;

//     int xlayer = info.GetXLayerNum();
//     int ylayer = info.GetYLayerNum();
//     fNBoardLayerArray.push_back(xlayer);
//     fNBoardLayerArray.push_back(ylayer);

//     for (int i = 0; i < xlayer; i++)
//     {
//         int mac5Temp = info.GetMac5(0, i);
//         if (mac5Temp > 0)
//         {
//             fBoardMacArray.push_back(mac5Temp);
//             vector<int> chTemp;
//             fInvalidCh.push_back(chTemp);
//             fBoardClockCounter.push_back(0);
//         }
//     }
//     for (int i = 0; i < ylayer; i++)
//     {
//         int mac5Temp = info.GetMac5(1, i);
//         if (mac5Temp > 0)
//         {
//             fBoardMacArray.push_back(mac5Temp);
//             vector<int> chTemp;
//             fInvalidCh.push_back(chTemp);
//             fBoardClockCounter.push_back(0);
//         }
//     }
//     return true;
// }

BoardType DetectorConfig::GetDetectorTypeByMac(UChar_t mac)
{
    int boardindex = GetBoardIndexByMac(mac);
    return fBoardTypeArray[boardindex];
}

bool DetectorConfig::IncrementClockByMac5(UChar_t mac5)
{
    int temp = GetBoardIndexByMac(mac5);
    if (temp < 0)
    {
        cerr << "Warning: Cannot find mac: " << (int)mac5 << endl;
        return 0;
    }
    fBoardClockCounter[temp]++;
    return true;
}

bool DetectorConfig::IncrementClockByIndex(int index)
{
    if (index < 0 || index >= fBoardCount)
    {
        cerr << "Warning: Mac index error!" << endl;
        return false;
    }
    fBoardClockCounter[index]++;
    return true;
}

int DetectorConfig::GetClockByIndex(int index) const
{
    if (index < 0 || index >= fBoardCount)
    {
        cerr << "Warning: Mac index error!" << endl;
        return -1;
    }
    return fBoardClockCounter[index];
}

bool DetectorConfig::SetBoardInvalidChannel(UChar_t mac5, vector<int> &ch)
{
    int boardIndex = GetBoardIndexByMac(mac5);
    if (boardIndex < 0)
        return 0;

    fInvalidCh[boardIndex] = ch;
    return true;
}
bool DetectorConfig::JudgeValidChannel(UChar_t mac5, int chNo)
{
    int boardIndex = GetBoardIndexByMac(mac5);
    if (boardIndex < 0)
        return 0;
    if (chNo < 0 || chNo > 31)
    {
        return 0;
    }

    vector<int> &ch = fInvalidCh[boardIndex];
    for (int i = 0; i < ch.size(); i++)
    {
        if (chNo == ch[i])
        {
            return 0;
        }
    }
    return true;
}

UChar_t DetectorConfig::GetMac(int index) const
{
    if (index >= fBoardCount)
    {
        return -1;
    }
    return fBoardMacArray[index];
}

int DetectorConfig::GetTotalBoardNumber() const
{
    return fBoardCount;
}

int DetectorConfig::BoardCounts()
{
    // cout << "Board Count: " << fBoardCount << endl;
    return fBoardCount;
}