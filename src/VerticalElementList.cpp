#include "VerticalElementList.h"

VerticalElementList::VerticalElementList(float x, float y, float elementWidth, float elementHeight, std::vector<Player> players, ofColor color)
    : x(x), y(y), elementWidth(elementWidth), elementHeight(elementHeight), color(color), players(players), dragThreshold(1.0f), selectedPlayer(), scrollOffset(0), isDragging(false) {}

void VerticalElementList::draw() {
    ofPushMatrix();
    ofTranslate(x, y - scrollOffset);

    float spacing = 10.0f;
    float totalHeight = 0;

    // Variables to keep track of the player with the highest scale
    float maxScale = 0.0f;
    Player* maxScalePlayer = nullptr;

    for (auto& player : players) {
        // Calculate the distance from the button center to the top where it should start fading
        float buttonCenterY = y - scrollOffset + totalHeight + elementHeight / 2.0f;
        float screenHeight = ofGetHeight();
        float fadeHeight = (screenHeight - elementHeight / 2.0f) / 2.0f; // Height of the area where buttons will start to fade
        float distanceToFadeStart = std::min(buttonCenterY, screenHeight - buttonCenterY);

        // Calculate the transparency
        float transparency = ofMap(distanceToFadeStart, 0, fadeHeight, 0, 1, true);
        // Calculate the scale
        float scale = ofMap(distanceToFadeStart, 0, fadeHeight, 0.7, 1, true);

        // Track the player with the highest scale
        if (scale > maxScale) {
            maxScale = scale;
            maxScalePlayer = &player;
        }

        drawListElement(0, 0, elementWidth, elementHeight, color, transparency, scale, player);

        ofTranslate(0, elementHeight + (spacing * scale));
        totalHeight += elementHeight + (spacing * scale);
    }

    // Save the player with the highest scale in the selectedPlayer variable
    if (maxScalePlayer) {
        selectedPlayer = *maxScalePlayer;
    }

    ofPopMatrix();
}

void VerticalElementList::drawListElement(float x, float y, float width, float height, ofColor color, float transparency, float scale, Player player) {
    ofSetColor(color, 255 * transparency); // Button color with transparency

    // Button Scaled
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;
    ofDrawRectRounded(x + (width - scaledWidth) / 2, y + (height - scaledHeight) / 2, scaledWidth, scaledHeight, 10); // Draw rounded rectangle

    ofSetColor(255, 255 * transparency); // Reset color to white with transparency for the icon

    // Icon Scaled
    float iconSize = scaledHeight * 0.8; // Assuming square icons
    float iconX = x + (width - scaledWidth) / 2 + 10; // Padding of 10
    float iconY = y + (height - scaledHeight) / 2 + (scaledHeight - iconSize) / 2;
    player.image.draw(iconX, iconY, iconSize, iconSize); // Draw icon in the center-left

    // Text
    float textX = iconX + iconSize + 10; // Padding of 10
    float textY = y + (height - scaledHeight) / 2 + scaledHeight / 2 + 5; // Slightly below center

    ofSetColor(255, 255 * transparency); // Set text color with transparency
    ofDrawBitmapString(player.name + " #" + std::to_string(player.number), textX, textY);

    // Reset color
    ofSetColor(255, 255, 255);
}

//--------------------------------------------------------------
void VerticalElementList::mouseDragged(float mouseX, float mouseY) {
    if (isDragging) {
        ofLog() << "ScrollableButtonList dragged.";
        float deltaY = dragStartY - mouseY;
        scrollOffset += deltaY;
        dragStartY = mouseY;
        clampScroll();
    }
}

void VerticalElementList::mousePressed(float mouseX, float mouseY) {
    ofLog() << "ScrollableButtonList pressed.";
    // Record the initial mouse position
    dragStartX = mouseX;
    dragStartY = mouseY;

    // Check if the mouse is pressed within the bounds of a button
    float width = elementWidth;
    float height = elementHeight * players.size() + 4 * elementHeight * players.size();
    if (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height) {
        isDragging = true;
    }
}

void VerticalElementList::mouseReleased(float mouseX, float mouseY) {
    ofLog() << "ScrollableButtonList released.";
    // Check if the mouse has moved significantly
    float dragDistance = std::abs(mouseX - dragStartX) + std::abs(mouseY - dragStartY);
    if (dragDistance < dragThreshold) {
        // If not dragged, treat it as a click and perform the action
        ofLog() << "ScrollableButtonList clicked.";
        if (onClickListener) {
            onClickListener();
        }
    }

    // Reset dragging flag
    isDragging = false;
}

void VerticalElementList::setOnClickListener(std::function<void()> listener) {
    onClickListener = listener;
}

//--------------------------------------------------------------
void VerticalElementList::clampScroll() {
    float height = elementHeight * players.size();
    float maxScroll = std::max(0.0f, elementHeight * 1.5f);
    if (scrollOffset < -4.5f * elementHeight) scrollOffset = -4.5f * elementHeight;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}

//--------------------------------------------------------------
Player VerticalElementList::getSelectedPlayer() {
    return selectedPlayer;
}
