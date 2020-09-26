#ifndef GENERAL_H
#define GENERAL_H
#include "TChain.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include "TSystem.h"

using namespace std;

namespace JohnGeneral
{
    TChain *GenerateChain(std::string treeName, const std::vector<std::string> &keyWord, TChain *chain = NULL);

}

#endif