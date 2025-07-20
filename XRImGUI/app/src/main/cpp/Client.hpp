#include "httplib.h"
#include "nlohmann/json.hpp"
#include "array"
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
    bool newline;
    Slider slider;
    std::array<float, 3> hexColor;

    Entry(const std::string &text, const EntryType &type, bool newline, Slider slider = {0,0,0}, std::array<float, 3> hexColor = {0,0,0}) : text(text), type(type), newline(newline), slider(slider), hexColor(hexColor) {}
};

struct Window {
    std::string name;
    std::vector<Entry> entries;

    Window(const std::string &name) : name(name) {}
    Window(const std::string &name, const std::vector<Entry> &entries) : name(name), entries(entries) {}
};

struct Data {
    std::vector<Window> windows;
};


class Client {
public:
    std::vector<Window> windows;

    void SendUpdate(std::string name, EntryType type, bool toggleValue, float sliderValue, std::array<float, 3> colorValue);

    Client(){
        Setup();
    }
private:
    void Setup();
};
