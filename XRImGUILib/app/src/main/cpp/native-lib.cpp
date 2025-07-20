#include <jni.h>
#include <string>
#include <thread>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <android/log.h>
#include "ImGUIServer.hpp"

void Update(std::string name, EntryType type, bool toggleValue, float sliderValue) {
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
    }
}

ImGUIServer* IMGUISERVER = nullptr; //Cannot be called server or it compiler freaks out

__attribute__((constructor))
void lib_main() {
    IMGUISERVER = new ImGUIServer(
    Data({
        Window("Hello Window", {
            Entry("ToggleTest", toggle, false),
            Entry("ToggleTest2", toggle, true),
            Entry("Button", button, true),
            Entry("Color", colorpicker, true)
        })
        }),
        Update
    );
}

