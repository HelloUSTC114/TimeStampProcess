#ifndef LOGIC_H
#define LOGIC_H
#include "Configure.h"
#include "CombineData.h"
#include <vector>

class CombinedData;

class LogicJudge
{
public:
    // LogicJudge();
    // ~LogicJudge();
    virtual bool JudgeLogic(const CombinedData &CData) const = 0;
    virtual bool JudgeLogic(const vector<bool> &) const = 0;
};

// Class that stashes all invalid data from each detector
class CheckValid
{
public:
    static bool CheckValidData(const CombinedData &CData, bool ShowFlag);

private:
    static vector<vector<int>> &GetInvalidDataBase();
};

class CRCaliLogicJudge : public LogicJudge // Cosmic Ray calibration logic
{
public:
    bool JudgeLogic(const vector<bool> &) const override;
    bool JudgeLogic(const CombinedData &CData) const override { return JudgeLogic(CData.GetFlagArray()); }
};

class CRImageLogicJudge : public LogicJudge // CR imaging logic
{
public:
    bool JudgeLogic(const vector<bool> &) const override;
    bool JudgeLogic(const CombinedData &CData) const override { return JudgeLogic(CData.GetFlagArray()); }
};

bool CheckLogicAndValid(const CombinedData &CData, const LogicJudge &logic);

#endif