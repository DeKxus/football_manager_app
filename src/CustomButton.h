#pragma once
#include "ofMain.h"

class CustomButton {
public:
    CustomButton(float x, float y, float width, float height, ofImage icon, ofColor buttonColor = ofColor(0x9B9B9B));
    void draw();
    void drawTransparentAndScale(float transparency, float scale);
    bool isMouseOver(float mouseX, float mouseY);
    void mousePressed(float mouseX, float mouseY);
    void setOnClickListener(std::function<void()> listener);
    float getX();
    float getY();

private:
    float x, y, width, height;
    ofImage icon;
    ofColor color;
    std::function<void()> onClickListener;
};
