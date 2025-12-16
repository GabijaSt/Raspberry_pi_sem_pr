#ifndef KEYPRESS_H
#define KEYPRESS_H

#include "PushButton.h"                                                     // Base Button class

class KeyPress : public Button {
public:
    KeyPress();                                                             // Constructor (auto-configures GPIO10)
    bool isPressed();                                                       // Checks if button is pressed
};

#endif // KEYPRESS_H

