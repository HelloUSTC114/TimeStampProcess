#include "Logic.h"

bool CRCaliLogicJudge::JudgeLogic(const vector<bool> &flagArray) const
{
    int size = flagArray.size();
    int RequiredMinBoards = gConfigure->GetTotalBoardNumber();
    if (size < RequiredMinBoards)
    {
        return false;
    }

    for (int i = 0; i < RequiredMinBoards; i++)
    {
        if (!flagArray[i])
        {
            return false;
        }
    }
    return true;
}

bool CRImageLogicJudge::JudgeLogic(const vector<bool> &flagArray) const 
{
    int size = flagArray.size();
    int RequiredMinBoards = gConfigure->GetTotalBoardNumber();

    if (size < RequiredMinBoards)
    {
        return false;
    }

    for (int i = 0; i < gConfigure->GetTotalDetectorNumber(); i++)
    {
        // i th detector X layer
        bool flagX = 0;
        int boardNumberX = gConfigure->GetBoardNumberX(i);
        for (int x = 0; x < boardNumberX; x++)
        {
            int boardIndexTemp = gConfigure->GetIndex(i, 0, x);
            if (flagArray[boardIndexTemp])
            {
                flagX = 1;
                break;
            }
        }
        if (!flagX)
            return false;

        // i th detecotr Y layer
        bool flagY = 0;
        int boardNumberY = gConfigure->GetBoardNumberY(i);
        for (int y = 0; y < boardNumberY; y++)
        {
            int boardIndexTemp = gConfigure->GetIndex(i, 1, y);
            if (flagArray[boardIndexTemp])
            {
                flagY = 1;
                break;
            }
        }
        if (!flagY)
            return false;
    }
    return true;
}

// bool LogicJudge::JudgeLogic(const CombinedData &CData)
// {
//     bool Logic = JudgeLogic(CData.GetFlagArray());
//     if (!Logic)
//     {
//         return false;
//     }
//     return CheckValid::CheckValidData(CData, 0);
// }

bool CheckValid::CheckValidData(const CombinedData &CData, bool ShowFlag)
{
    int layerCount = CData.Get_Layer_Number();
    auto DataFlag = new bool[layerCount]{0};
    auto DataIndex = new int[layerCount]{0};
    bool ValidFlag = 1;

    auto InvalidDB = GetInvalidDataBase();

    // See if data is invalid
    for (int layer = 0; layer < layerCount; layer++)
    {
        if (CData.GetData(layer).get() == NULL)
        {
            DataIndex[layer] = -1;
            continue;
        }
        auto array = CData.GetData(layer)->GetDataArray();
        vector<int> temp(32);
        for (int i = 0; i < 32; i++)
        {
            temp[i] = array[i];
        }
        for (int DBIndex = 0; DBIndex < InvalidDB.size(); DBIndex++)
        {
            if (temp == InvalidDB[DBIndex])
            {
                ValidFlag = false;
                DataFlag[layer] = false;
                DataIndex[layer] = DBIndex;
            }
            else
            {
                DataFlag[layer] = true;
                DataIndex[layer] = -1;
            }
        }
    }

    if (ShowFlag)
    {
        for (int i = 0; i < layerCount; i++)
        {
            cout << "Layer: " << i << '\t' << "Validation: " << DataFlag[i] << '\t' << "Index in DB: " << DataIndex[i] << endl;
        }
    }
    delete[] DataFlag;
    delete[] DataIndex;

    return ValidFlag;
}

vector<vector<int>> &CheckValid::GetInvalidDataBase()
{
    vector<int> temp1{
        0, 0, 1004, 0,
        0, 40002, 32768, 29,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1876, 16993, 29547,
        25972, 867, 26727, 1133,
        28784, 25344, 769, 34464,
        0, 0, 0, 0};
    static vector<vector<int>> fInvalidDataBase{temp1};
    return fInvalidDataBase;
}

bool CheckLogicAndValid(const CombinedData& CData, const LogicJudge & logic)
{
    bool logicResult = logic.JudgeLogic(CData);
    if(!logicResult)
    {
        return false;
    }
    bool checkResult = CheckValid::CheckValidData(CData, 0);
    return checkResult;

}