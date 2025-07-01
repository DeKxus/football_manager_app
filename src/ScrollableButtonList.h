#pragma once
#include "ofMain.h"
#include "Team.h"

class ScrollableButtonList {
public:
    ScrollableButtonList(float x, float y, float buttonWidth, float buttonHeight, std::vector<Team> teams, ofColor color = ofColor(0x9B9B9B));
    void draw();
    void drawListElement(float x, float y, float width, float height, ofColor color, float transparency, float scale, ofImage icon);
    void drawEmptyListElement(float x, float y, float width, float height, ofColor color);
    void mouseDragged(float mouseX, float mouseY);
    void mousePressed(float mouseX, float mouseY);
    void mouseReleased(float mouseX, float mouseY);
    void setOnClickListener(std::function<void()> listener);
    Team getSelectedTeam();

private:
    float x, y, buttonWidth, buttonHeight;
    ofColor color;
    std::vector<Team> teams;
    Team selectedTeam;
    float scrollOffset;
    float dragStartX;
    float dragStartY;
    float dragThreshold;
    bool isDragging;
    std::function<void()> onClickListener;

    void clampScroll();
};
