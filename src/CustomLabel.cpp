#include "CustomLabel.h"

CustomLabel::CustomLabel(float x, float y, float width, float height, ofImage icon, std::string text, ofColor labelColor)
    : x(x), y(y), width(width), height(height), icon(icon), text(text), color(labelColor) {}

void CustomLabel::draw() {
    // Set color for the rounded rectangle background
    ofSetColor(color);

    // Draw the rounded rectangle
    ofDrawRectRounded(x, y, width, height, 10);

    // Calculate positions for the icon and text
    float iconX = x + (width - icon.getWidth()) / 2;
    float iconY = y + height * 0.2; // Top 10% margin for the icon
    float textX = x + 10; // Left margin for the text
    float textY = y + height * 0.6; // Centered vertically in the bottom half

    // Draw the icon centered in the top half
    if (icon.isAllocated()) {
        ofSetColor(255); // Reset color to white for the icon (if they are white in the images, this ensures they stay white)

        icon.draw(iconX, iconY); // Draw icon in the center
    }
    else {
        ofLog() << "Icon is not allocated";
    }
    // Set color for the text
    ofSetColor(255);
    ofDrawBitmapString(text, textX, textY);

}