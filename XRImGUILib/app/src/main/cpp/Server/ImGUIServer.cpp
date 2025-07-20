#include "ImGUIServer.hpp"

void to_json(json &j, const Entry &e) {
    j = json{{"text", e.text}, {"type", e.type}, {"newline", e.newline}};
}
void to_json(json &j, const Window &w) {
    j = json{{"name", w.name}, {"entries", w.entries}};
}
void to_json(json &j, const Data &d) {
    j = json{{"windows", d.windows}};
}
inline void from_json(const json& j, EntryType& e) {
    e = static_cast<EntryType>(j.get<int>());
}

Server server;

void ImGUIServer::Setup(Data setupData) {
    auto sharedData = std::make_shared<Data>(std::move(setupData));
    std::thread([this, sharedData]() {

        server.Get("/Setup", [sharedData](const Request &req, Response &res) {
            LOGI("SetupCalled");

            json jsonData = *sharedData;
            res.set_content(jsonData.dump(), "application/json");
        });

        server.Post("/Update", [this](const Request &req, Response &res) {
            
            json jsonData = json::parse(req.body);
            std::string name = jsonData["name"];
            auto type = static_cast<EntryType>(jsonData["type"].get<int>());
            bool toggleValue = jsonData["toggleValue"].get<bool>();
            float sliderValue = jsonData["sliderValue"].get<float>();

            if(UpdateFunction){
                UpdateFunction(name,type,toggleValue,sliderValue);
            }
        });

        bool res = server.listen("0.0.0.0", 5000);
        if (!res) {
            LOGE("Server failed");
        }
    }).detach();
}
