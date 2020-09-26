#ifndef SORTWEIGHT_H
#define SORTWEIGHT_H

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

typedef pair<int, double> ChWeight;

// Descend weight
struct WeightCompareDescend
{
    bool operator()(const ChWeight &c1, const ChWeight &c2)
    {
        return c1.second>c2.second;
    }
};

vector<ChWeight> & SortByDescend(vector<ChWeight> & ChArray);
vector<int> SortByDescend(const vector<double> &ChArray);



// Ascend Weight
struct WeightCompareAscend
{
    bool operator()(const ChWeight &c1, const ChWeight &c2)
    {
        return c1.second < c2.second;
    }
};

vector<ChWeight> &SortByAscend(vector<ChWeight> &ChArray);

vector<int>  SortByAscend(const vector<double> & ChArray);


template <typename T>
vector<int> sortByAscend(vector<T> &ChArray)
{
    vector<ChWeight> weightTemp;
    for (int i = 0; i < ChArray.size(); i++)
    {
        weightTemp.push_back(ChWeight{i, ChArray[i]});
    }
    SortByAscend(weightTemp);
    vector<int> chSequence;
    for (int i = 0; i < ChArray.size(); i++)
    {
        chSequence.push_back(weightTemp[i].first);
    }
    return chSequence;
}
template <typename T>
vector<int> sortByDescend(vector<T> &ChArray)
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

#endif