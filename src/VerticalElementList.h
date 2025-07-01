#pragma once
#include "ofMain.h"
#include "Team.h"

class VerticalElementList {
public:
    VerticalElementList(float x, float y, float elementWidth, float elementHeigth, std::vector<Player> players, ofColor color = ofColor(0x9B9B9B));
    void draw();
    void drawListElement(float x, float y, float width, float height, ofColor color, float transparency, float scale, Player player);
    void mouseDragged(float mouseX, float mouseY);
    void mousePressed(float mouseX, float mouseY);
    void mouseReleased(float mouseX, float mouseY);
    void setOnClickListener(std::function<void()> listener);
    Player getSelectedPlayer();

private:
    float x, y, elementWidth, elementHeight;
    ofColor color;
    std::vector<Player> players;
    Player selectedPlayer;
    float scrollOffset;
    float dragStartX;
    float dragStartY;
    float dragThreshold;
    bool isDragging;
    std::function<void()> onClickListener;

    void clampScroll();
};

