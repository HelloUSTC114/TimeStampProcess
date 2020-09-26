#include "SortWeight.h"

vector<ChWeight> &SortByDescend(vector<ChWeight> &ChArray)
{
    sort(ChArray.begin(), ChArray.end(), WeightCompareDescend());

    return ChArray;
}

vector<ChWeight> &SortByAscend(vector<ChWeight> &ChArray)
{
    sort(ChArray.begin(), ChArray.end(), WeightCompareAscend());

    return ChArray;
}

vector<int> SortByAscend(const vector<double> &ChArray)
{
    vector<ChWeight> weightTemp;
    for(int i = 0; i < ChArray.size(); i++)
    {
        weightTemp.push_back(ChWeight{i,ChArray[i]});
    }
    SortByAscend(weightTemp);
    vector<int> chSequence;
    for(int i = 0; i < ChArray.size(); i++)
    {
        chSequence.push_back(weightTemp[i].first);
    }
    return chSequence;
}

vector<int> SortByDescend(const vector<double> &ChArray)
{
    vector<ChWeight> weightTemp;
    for (int i = 0; i < ChArray.size(); i++)
    {
        weightTemp.push_back(ChWeight{i, ChArray[i]});
    }
    SortByDescend(weightTemp);
    vector<int> chSequence;
    for (int i = 0; i < ChArray.size(); i++)
    {
        chSequence.push_back(weightTemp[i].first);
    }
    return chSequence;
}
