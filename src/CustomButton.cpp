#include "CustomButton.h"

CustomButton::CustomButton(float x, float y, float width, float height, ofImage icon, ofColor color)
    : x(x), y(y), width(width), height(height), icon(icon), color(color) {}

void CustomButton::draw() {
    ofSetColor(color); // Button color
    ofDrawRectRounded(x, y, width, height, 10); // Draw rounded rectangle

    if (icon.isAllocated()) {
        ofSetColor(255); // Reset color to white for the icon (if they are white in the images, this ensures they stay white)

        // Calculate the position to center the icon
        float iconX = x + (width - icon.getWidth()) / 2;
        float iconY = y + (height - icon.getHeight()) / 2;

        icon.draw(iconX, iconY); // Draw icon in the center
    }
    else {
        ofLog() << "Icon is not allocated";
    }
}

void CustomButton::drawTransparentAndScale(float transparency, float scale) {
    ofSetColor(color, 255 * transparency); // Button color with transparency

    //Button Scaled
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;
    ofDrawRectRounded(x + (width - scaledWidth)/2, y + (height - scaledHeight)/2, scaledWidth, scaledHeight, 10); // Draw rounded rectangle

    if (icon.isAllocated()) {
        ofSetColor(255, 255 * transparency); // Reset color to white with transparency for the icon

        //Icon Scaled
        float scaledIconWidth = icon.getWidth() * scale;
        float scaledIconHeight = icon.getHeight() * scale;

        // Calculate the position to center the icon
        float iconX = x + (width*scale - scaledIconWidth) / 2 + (width - scaledWidth) / 2;
        float iconY = y + (height*scale - scaledIconHeight) / 2 + (height - scaledHeight) / 2;

        icon.draw(iconX, iconY, scaledIconWidth, scaledIconHeight); // Draw icon in the center
    }
    else {
        ofLog() << "Icon is not allocated";
    }
}

bool CustomButton::isMouseOver(float mouseX, float mouseY) {
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

void CustomButton::mousePressed(float mouseX, float mouseY) {
    if (isMouseOver(mouseX, mouseY)) {
        ofLog() << "CustomButton at (" << x << ", " << y << ") clicked.";
        if (onClickListener) {
            onClickListener();
        }
    }
}

void CustomButton::setOnClickListener(std::function<void()> listener) {
    onClickListener = listener;
}

float CustomButton::getX() {
    return x;
}

float CustomButton::getY() {
    return y;
}