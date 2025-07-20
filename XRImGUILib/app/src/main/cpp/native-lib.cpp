#include <jni.h>
#include <string>
#include <thread>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <android/log.h>
#include "ImGUIServer.hpp"

void Update(std::string name, EntryType type, bool toggleValue, float sliderValue, std::array<float,3> colorValue) {
    switch(type){
        case button:
            if(name == "Button"){
                LOGI("Button Pressed");
            }
            break;
        case slider:
            break;
        case toggle:
            LOGI("Toggle Changed With Name %s And Value %b", name.c_str(), toggleValue);
            break;
        case colorpicker:
            LOGI("Color: %f, %f, %f", colorValue[0], colorValue[1], colorValue[2]);
            break;
    }
}

ImGUIServer* IMGUISERVER = nullptr; //Cannot be called server or it compiler freaks out

__attribute__((constructor))
void lib_main() {
    IMGUISERVER = new ImGUIServer(
    Data({
        Window("Hello Window", {
            Entry("ToggleTest", toggle, false),
            Entry("ToggleTest2", toggle, false),
            Entry("Button", button, false),
            Entry("Color", colorpicker, false),
        })
        }),
        Update
    );
}

