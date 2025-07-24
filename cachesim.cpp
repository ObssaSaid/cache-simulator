// cachesim.cpp : This file contains the 'main' function. Program execution begins and ends there.
// -------OBSSA SAID--------

#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <vector>
#include <math.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

int main() {
    int blocksize = 32;
    int cache = 1024; //2^(9 - 13)
    cout << "Name: Obssa Said" << endl;
    cout << "Starting Cache Simulation" << endl;
    cout << "Cache Size = " << cache << endl;
    cout << "Bytes Per Block = " << blocksize << endl;
    cout << endl;

    vector<string> storeAddress;
    ifstream trace("swim.trace");
    string memAccess;

    while (getline(trace, memAccess)) {
        istringstream input(memAccess);
        string memoryAddress;
        input >> memoryAddress;
        if (input >> memoryAddress) {
            storeAddress.push_back(memoryAddress);
        }
    }

    for (string& address : storeAddress) {
        string binary;
        for (int i = 2; i < address.length(); i++) {
            if (address.at(i) == '0') {
                binary += "0000";
            }
            else if (address.at(i) == '1') {
                binary += "0001";
            }
            else if (address.at(i) == '2') {
                binary += "0010";
            }
            else if (address.at(i) == '3') {
                binary += "0011";
            }
            else if (address.at(i) == '4') {
                binary += "0100";
            }
            else if (address.at(i) == '5') {
                binary += "0101";
            }
            else if (address.at(i) == '6') {
                binary += "0110";
            }
            else if (address.at(i) == '7') {
                binary += "0111";
            }
            else if (address.at(i) == '8') {
                binary += "1000";
            }
            else if (address.at(i) == '9') {
                binary += "1001";
            }
            else if (address.at(i) == 'a') {
                binary += "1010";
            }
            else if (address.at(i) == 'b') {
                binary += "1011";
            }
            else if (address.at(i) == 'c') {
                binary += "1100";
            }
            else if (address.at(i) == 'd') {
                binary += "1101";
            }
            else if (address.at(i) == 'e') {
                binary += "1110";
            }
            else if (address.at(i) == 'f') {
                binary += "1111";
            }
        }

        while (binary.length() < 32) {
            binary = "0" + binary;
        }
        address = binary;
    }

    int numberOfLines = cache / blocksize;


    // Directly Mapped Simulation
    map<string, string> tagMap;
    int DMhits = 0;
    int DMmisses = 0;
    for (int i = 0; i < storeAddress.size(); i++) {
        int lineSize = log2(numberOfLines);
        int offsetSize = log2(blocksize);
        int tagSize = storeAddress.at(i).length() - lineSize - offsetSize;

        string tag(tagSize, '\0');
        copy_n(storeAddress.at(i).begin(), tagSize, tag.begin());

        string addr(lineSize, '\0');
        copy_n(storeAddress.at(i).begin() + tagSize, lineSize, addr.begin());

        if (tagMap.find(addr) == tagMap.end()) {
            DMmisses++;
            tagMap.insert(make_pair(addr, tag));
        }
        else {
            if (tagMap.at(addr) == tag) {
                DMhits++;
            }
            else {
                DMmisses++;
                tagMap.at(addr) = tag;
            }
        }
    }
    int accessCount = DMhits + DMmisses;
    double DMhitrate = (double)DMhits / (double)accessCount;
    cout << "Direct Mapped Hit Rate: " << DMhitrate << endl;
    

    // Fully Associative Simulation
    int hitsFIFO = 0;
    int missesFIFO = 0;
    int hitsLRU = 0;
    int missesLRU = 0;
    list<string> l;
    queue<string> q;
    map<string, string> tagMapFIFO;
    map<string, string> tagMapLRU;
    for (int i = 0; i < storeAddress.size(); i++) {
        int FAoffsetsize = log2(blocksize);
        int FAtagsize = storeAddress.at(i).length() - FAoffsetsize;

        string tag(FAtagsize, '\0');
        copy_n(storeAddress.at(i).begin(), FAtagsize, tag.begin());

        string offset(FAoffsetsize, '\0');
        copy_n(storeAddress.at(i).begin() + FAtagsize, FAoffsetsize, offset.begin());

        // FIFO policy
        if (tagMapFIFO.find(tag) == tagMapFIFO.end()) {
            missesFIFO++;
            if (tagMapFIFO.size() < numberOfLines) {
                tagMapFIFO.insert(make_pair(tag, offset));
                q.push(tag);
            }
            else if (tagMapFIFO.size() == numberOfLines) {
                tagMapFIFO.erase(q.front());
                q.pop();
                q.push(tag);
                tagMapFIFO.insert(make_pair(tag, offset));
            }
        }
        else {
            hitsFIFO++;
        }

        // LRU policy
        if (tagMapLRU.find(tag) == tagMapLRU.end()) {
            missesLRU++;
            if (tagMapLRU.size() < numberOfLines) {
                tagMapLRU.insert(make_pair(tag, offset));
                l.push_back(tag);
            }
            else if (tagMapLRU.size() == numberOfLines) {
                tagMapLRU.erase(l.front());
                l.pop_front();
                l.push_back(tag);
                tagMapLRU.insert(make_pair(tag, offset));
            }
        }
        else {
            hitsLRU++;
            l.remove(tag);
            l.push_back(tag);
        }
    }
    int accessCountFIFO = hitsFIFO + missesFIFO;
    int accessCountLRU = hitsLRU + missesLRU;
    cout << "FIFO Fully Associative Hit Rate: " << (double)hitsFIFO / (double)accessCountFIFO << endl;
    cout << "LRU Fully Associative Hit Rate: " << (double)hitsLRU / (double)accessCountLRU << endl;
    

    // Set Associative Simulation
    // FIFO = 1
    // LRU = 2
for (int n : {2, 4, 8}) {
    for (int replace : {1, 2}) {
        map<string, map<string, string>> tagMap;
        map<string, queue<string>> tagQ;
        map<string, list<string>> tagL;
        int sethits = 0;
        int setmisses = 0;
        for (int i = 0; i < storeAddress.size(); i++) {
            int numberOfSets = (cache / blocksize) / n;
            int setsize = log2(numberOfSets);
            int SAoffsetsize = log2(blocksize);
            int SAtagsize = storeAddress.at(i).length() - setsize - SAoffsetsize;

            string tag(SAtagsize, '\0');
            copy_n(storeAddress.at(i).begin(), SAtagsize, tag.begin());

            string set(setsize, '\0');
            copy_n(storeAddress.at(i).begin() + SAtagsize, setsize, set.begin());

            string offset(SAoffsetsize, '\0');
            copy_n(storeAddress.at(i).begin() + SAtagsize + setsize, SAoffsetsize, offset.begin());


            if (tagMap.find(set) == tagMap.end()) {
                setmisses++;
                map<string, string> temp;
                temp.insert(make_pair(tag, offset));
                tagMap.insert(make_pair(set, temp));

                if (replace == 1) {
                    queue<string> q;
                    q.push(tag);
                    tagQ.insert(make_pair(set, q));
                }

                else if (replace == 2) {
                    list<string> l;
                    l.push_back(tag);
                    tagL.insert(make_pair(set, l));
                }
            }

            else {

                if (tagMap.at(set).find(tag) == tagMap.at(set).end()) {
                    setmisses++;

                    if (tagMap.at(set).size() < n) {
                        tagMap.at(set).insert(make_pair(tag, offset));

                        if (replace == 1) {
                            tagQ.at(set).push(tag);
                        }

                        else if (replace == 2) {
                            tagL.at(set).push_back(tag);
                        }
                    }

                    else if (tagMap.at(set).size() == n) {
                        if (replace == 1) {
                            tagMap.at(set).erase(tagQ.at(set).front());
                            tagQ.at(set).pop();
                            tagQ.at(set).push(tag);
                        }

                        else if (replace == 2) {
                            tagMap.at(set).erase(tagL.at(set).front());
                            tagL.at(set).pop_front();
                            tagL.at(set).push_back(tag);
                        }
                        tagMap.at(set).insert(make_pair(tag, offset));
                    }
                }

                else {
                    sethits++;

                    if (replace == 2) {
                        tagL.at(set).remove(tag);
                        tagL.at(set).push_back(tag);
                    }
                }
            }
        }
        string policy;
        if (replace == 1) {
            policy = "FIFO";
        }
        else if (replace == 2) {
            policy = "LRU";
        }
        double SAaccessCount = (double)sethits / (double)(sethits + setmisses);
        //cout << "Set Associative:" << endl;
        cout << "Hit Rate (" << policy << ", " << n << "-way): " << SAaccessCount << endl;
    }
}


    
}

