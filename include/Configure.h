#ifndef CONFIG_H
#define CONFIG_H
#include <TROOT.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "SortWeight.h"
using namespace std;

#include "TDetectorInfo.h"
class TDetectorInfo;

enum BoardType
{
    Square,
    Tri,
    Plate,
    Default
}; // Define 3 boards

// All data must be global constants. If want to define global variables, declear here and define in Configure.cpp.

extern int Loop_Size; // Needn't change

extern int Search_Index_Max; //  Extract buffer(in case some data is invalid)

class DetectorConfig
{
public:
    DetectorConfig();
    DetectorConfig(string filename);

    void ResetConfig(string filename);

    int GetBoardIndexByMac(UChar_t Mac) const; // Judge Mac index in mac array

    int GetBoardNumberX(int detector) const; // Get detector boards counts
    int GetBoardNumberY(int detector) const;
    int GetBoardNumber(int detector, bool X_Y) const;
    int GetBoardNumber(int detector) const;

    int GetTotalBoardNumber() const;
    int GetTotalDetectorNumber() const { return fDetectorCount; }

    int GetMacStartIndexX(int detector); // Get start mac index of a detector
    int GetMacStartIndexY(int detector);
    int GetMacStartIndex(int detector, bool X_Y);
    int GetIndex(int detector, bool X_Y, int boardNo);

    UChar_t *MacArray(int detector, bool X_Y);

    int GetDetectorNumByMac5(UChar_t mac5) const;    // Get detector No. by mac5
    int GetDetectorNumByBoard(int boardindex) const; // Get detector No. by board index

    int GetXYByMac5(UChar_t mac5) const;    // Get X/Y detector by mac5
    int GetXYByBoardIndex(int index) const; // Get X/Y detector by mac5

    bool IncrementClockByMac5(UChar_t mac5); // Used only when reading data from original file
    bool IncrementClockByIndex(int index);
    int GetClockByIndex(int index) const;

    int BoardCounts();
    const vector<UChar_t> &MacVector() { return fBoardMacArray; }
    UChar_t GetMac(int index) const;

    static DetectorConfig *&CurrentDetectorConfig();

    void ReadConfig();
    void PrintConfig(ostream &);

    // bool AddDetector(int detector, const TDetectorInfo& info);  // Add Detector in the end, variable detector should be related to fDetectorCount

    bool SetBoardInvalidChannel(UChar_t mac5, vector<int> &ch);
    bool JudgeValidChannel(UChar_t mac5, int ch);

    void Clear();
    bool HeightIsAllSet() const;
    ostream &OutputHeightMissedDetector(ostream &) const;
    ostream &OutputHeightInformation(ostream &) const;

    bool SetHeightByDetecIndex(int index, double height);
    double GetHeightByMac5(UChar_t mac5) const;
    double GetHeightByDetecIndex(int index) const;
    double GetHeightByBoardIndex(int index) const;
    int GetDetectorNumByHeightIndex(int index) const { return fHeightToDetector[index]; }   // Get detector index for this layer
    int GetLayerNumberByDetectorIndex(int index) const { return fDetectorToHeight[index]; } // Get which layer is this detector on

    static BoardType JudgeDetectorType(string word);
    static string JudgeDetectorType(BoardType);
    BoardType GetDetectorTypeByMac(UChar_t mac);

private:
    int fDetectorCount;                  // Total detectors count
    vector<double> fDetectorHeightArray; // Height for each detector
    vector<int> fHeightToDetector;       // Determine what the lowest to highest detector is. 0 represents for lowest detector, while the last one is highest detector
    vector<int> fDetectorToHeight;       // Determine which layer is this detector on, 0 is the lowest
    vector<bool> fHeightFlagArray;       // Stash whether height information is set

    int fLayerCount;               //  Layers in total.
    vector<int> fNBoardLayerArray; // Boards in each layer.

    vector<UChar_t> fBoardMacArray;    // Board MACs
    vector<BoardType> fBoardTypeArray; //Board Type
    vector<vector<int>> fInvalidCh;    // Invalid Channels in board

    vector<int> fBoardClockCounter; // Board clock counter

    int fBoardCount = 0; // Total counts of boards.

    int Search_Index_Max; //  Extract buffer(in case some data is invalid)
    string fConfigFileName;

    bool RefreshHeightData();
};

// extern DetectorConfig *gConfigure;
#define gConfigure (DetectorConfig::CurrentDetectorConfig())

#endif
