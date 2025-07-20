#include <string>
#include <thread>
#include "../httplib.h"
#include "nlohmann/json.hpp"
#include <android/log.h>
#include <functional>
#include "array"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Menu", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Menu", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Menu", __VA_ARGS__)

using namespace httplib;
using namespace nlohmann;

enum EntryType{
    slider,
    toggle,
    button,
    colorpicker
};
struct Slider{
    float min, max, start;
};
struct Entry {
    std::string text;
    EntryType type;
    bool sameline;
    Slider slider;
    std::array<float, 3> hexColor;

    Entry(const std::string &text, const EntryType &type, bool sameline, Slider slider = {0,0,0}, std::array<float, 3> hexColor = {0,0,0}) : text(text), type(type), sameline(sameline), slider(slider), hexColor(hexColor) {}
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
    std::function<void(std::string name, EntryType type, bool toggleValue, float sliderValue, std::array<float, 3> colorValue)> UpdateFunction = nullptr;

    ImGUIServer(Data setupData, std::function<void(std::string name, EntryType type, bool toggleValue, float sliderValue, std::array<float, 3> colorValue)> updateFunction){
        UpdateFunction = updateFunction;
        Setup(setupData);
    }
private:
    void Setup(Data setupData);
};
