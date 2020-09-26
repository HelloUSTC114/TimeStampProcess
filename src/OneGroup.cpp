#define DETECTOR_MANAGER_CXX

#include "OneGroup.h"

DetectorLoop::DetectorLoop(int Loop_Size, FileManager *filemanager) : Loop<CombinedData>(Loop_Size), fFileManager(filemanager)
{
}

DetectorLoop::~DetectorLoop() // Why this is executed twice?
{
    // cout << "Fill" << endl;
    // cout << "Event Num: " << event_num << endl;
    for (int i = 0; i < event_num; i++)
    {
        DetectorLoop::Fill(i);
    }
}

int DetectorLoop::Add(const CombinedData &a)
{
    if (!full_flag)
    {
        return Loop<CombinedData>::Add(a);
    }
    Fill(0);
    int temp = Loop<CombinedData>::Add(a);
    return temp;
}

void DetectorLoop::ClearLoop()
{
    cout << "Clear Loop" << endl;
    for (int i = 0; i < event_num; i++)
    {
        DetectorLoop::Fill(i);
    }
    event_num = 0;
    full_flag = 0;
    for (int i = 0; i < loop_size; i++)
    {
        flag_ptr = 0;
    }
    fLoopIndex = 0;
}

bool DetectorLoop::Fill(int event_index)
{
    CombinedData temp = Get_First_Event(event_index);
    TCombinedData *comb_temp = new TCombinedData(temp);

    bool flag = fFileManager->Fill(comb_temp);

    delete comb_temp;
    return flag;
}

int DetectorLoop::GetEventNum() const
{
    return event_num;
}

CombinedData &DetectorLoop::GetLastEvent(int a)
{
    return loop_ptr[Get_Last_Event_Index(a)];
}

bool DetectorLoop::Combine(PrimaryDataPtr &b, int index, const DetectorConfig *detector)
{
    return GetLastEvent(index).Combine(b, detector);
}

DetectorDataBuffer::DetectorDataBuffer(FileManager *fileManager) : combine_data_loop(Loop_Size, fileManager), time_loop(Loop_Size), fFileManager(fileManager)
{
}

int DetectorDataBuffer::Search(int Time)
{
    for (int i = 0; i < combine_data_loop.GetEventNum(); i++)
    {
        const CombinedData temp = combine_data_loop.Get_Last_Event(i);
        int time_temp = temp.Get_Time();
        if (TMath::Abs(time_temp - Time) < 1000)
        {
            return i;
        }
    }
    return -1;
}

int DetectorDataBuffer::Search(PrimaryDataPtr &a)
{
    return Search(a.Get_TDC_Value());
}

bool DetectorDataBuffer::TryCombine(PrimaryDataPtr &a)
{
    int event_index = Search(a);
    if (event_index == -1)
        return false;

    return combine_data_loop.Combine(a, event_index);
}

int DetectorDataBuffer::Add(CombinedData &a)
{
    UInt_t time_temp = a.Get_Time();
    int temp1 = time_loop.Add(time_temp);
    int temp2 = combine_data_loop.Add(a);
    return temp1;
}

OneGroup::OneGroup(DetectorConfig *detector) : fDetectorNumber(0)
{
    fDetectorInfo = new TDetectorInfo(fDetectorNumber);
    fFileManager = new FileManager();
    fFileManager->Initiate();
    // fFileManager -> Print();
    fBuffer = new DetectorDataBuffer(fFileManager);

    fFileManager->Write(fDetectorInfo);
    fWrite = 1;

    if (detector)
    {
        fCombineDataTemp = CombinedData(*detector);
    }
    else
    {
        fCombineDataTemp = CombinedData(*gConfigure);
    }
}

OneGroup::~OneGroup()
{
    delete fBuffer;
    fBuffer = NULL;

    fFileManager->Write();

    delete fFileManager;
    delete fDetectorInfo;
    fFileManager = NULL;
    fDetectorInfo = NULL;
}

bool OneGroup::AddData(PrimaryDataPtr &a)
{
    bool CombineFlag(0);
    CombineFlag = fBuffer->TryCombine(a);
    if (CombineFlag)
    {
        return true;
    }
    bool BoardProcessCPFlag(1);
    if (BoardProcessCPFlag)
    {
        /* ************************
        // index should be determined here
        // int indexTemp;
        // fCombineDataTemp -> Combine(indexTemp, a);
        *******************************/
        fCombineDataTemp->Combine(a, gConfigure);

        fBuffer->Add(fCombineDataTemp);
        fCombineDataTemp = CombinedData(*gConfigure);
        // cout << "Extract from loop successful." << endl;
        return true;
    }

    return false;
}

OneGroup *&OneGroup::CurrentGroupManager()
{
    static OneGroup *currentDetectorManager = new OneGroup(0);
    return currentDetectorManager;
}