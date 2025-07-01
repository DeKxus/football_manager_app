#pragma once
#include "ofMain.h"

struct Player {
    int number;
    std::string name;
    ofImage image;
};

struct Team {
    std::string name;
    ofImage logo;
    std::vector<Player> players;
    std::vector<std::string> videos;
};