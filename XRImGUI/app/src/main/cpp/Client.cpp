#include "android/log.h"
#include "Client.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Menu", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Menu", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Menu", __VA_ARGS__)

httplib::Client cli("http://localhost:5000");

void Client::Setup() {
    LOGI("1");

    auto setupres = cli.Get("/Setup");

    if (!setupres) {
        LOGE("RESULT IS NULL");
        return;
    }
    LOGI("2");

    nlohmann::json jsonData;
    try {
        jsonData = nlohmann::json::parse(setupres->body);
    } catch (const std::exception &e) {
        LOGE("Failed to parse JSON: %s", e.what());
        return;
    }
    LOGI("3");

    if (jsonData.contains("windows") && jsonData["windows"].is_array()) {
        for (const auto &window : jsonData["windows"]) {
            if (!window.contains("name") || !window.contains("entries")) {
                LOGE("Invalid window structure");
                continue;
            }

            std::string name = window["name"];
            std::vector<Entry> entries;

            for (const nlohmann::json &entry : window["entries"]) {
                if (!entry.contains("text") || !entry.contains("type")) {
                    LOGE("Invalid entry structure");
                    continue;
                }

                std::string text = entry["text"].get<std::string>();
                EntryType type = entry["type"].get<EntryType>();
                bool newline = entry["newline"].get<bool>();

                entries.emplace_back(text, type, newline);
                LOGI("Entry: %s (Type %d)", text.c_str(), type);
            }

            windows.emplace_back(name, entries);
            LOGI("Window: %s", name.c_str());
        }
    } else {
        LOGE("NO WINDOWS");
    }
}

using namespace nlohmann;

void Client::SendUpdate(std::string name, EntryType type, bool toggleValue, float sliderValue, std::array<float, 3> colorValue) {
    auto j = json{{"name", name}, {"type", type}, {"toggleValue", toggleValue}, {"sliderValue", sliderValue}, {"colorValue", colorValue}};

    auto res = cli.Post("/Update", j.dump(), "application/json");

    if(!res){
        LOGE("Update Failed");
    }
    else{
        LOGI("Update Sent");
    }
}