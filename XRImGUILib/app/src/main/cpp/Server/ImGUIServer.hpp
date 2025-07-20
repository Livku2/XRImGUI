#include <string>
#include <thread>
#include "../httplib.h"
#include "nlohmann/json.hpp"
#include <android/log.h>
#include <functional>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Menu", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Menu", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Menu", __VA_ARGS__)

using namespace httplib;
using namespace nlohmann;

enum EntryType{
    slider,
    toggle,
    button
};
struct Slider{
    float min, max, start;
};
struct Entry {
    std::string text;
    EntryType type;
    bool newline;
    Slider slider;

    Entry(const std::string &text, const EntryType &type, bool newline, Slider slider = {0,0,0}) : text(text), type(type), newline(newline), slider(slider) {}
};
struct Window {
    std::string name;
    std::vector<Entry> entries;

    Window(const std::string &name, const std::vector<Entry> &entries) : name(name), entries(entries) {}
};
struct Data {
    std::vector<Window> windows;

    Data(std::vector<Window> windows) : windows(windows) {}
};

class ImGUIServer {
public:
    std::function<void(std::string name, EntryType type, bool toggleValue, float sliderValue)> UpdateFunction = nullptr;

    ImGUIServer(Data setupData, std::function<void(std::string name, EntryType type, bool toggleValue, float sliderValue)> updateFunction){
        UpdateFunction = updateFunction;
        Setup(setupData);
    }
private:
    void Setup(Data setupData);
};
