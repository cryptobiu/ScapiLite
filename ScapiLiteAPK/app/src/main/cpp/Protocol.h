//
// Created by liork on 11/03/18.
//

#ifndef SCAPILITEAPK_PROTOCOL_H
#define SCAPILITEAPK_PROTOCOL_H


#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

class CmdParser {
public:
    string getKey(string parameter);
    vector<pair<string, string>> parseArguments(string protocolName, int argc, char* argv[]);
    string getValueByKey(vector<pair<string, string>>arguments, string key);
};

/**
 * This class is an abstract class for all kinds of protocols.
 * Since the protocols are different from each other, the only function that common is the run function that executes
 * the protocol.
 *
 * In order to run a protocol one should follow the next steps:
 * 1. Create the protocol. Give all the protocol's parameters to the constructor.
 * 2. In case the protocols needs input, call setInput function.
 * 3. Call run function.
 *
 * The setInput function is not part of this abstract class since:
 * 1. Not all the protocols has input.
 * 2. Every protocol ges different input.
*/

class Protocol
{
private:
    CmdParser parser;
protected:
    vector<pair<string, string>> arguments;

public:
    Protocol(string protocolName, int argc, char* argv[]);

    /**
     * Executes the protocol.
     */
    virtual string run() = 0;
    virtual bool hasOffline() = 0;
    virtual void runOffline(){};
    virtual bool hasOnline() = 0;
    virtual string runOnline(){};
    vector<pair<string, string>> getArguments();
    CmdParser getParser();

    virtual ~Protocol() {}
};


class PartiesNumber {};

class TwoParty : public PartiesNumber {};
class ThreeParty : public PartiesNumber {};
class MultiParty : public PartiesNumber {};


#endif //SCAPILITEAPK_PROTOCOL_H
