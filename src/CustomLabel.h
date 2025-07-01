#pragma once
#include "ofMain.h"

class CustomLabel {
public:
    CustomLabel(float x, float y, float width, float height, ofImage icon, std::string text, ofColor labelColor = ofColor(0x9B9B9B));
    void draw();

private:
    float x, y, width, height;
    ofImage icon;
    std::string text;
    ofColor color;
};