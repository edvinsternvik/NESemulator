#include "NES.h"
#include "Controller.h"
#include "GraphicsLibrary.h"

#include <iostream>
#include <cstring>

class NesApplication : public rgl::Application {
public:
    NesApplication(std::shared_ptr<Cartridge> cartridge) {
        nes.loadCartridge(cartridge);

        controller1 = std::make_shared<Controller>();
        nes.connectController1(controller1);

        nes.reset();
    }

    virtual void onUpdate() override {
        while(true) {
            nes.cpu.clock();
            for(int i = 0; i < 3; ++i) {
                nes.ppu->clock();
            }

            if(nes.ppu->nmi()) {
                nes.cpu.nmiPin = true;
            }

            if(nes.ppu->frameDone) {
                nes.ppu->frameDone = false;
                break;
            }
        }


        renderFrame();

        setControllerInput();
    }

    virtual void onEvent(rgl::Event* event) override {
        if(event->GetEventType() == rgl::EventType::WindowResize) {
            // Clear screen
            int wbs = window->getWindowBufferSize();
            char* wb = window->getWindowBuffer();
            std::memset(wb, 0, wbs);
        }
    }

private:
    void renderFrame() {
        int widthScale = window->getWindowSize().x / 256;
        int heightScale = window->getWindowSize().y / 240;
        for(int i = 0; i < 256; ++i) {
            for(int j = 0; j < 240; ++j) {
                char* wb = window->getWindowBuffer();
                unsigned int pv = *(nes.ppu->windowBuffer + i + 256 * j);

                int offset = i * widthScale + window->getWindowSize().x * j * heightScale;
                for(int x = 0; x < widthScale; ++x) {
                    for(int y = 0; y < heightScale; ++y) {
                        int pp = (offset + x + y * window->getWindowSize().x) * window->bytesPerPixel;
                        *((unsigned int*)(wb + pp)) = pv;
                    }
                }
            }
        }
    }

    void setControllerInput() {
        controller1->A = rgl::Input::GetKey(rgl::Z);
        controller1->B = rgl::Input::GetKey(rgl::X);
        controller1->Select = rgl::Input::GetKey(rgl::A);
        controller1->Start = rgl::Input::GetKey(rgl::S);
        controller1->Up = rgl::Input::GetKey(rgl::Up);
        controller1->Down = rgl::Input::GetKey(rgl::Down);
        controller1->Left = rgl::Input::GetKey(rgl::Left);
        controller1->Right = rgl::Input::GetKey(rgl::Right);
    }

    NES nes;
    std::shared_ptr<Controller> controller1;
};

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cout << "No cartridge specified" << std::endl;
        return 0;
    }

    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(argv[1]);
    if(!cartridge->successfulLoad()) {
        std::cout << "Could not load cartridge" << std::endl;
        return 0;
    }

    NesApplication app(cartridge);
    app.create("NES", rgl::Vector2i(0,0), rgl::Vector2i(256, 240) * 4);
    app.run();
}