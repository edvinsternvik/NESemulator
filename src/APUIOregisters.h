#pragma once
#include "BussDevice.h"
#include <memory>

class Controller;

class APUIOregisters : public BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) override {
        uint8_t data = 0;
        if(address == 0x16) {
            if(m_controllerStrobe) {
                m_controller1Data = getControllerData(m_controller1.get());
            }
            data = m_controller1Data & 0x1;
            m_controller1Data >>= 1;
        }
        else if(address == 0x17) {
            if(m_controllerStrobe) {
                m_controller2Data = getControllerData(m_controller2.get());
            }
            data = m_controller2Data & 0x1;
            m_controller2Data >>= 1;
        }
        
        return data;
    }

    virtual void write(const uint16_t& address, const uint8_t& data) override {
        if(address == 0x16) {
            // When controllerStrobe is false, the controllerData variables are no longer updated before reading
            // so we update it's value one last time here            
            if(m_controllerStrobe && (data & 0x1) == 0) { // If controller strobe is being disabled
                m_controller1Data = getControllerData(m_controller1.get());
                m_controller2Data = getControllerData(m_controller2.get());
            }

            m_controllerStrobe = data & 0x1;
        }
    }

    void connectController(std::shared_ptr<Controller> controller, bool isController1);
    Controller* getController1() { return m_controller1.get(); }
    Controller* getController2() { return m_controller2.get(); }

private:
    uint8_t getControllerData(Controller* controller);

private:
    bool m_controllerStrobe = false;
    std::shared_ptr<Controller> m_controller1;
    std::shared_ptr<Controller> m_controller2;

    uint8_t m_controller1Data;
    uint8_t m_controller2Data;
};