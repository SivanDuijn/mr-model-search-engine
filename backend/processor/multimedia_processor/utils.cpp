#include "headers.h"

namespace utils
{
     void GetAllFilenamesInDatabase(const std::string database, std::vector<std::string>& filenames)
    {
        // Read in filenames
        ifstream ifs(vars::GetAssetPath(database + "/files.json"));
        nlohmann::json files = nlohmann::json::parse(ifs);
        for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
            for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) 
                filenames.push_back(*it2);
    }
}