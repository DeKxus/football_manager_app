#include "ScrollableButtonList.h"

ScrollableButtonList::ScrollableButtonList(float x, float y, float buttonWidth, float buttonHeight, std::vector<Team> teams, ofColor color)
    : x(x), y(y), buttonWidth(buttonWidth), buttonHeight(buttonHeight), color(color), teams(teams), dragThreshold(1.0f), selectedTeam(), scrollOffset(0), isDragging(false) {}

void ScrollableButtonList::draw() {
    ofPushMatrix();
    ofTranslate(x - scrollOffset - 25, y);

    //Screen characteristics
    float spacing = 10.0f;
    float screenWidth = 1366.0f;
    float fadeWidth = (screenWidth - buttonWidth / 2.0f) / 2.0f; // Width of the area where buttons will start to fade
    float totalWidth = 0;

    // Variables to keep track of the team with the highest scale
    float maxScale = 0.0f;
    Team* maxScaleTeam = nullptr;

    for (size_t i = 0; i < 2; i++)
    {
        drawEmptyListElement(0, 0, buttonWidth, buttonHeight, ofColor(0x444444));
        ofTranslate(buttonWidth + (spacing), 0);
        totalWidth += buttonWidth + (spacing);
    }

    for (auto& team : teams) {
        
        // Calculate the distance from the button center to the nearest edge where it should start fading
        float buttonCenterX = x - scrollOffset + totalWidth + buttonWidth / 3.0f;
        float distanceToFadeStart = std::min(buttonCenterX, screenWidth - buttonCenterX);

        // Calculate the transparency
        float transparency = ofMap(distanceToFadeStart, 0, fadeWidth, 0, 1, true);
        // Calculate the scale
        float scale = ofMap(distanceToFadeStart, 0, fadeWidth, 0.7, 1, true);

        // Track the team with the highest scale
        if (scale > maxScale) {
            maxScale = scale;
            maxScaleTeam = &team;
        }

        drawListElement(0, 0, buttonWidth, buttonHeight, color, transparency, scale, team.logo);

        ofTranslate(buttonWidth + (spacing * scale) , 0);
        totalWidth += buttonWidth + (spacing * scale);
    }

    // Save the team with the highest scale in the selectedTeam variable
    if (maxScaleTeam) {
        selectedTeam = *maxScaleTeam;
    }

    ofPopMatrix();
}

void  ScrollableButtonList::drawListElement(float x, float y, float width, float height, ofColor color, float transparency, float scale, ofImage icon) {
    
    ofSetColor(color, 255 * transparency); // Button color with transparency

    //Button Scaled
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;
    ofDrawRectRounded(x + (width - scaledWidth) / 2, y + (height - scaledHeight) / 2, scaledWidth, scaledHeight, 10); // Draw rounded rectangle

    ofSetColor(255, 255 * transparency); // Reset color to white with transparency for the icon

    //Icon Scaled
    float scaledIconWidth = icon.getWidth() * scale;
    float scaledIconHeight = icon.getHeight() * scale;

    // Calculate the position to center the icon
    float iconX = x + (width * scale - scaledIconWidth) / 2 + (width - scaledWidth) / 2;
    float iconY = y + (height * scale - scaledIconHeight) / 2 + (height - scaledHeight) / 2;

    icon.draw(iconX, iconY, scaledIconWidth, scaledIconHeight); // Draw icon in the center

    //Reset
    ofSetColor(255, 255, 255);
}

void  ScrollableButtonList::drawEmptyListElement(float x, float y, float width, float height, ofColor color) {

    ofSetColor(color); // Button color with transparency

    //Button Scaled
    float scaledWidth = width;
    float scaledHeight = height;
    ofDrawRectRounded(x + (width - scaledWidth) / 2, y + (height - scaledHeight) / 2, scaledWidth, scaledHeight, 10); // Draw rounded rectangle

    //Reset
    ofSetColor(255, 255, 255);
}

//--------------------------------------------------------------
void ScrollableButtonList::mouseDragged(float mouseX, float mouseY) {
    if (isDragging) {
        ofLog() << "ScrollableButtonList dragged.";
        float deltaX = dragStartX - mouseX;
        scrollOffset += deltaX;
        dragStartX = mouseX;
        clampScroll();
    }
}

void ScrollableButtonList::mousePressed(float mouseX, float mouseY) {
    ofLog() << "ScrollableButtonList pressed.";
    // Record the initial mouse position
    dragStartX = mouseX;
    dragStartY = mouseY;

    // Check if the mouse is pressed within the bounds of a button
    float width = buttonWidth * 5;
    float height = buttonHeight;
    if (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height) {
        isDragging = true;
    }
}

void ScrollableButtonList::mouseReleased(float mouseX, float mouseY) {
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

void ScrollableButtonList::setOnClickListener(std::function<void()> listener) {
    onClickListener = listener;
}

//--------------------------------------------------------------
void ScrollableButtonList::clampScroll() {
    float width = buttonWidth * 5;
    float height = buttonHeight;

    float maxScroll = std::max(0.0f, teams.size() * buttonWidth - width + buttonWidth * 4);
    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}

//--------------------------------------------------------------
Team ScrollableButtonList::getSelectedTeam() {
    return selectedTeam;
}

