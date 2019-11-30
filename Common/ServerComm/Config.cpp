
#include <string>
#include <fstream>
#include <streambuf>

#include "Config.h"

std::unique_ptr<Config> g_pConfig = nullptr;

bool Config::Load(const char *jsonFile)
{
    Json::Reader fileReader;
    std::ifstream ifs(jsonFile);
    if(ifs.good())
    {
        std::string str((std::istreambuf_iterator<char>(ifs)),std::istreambuf_iterator<char>());
        fileReader.parse(str, m_Root, true);
        return true;
    }
    return false;
}
