#include "SpecializedInputOutput.h"

string sio::getFilePathWithoutSuffix(const string& filepath)
{
    size_t found = filepath.find_last_of('.');
    if (found == string::npos) {
        return filepath;
    }
    else {
        return filepath.substr(0, found);
    }
}
