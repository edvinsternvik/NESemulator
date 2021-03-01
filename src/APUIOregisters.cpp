#include "APUIOregisters.h"
#include "Controller.h"

void APUIOregisters::connectController(std::shared_ptr<Controller> controller, bool isController1) {
    if(isController1) m_controller1 = controller;
    else m_controller2 = controller;
}

uint8_t APUIOregisters::getControllerData(Controller* controller) {
    if(controller == nullptr) return 0;

    union ControllerData {
        struct Layout {
            uint8_t A : 1;
            uint8_t B : 1;
            uint8_t Select : 1;
            uint8_t Start : 1;
            uint8_t Up : 1;
            uint8_t Down : 1;
            uint8_t Left : 1;
            uint8_t Right : 1;
        };
        uint8_t data;
        Layout layout;
    };

    ControllerData cd;
    cd.layout.A = controller->A;
    cd.layout.B = controller->B;
    cd.layout.Select = controller->Select;
    cd.layout.Start = controller->Start;
    cd.layout.Up = controller->Up;
    cd.layout.Down = controller->Down;
    cd.layout.Left = controller->Left;
    cd.layout.Right = controller->Right;
    
    return cd.data;
}