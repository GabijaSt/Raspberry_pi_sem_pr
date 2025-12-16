#include "KeyPress.h"                                                       // Class declaration
KeyPress::KeyPress()
    : Button(10, GPIO_IRQ_EDGE_RISE) { }                                    // PicoBricks button on GPIO10

bool KeyPress::isPressed() {
    return hasEvent();                                                      // Reuse base class functionality
}

