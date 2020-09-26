#include "General.h"

TChain *JohnGeneral::GenerateChain(std::string treeName, const std::vector<std::string> &keyWord, TChain *chain)
{
    gSystem->Exec("ls  1> .~filelist 2> /dev/null");

    auto ch = chain;
    if (ch == NULL)
    {
        ch = new TChain(treeName.c_str());
    }

    ifstream file_list(".~filelist");
    for (int i = 0; file_list.is_open() && file_list.eof() == false; i++)
    {
        string s_temp;
        file_list >> s_temp;

        bool continueFlag = 0;
        for (size_t keyCount = 0; keyCount < keyWord.size(); keyCount++)
        {
            if (s_temp.find(keyWord[keyCount]) == string::npos)
            {
                continueFlag = 1;
            }
        }
        if (continueFlag)
            continue;

        cout << "File: " << s_temp << " Read" << endl;
        ch->Add(s_temp.c_str());
    }
    cout << "Totally get " << ch->GetEntries() << " Entries" << endl;
    gSystem->Exec("rm .~filelist");
    return ch;
}