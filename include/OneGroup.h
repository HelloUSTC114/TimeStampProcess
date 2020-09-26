#ifndef DETECTOR_MANAGER_H
#define DETECTOR_MANAGER_H
#include "TROOT.h"

#include "Loop.h"
#include "CombineData.h"
#include "Configure.h"
#include "FileManager.h"

class DetectorLoop : public Loop<CombinedData>
{
public:
    DetectorLoop(int Loop_size, FileManager *);
    ~DetectorLoop();
    // Add and judge whether previous data should be saved to root file, return array index
    int Add(const CombinedData &a); // Need to test function of this function. I doubt it can delete objects properly.

    void ClearLoop();
    bool Fill(int event_index); // event_index is the oldest event index, Fill Data into file

    int GetEventNum() const;
    bool Combine(PrimaryDataPtr &b, int index, const DetectorConfig *detector = gConfigure);

private:
    CombinedData &GetLastEvent(int a);
    FileManager *fFileManager;
};

// class Detector is in charge of construct, destruct, search, combine, add... process of detector loop.
// When create a new Detector, first create a boards_manager, and use this boards manager to create combine data and combine data loop.
// When there comes a new primary data, first will search within combine data loop, (but this time, just search in time loop instead, because primary data is different with combined data)
// If no combined data (time data) is found, data and combined data with no address infomation will be passed to board manager to judge whether should create a new combined data.
// If find some boards with the same TDC value, boards manager will allocate a new address to save passed combined data.
// The new combined data will than be added to combined loop. However, before adding procedure, loop will judge whether loop is full
// If loop is full, the last element will be saved to root file through file manager. Else loop will just add a new combined data.
// If find no boards with the same TDC value, new primary data will be just added to corresponding board straightly.

class DetectorDataBuffer
{
public:
    DetectorDataBuffer(FileManager *fileManager);

    int Search(int Time);          // Search through time loop, return event index;
    int Search(PrimaryDataPtr &a); // Search through time loop
    bool TryCombine(PrimaryDataPtr &a);

    int Add(CombinedData &a); // Add combined_data to loop combine data loop and data time to time loop;

private:
    DetectorLoop combine_data_loop;
    Loop<UInt_t> time_loop;

    FileManager *fFileManager;
};

class OneGroup // Construct boards manager, file manager, and detector info.
{
public:
    OneGroup(DetectorConfig *detector);
    OneGroup(OneGroup &) = delete;
    OneGroup &operator=(OneGroup &) = delete;
    ~OneGroup(); // Save all buffer data which is still saved inside the loop.

    bool AddData(PrimaryDataPtr &a);
    void PrintDetectorInfo() { fDetectorInfo->Print(); }

    static OneGroup *&CurrentGroupManager();

private:
    int fDetectorNumber;
    TDetectorInfo *fDetectorInfo;
    DetectorDataBuffer *fBuffer;
    FileManager *fFileManager;

    CombinedData fCombineDataTemp;

    bool fWrite;
};

#define gGroupManager (OneGroup::CurrentGroupManager())

#endif