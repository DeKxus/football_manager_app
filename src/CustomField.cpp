#include "CustomField.h"
#include "Team.h"

CustomField::CustomField(float x, float y, ofImage fieldImage)
    : x(x), y(y), fieldImage(fieldImage), lineWidth(5.0f),
    pencilButton(x + fieldImage.getWidth() + 220, y + fieldImage.getHeight() - 110 , 100, 100, ofImage("icons/Edit.png")),
    clearButton(x + fieldImage.getWidth() + 110, y + fieldImage.getHeight() - 110, 100, 100, ofImage("icons/Delete.png")),
    isDrawing(false),
    brushColor(ofColor::fromHex(0xFF5154)) {
}

void CustomField::setup() {
    drawingFbo.allocate(fieldImage.getWidth(), fieldImage.getHeight(), GL_RGBA);
    drawingFbo.begin();
    ofClear(255, 255, 255, 0);
    drawingFbo.end();

    setupButtons();
}

void CustomField::draw() {

    //Drawing feature
    fieldImage.draw(x, y);
    drawingFbo.draw(x, y);

    pencilButton.draw();
    clearButton.draw();

    //Player feature
    for (size_t i = 0; i < placedPlayers.size(); ++i) {
        const Player& player = placedPlayers[i];
        const auto& pos = positions[i];
        float posX = std::get<0>(pos);
        float posY = std::get<1>(pos);

        // Draw circle at player's position
        ofSetColor(ofColor(0x9B9B9B)); // Red color for the circle
        ofDrawCircle(posX, posY, 20); // Circle with radius 20

        // Draw player's number inside the circle
        ofSetColor(255, 255, 255); // Black color for the text
        ofDrawBitmapString(std::to_string(player.number), posX - 5, posY + 5);
    }
    
}

void CustomField::mouseDragged(int x, int y) {
    if (isDrawing) {

        // Add the new point to the polyline
        polyline.addVertex(x - this->x, y - this->y);

        drawingFbo.begin();
        ofSetColor(brushColor);
        ofSetLineWidth(lineWidth);
        polyline.draw();
        drawingFbo.end();
    }
}

void CustomField::mousePressed(int x, int y, Player player) {
    if (pencilButton.isMouseOver(x, y)) {
        pencilButton.mousePressed(x, y);
    }
    else if (clearButton.isMouseOver(x, y)) {
        clearButton.mousePressed(x, y);
    }
    else if (x >= this->x && x <= this->x + fieldImage.getWidth() && y >= this->y && y <= this->y + fieldImage.getHeight() && findPlayerIndexByNumber(player.number) == -1) {
        // Player does not exist, add new player and position
        if (player.number != 0) {
            placedPlayers.push_back(player);
            positions.push_back(std::make_tuple(x, y));
        }  
    }
    else if (x >= this->x && x <= this->x + fieldImage.getWidth() && y >= this->y && y <= this->y + fieldImage.getHeight()) {
        isDrawing = true;
    }
}

void CustomField::mouseReleased(int x, int y) {
    isDrawing = false;
    polyline.clear();
}

void CustomField::setupButtons() {
    pencilButton.setOnClickListener([this]() { onPencilButtonPressed(); });
    clearButton.setOnClickListener([this]() { onClearButtonPressed(); });
}

void CustomField::onPencilButtonPressed() {
    brushColor = ofColor::fromHex(0xFF5154);  // Set the brush color
}

void CustomField::onClearButtonPressed() {
    drawingFbo.begin();
    ofClear(255, 255, 255, 0);
    drawingFbo.end();

    //Clear arrays
    placedPlayers.clear();
    positions.clear();
}

// Function to add or update a player and their position
void CustomField::addOrUpdatePlayer(const Player& player, const std::tuple<int, int>& position) {
    int index = findPlayerIndexByNumber(player.number);

    if (index != -1) {
        // Player exists, update their position
        //placedPlayers[index] = player;
        //positions[index] = position;
    }
    else {
        // Player does not exist, add new player and position
        placedPlayers.push_back(player);
        positions.push_back(position);
    }
}

// Function to find the index of a player by number
int CustomField::findPlayerIndexByNumber(int number) {
    for (size_t i = 0; i < placedPlayers.size(); ++i) {
        if (placedPlayers[i].number == number) {
            return i;
        }
    }
    return -1; // Not found
}

