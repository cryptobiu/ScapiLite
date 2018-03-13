//
// Created by liork on 11/03/18.
//

#include "ConfigFile.h"

string trim(string const& source, char const* delims = " \t\r\n") {
    string result(source);
    string::size_type index = result.find_last_not_of(delims);
    if (index != string::npos)
        result.erase(++index);

    index = result.find_first_not_of(delims);
    if (index != string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}

ConfigFile::ConfigFile(string const &configFile, JNIEnv *env, AAssetManager *assetManager) {

    AAsset* file = AAssetManager_open(assetManager, configFile.c_str(), AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);
    char* fileContent = new char[fileLength+1];

    // Read your file
    AAsset_read(file, fileContent, fileLength);
    // For safety you can add a 0 terminating character at the end of your file ...
    fileContent[fileLength] = '\0';
    string dataBeforeParsing = fileContent;
    stringstream data(fileContent);

    string line;
    string name;
    string value;
    string inSection;
    int posEqual;
    while (std::getline(data, line))
    {
        if (!line.length()) continue;

        if (line[0] == '#') continue;
        if (line[0] == ';') continue;

        if (line[0] == '[') {
            inSection = trim(line.substr(1, line.find(']') - 1));
            continue;
        }

        posEqual = line.find('=');
        name = trim(line.substr(0, posEqual));
        value = trim(line.substr(posEqual + 1));

        content_[inSection + '/' + name] = value;
    }
}

string const& ConfigFile::Value(string const& section, string const& entry) const {
    map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

    if (ci == content_.end()) throw "does not exist";

    return ci->second;
}

