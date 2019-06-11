#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
bool comparerTwoString(string string1, string string2);
string openCacheFile(string request);
void createNewCacheFile(string data, string newRequest);
string cache(string request);