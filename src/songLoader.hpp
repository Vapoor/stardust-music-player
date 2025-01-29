#ifndef SONGLOADERHPP
#define SONGLOADERHPP

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

using namespace std;

class songLoader{
    private:
        vector<string> songs;
    public:
        songLoader(string filepath);
        ~songLoader();
};


#endif