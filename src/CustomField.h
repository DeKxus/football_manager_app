#pragma once

#include "ofMain.h"
#include "CustomButton.h"
#include "Team.h"

class CustomField {
public:
    CustomField(float x, float y, ofImage fieldImage);
    void setup();
    void draw();
    void mouseDragged(int x, int y);
    void mousePressed(int x, int y, Player player);
    void mouseReleased(int x, int y);
    void addOrUpdatePlayer(const Player& player, const std::tuple<int, int>& position);
    int findPlayerIndexByNumber(int number);
    

private:
    float x, y;
    ofImage fieldImage;
    ofFbo drawingFbo;
    ofPolyline polyline;
    float lineWidth;
    ofColor brushColor;
    bool isDrawing;

    std::vector<Player> placedPlayers;
    std::vector<std::tuple<int, int>> positions;

    CustomButton pencilButton;
    CustomButton clearButton;

    void setupButtons();
    void onPencilButtonPressed();
    void onClearButtonPressed();
};