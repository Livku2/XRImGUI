// dear imgui: standalone example application for Android + OpenGL ES 3

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <string>
#include "Client.hpp"
#include "map"
#include "array"

Client* client;

static EGLDisplay           g_EglDisplay = EGL_NO_DISPLAY;
static EGLSurface           g_EglSurface = EGL_NO_SURFACE;
static EGLContext           g_EglContext = EGL_NO_CONTEXT;
static struct android_app*  g_App = nullptr;
static bool                 g_Initialized = false;
static char                 g_LogTag[] = "ImGuiExample";
static std::string          g_IniFilename = "";

static void Init(struct android_app* app);
static void Shutdown();
static void MainLoopStep();

static void handleAppCmd(struct android_app* app, int32_t appCmd)
{
    switch (appCmd)
    {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            Init(app);
            break;
        case APP_CMD_TERM_WINDOW:
            Shutdown();
            break;
        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

static int32_t handleInputEvent(struct android_app* app, AInputEvent* inputEvent)
{
    return ImGui_ImplAndroid_HandleInputEvent(inputEvent);
}

void android_main(struct android_app* app)
{
    app->onAppCmd = handleAppCmd;
    app->onInputEvent = handleInputEvent;

    while (true)
    {
        int out_events;
        struct android_poll_source* out_data;

        while (ALooper_pollOnce(g_Initialized ? 0 : -1, nullptr, &out_events, (void**)&out_data) >= 0)
        {
            if (out_data != nullptr)
                out_data->process(app, out_data);

            if (app->destroyRequested != 0)
            {
                if (!g_Initialized)
                    Shutdown();

                return;
            }
        }

        MainLoopStep();
    }
}

void Init(struct android_app* app)
{
    if (g_Initialized)
        return;

    g_App = app;
    ANativeWindow_acquire(g_App->window);

    // Initialize EGL
    // This is mostly boilerplate code for EGL...
    {
        g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (g_EglDisplay == EGL_NO_DISPLAY)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");

        if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglInitialize() returned with an error");

        const EGLint egl_attributes[] = { EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
        EGLint num_configs = 0;
        if (eglChooseConfig(g_EglDisplay, egl_attributes, nullptr, 0, &num_configs) != EGL_TRUE)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglChooseConfig() returned with an error");
        if (num_configs == 0)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglChooseConfig() returned 0 matching config");

        // Get the first matching config
        EGLConfig egl_config;
        eglChooseConfig(g_EglDisplay, egl_attributes, &egl_config, 1, &num_configs);
        EGLint egl_format;
        eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
        ANativeWindow_setBuffersGeometry(g_App->window, 0, 0, egl_format);

        const EGLint egl_context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, egl_context_attributes);

        if (g_EglContext == EGL_NO_CONTEXT)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglCreateContext() returned EGL_NO_CONTEXT");

        g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, g_App->window, nullptr);
        eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    g_IniFilename = std::string(app->activity->internalDataPath) + "/imgui.ini";
    io.IniFilename = g_IniFilename.c_str();;

    ImGui::StyleColorsDark();

    ImGui_ImplAndroid_Init(g_App->window);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    ImGui::GetStyle().ScaleAllSizes(3.0f);

    g_Initialized = true;
}

std::map<std::string, bool> toggleMap;
std::map<std::string, float> sliderMap;
std::map<std::string, std::array<float, 3>> colorPickerMap;

void MainLoopStep()
{
    ImGuiIO& io = ImGui::GetIO();
    if (g_EglDisplay == EGL_NO_DISPLAY)
        return;

    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    if(!client){
        client = new Client();
    }
    {
        for (auto &window : client->windows){
            ImGui::Begin(window.name.c_str());

            for(auto &entry : window.entries) {
                if(entry.newline) ImGui::NewLine();

                if(entry.type == toggle){
                    if (toggleMap.find(entry.text) == toggleMap.end()) {
                        toggleMap[entry.text] = false;
                    }
                    if(ImGui::Checkbox(entry.text.c_str(), &toggleMap[entry.text])){
                        client->SendUpdate(entry.text, toggle, toggleMap[entry.text], 0.f, std::array<float, 3>());
                    }
                }
                else if(entry.type == button){
                    if(ImGui::Button(entry.text.c_str())) {
                        client->SendUpdate(entry.text, button, false, 0.f, std::array<float, 3>());
                    }
                }
                else if(entry.type == slider){
                    if (sliderMap.find(entry.text) == sliderMap.end()) {
                        sliderMap[entry.text] = entry.slider.start;
                    }
                    if(ImGui::SliderFloat(entry.text.c_str(), &sliderMap[entry.text], entry.slider.min, entry.slider.max)){
                        client->SendUpdate(entry.text, slider, false, sliderMap[entry.text], std::array<float, 3>());
                    }
                }
                else if (entry.type == colorpicker) {
                    if (colorPickerMap.find(entry.text) == colorPickerMap.end()) {
                        colorPickerMap[entry.text] = entry.hexColor;
                    }
                    float cols[3] { colorPickerMap[entry.text][0], colorPickerMap[entry.text][1], colorPickerMap[entry.text][2] };
                    if (ImGui::ColorPicker3(entry.text.c_str(), cols)) {
                        client->SendUpdate(entry.text, colorpicker, false, 0.f, std::array<float, 3>{cols[0], cols[1], cols[2]});
                        colorPickerMap[entry.text] = std::array<float, 3> {cols[0], cols[1], cols[2]};
                    }
                }
            }
            ImGui::End();
        }
    }

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(g_EglDisplay, g_EglSurface);
}

void Shutdown()
{
    if (!g_Initialized)
        return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();

    if (g_EglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(g_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (g_EglContext != EGL_NO_CONTEXT)
            eglDestroyContext(g_EglDisplay, g_EglContext);

        if (g_EglSurface != EGL_NO_SURFACE)
            eglDestroySurface(g_EglDisplay, g_EglSurface);

        eglTerminate(g_EglDisplay);
    }

    g_EglDisplay = EGL_NO_DISPLAY;
    g_EglContext = EGL_NO_CONTEXT;
    g_EglSurface = EGL_NO_SURFACE;
    ANativeWindow_release(g_App->window);

    g_Initialized = false;
}