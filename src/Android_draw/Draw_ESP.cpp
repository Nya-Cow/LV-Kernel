#include "draw.h"
#include "Draw_ESP.h"
#include "Yuaner.h"
#include "AndroidImgui.h"
#include "oxorany.h"
#include "FONTS/New.h"
#include "XXX/DOT.h"
#include <imgui.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <chrono>
#include <string>
extern bool 悬浮窗;
std::string filenama = "单透模式.txt";
std::ifstream file(filenama);
ANativeWindow *native_window;
EGLContext context = EGL_NO_CONTEXT;
EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface surface = EGL_NO_SURFACE;
EGLConfig config = nullptr;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;

#include <android/api-level.h>
int init_egl(int _screen_x,int _screen_y, bool flp ,bool log)
{
	native_window = android::ANativeWindowCreator::Create("Ssage", _screen_x, _screen_y, flp);
	ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        printf("eglGetDisplay error=%u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglGetDisplay ok\n");
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        printf("eglInitialize error=%u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglInitialize ok\n");
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_BUFFER_SIZE, 32,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    if (eglChooseConfig(display, attribList, nullptr, 0, &num_config) != EGL_TRUE) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("num_config=%d\n", num_config);
    }
    if (!eglChooseConfig(display, attribList, &config, 1, &num_config)) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglChooseConfig ok\n");
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        printf("eglCreateContext  error = %u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglCreateContext ok\n");
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface  error = %u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglCreateWindowSurface ok\n");
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        printf("eglMakeCurrent  error = %u\n", glGetError());
        return -1;
    }
    if (log) {
        printf("eglMakeCurrent ok\n");
    }
    return 1;
}

ImU32 GetGradientColor(float time) {
    float hue = fmodf(time * 40.0f, 360.0f);
    float saturation = 1.0f;
    float value = 1.0f;
    float alpha = 255.0f;

    ImVec4 rgb = ImColor::HSV(hue / 360.0f, saturation, value);
    return IM_COL32((int)(rgb.x * 255), (int)(rgb.y * 255), (int)(rgb.z * 255), (int)alpha);
}

void DrawCircleBorder(ImDrawList* imDrawList, ImVec2 center, float radius, ImU32 color, float thickness) {
    imDrawList->AddCircle(center, radius, color, 0, thickness);
}

float menu[4] = { 0/255.f, 255/255.f, 255/255.f, 1.000f };
int TABG = 1;
bool active = false;
ImFont* JAAT = nullptr;
pid_t obj_pid = -1;
Config globalConfig;
FILE *numSave2 = nullptr;
TextureInfo WeaponPic[100]{0};
c_driver *driver = NULL;
static ImDrawList *gui_drawList;//封装绘图
static ImVec2 Menuscreen;
int getPID(const char *PackageName) {
    FILE *fp;
    pid_t local_pid = -1;
    char cmd[0x100] = "pidof ";
    strcat(cmd, PackageName);
    fp = popen(cmd, "r");
    fscanf(fp, "%d", &local_pid);
    pclose(fp);
    return local_pid;
}
bool PidExamIne(pid_t pid_) {
    char path[128];
    sprintf(path, "/proc/%d", pid_);
    if (access(path, F_OK) != 0) {
        printf("\033[37;1m");
        puts("请先打开和平精英再执行");
        exit(1);
    }
    return true;
}

void DrawTextY(std::string text, ImVec2 pos, ImU32 color, bool outline, float fontSize) {
    const char* str = text.c_str();
    ImVec2 vec2 = pos;

    if (outline) {
        ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
        gui_drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(vec2.x + 1, vec2.y + 1), outlineColor, str);
        gui_drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(vec2.x - 1, vec2.y - 1), outlineColor, str);
        gui_drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(vec2.x + 1, vec2.y - 1), outlineColor, str);
        gui_drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(vec2.x - 1, vec2.y + 1), outlineColor, str);
    }
    gui_drawList->AddText(ImGui::GetFont(), fontSize, vec2, color, str);
}


float calcTextSizeY(const char *text, float font_size) {
    ImVec2 text_size;
    if (font_size == 0) {
        text_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFont()->FontSize, FLT_MAX, -1.0f, text, NULL, NULL);
    } else {
        text_size = ImGui::GetFont()->CalcTextSizeA(font_size, FLT_MAX, -1.0f, text, NULL, NULL);
    }
    text_size.x = IM_FLOOR(text_size.x + 0.99999f);
    return text_size.x;
}

void FixTriangle(float &XPos, float &YPos, int screenDist) {
    if (XPos > Menuscreen.x) {
        XPos = Menuscreen.x;
        XPos -= screenDist + 35;
    }

    if (XPos < 0) {
        XPos = 0;
        XPos += screenDist + 35;
    }

    if (YPos > Menuscreen.y) {
        YPos = Menuscreen.y;
        YPos -= screenDist;
    }
    if (YPos < 0) {
        YPos = 0;
        YPos += screenDist;
    }
}

void Line(ImDrawList *draw, ImVec2 origin, ImVec2 dest, ImColor color, float thickness) {
    draw->AddLine({origin.x, origin.y}, {dest.x, dest.y}, color, 2.0f);
}



void NumIoSave(const char *name) {
    if (numSave2 == nullptr) {
        std::string SaveFile = "/data/";
        SaveFile += "/";
        SaveFile += name;
        numSave2 = fopen(SaveFile.c_str(), "wb+");
    }
    fseek(numSave2, 0, SEEK_SET);
        // 写入配置项...
        fwrite(&globalConfig.FPS, sizeof(int), 1, numSave2);
        fwrite(&globalConfig.ray, sizeof(bool), 1, numSave2);//天线开关
    fwrite(&globalConfig.Bone, sizeof(bool), 1, numSave2);//骨骼开关
    fwrite(&globalConfig.weapon, sizeof(bool), 1, numSave2);//持枪开关
    fwrite(&globalConfig.OffScreen, sizeof(bool), 1, numSave2);//背线开关
    fwrite(&globalConfig.Health1, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.Health2, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.Name, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.Distance, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.Radar, sizeof(bool), 1, numSave2);      //背敌开关
    fwrite(&globalConfig.fangkuang, sizeof(bool), 1, numSave2);      //方框开关
    fwrite(&globalConfig.leida, sizeof(bool), 1, numSave2);      //雷达开关
    fwrite(&globalConfig.Grenade, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.Car, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.EscapeBox, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.Grenade, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.Box, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.Xinghao, sizeof(int), 1, numSave2);
    
    fwrite(&globalConfig.CharacterDis, sizeof(float), 1, numSave2);//距离限制
    fwrite(&globalConfig.CharacterAim, sizeof(float), 1, numSave2);//距离限制
    fwrite(&globalConfig.OffScreenmargin, sizeof(float), 1, numSave2);//背敌间距
    fwrite(&globalConfig.OffScreenBox, sizeof(int), 1, numSave2);//背敌间距
    fwrite(&globalConfig.AutoAim, sizeof(bool), 1, numSave2);//自瞄总开关
    fwrite(&globalConfig.ShotGunAim, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.SniperAim, sizeof(int), 1, numSave2);
    fwrite(&globalConfig.AimingState, sizeof(int), 1, numSave2);//判断方法
    fwrite(&globalConfig.Radius, sizeof(float), 1, numSave2);//范围大小
    fwrite(&globalConfig.IgnoreRoBot, sizeof(bool), 1, numSave2);//Aim忽略人机
    fwrite(&globalConfig.IgnoreFall, sizeof(bool), 1, numSave2);//Aim忽略倒地
    fwrite(&globalConfig.enemyHealthDecreased, sizeof(bool), 1, numSave2);//掉血自瞄
    fwrite(&globalConfig.AimLock, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.SmokeNoAim, sizeof(bool), 1, numSave2);
    fwrite(&globalConfig.radarOffsetX, sizeof(float), 1, numSave2);//雷达x
    fwrite(&globalConfig.radarOffsetY, sizeof(float), 1, numSave2);//雷达y
    fwrite(&globalConfig.CharacterRadius, sizeof(int), 1, numSave2);//人物身位
    fwrite(&globalConfig.AimSpeed, sizeof(float), 1, numSave2);//自瞄速度
    fwrite(&globalConfig.healthEnabled, sizeof(float), 1, numSave2);//血量开关
    fwrite(&globalConfig.AimPrediction, sizeof(float), 1, numSave2);//自瞄预判
    fwrite(&globalConfig.Pressure, sizeof(float), 1, numSave2);//压枪幅度
    fwrite(&globalConfig.anglemix, sizeof(float), 1, numSave2);//广角
    fwrite(&globalConfig.ShotGunDistance, sizeof(float), 1, numSave2);
    fwrite(&globalConfig.ShotDistance, sizeof(float), 1, numSave2);
    fwrite(&globalConfig.Aimshenxian, sizeof(float), 1, numSave2);//自瞄射线
    fwrite(&globalConfig.Yujing, sizeof(float), 1, numSave2);//被瞄
    fwrite(&globalConfig.radarLineLength, sizeof(float), 1, numSave2);//自瞄射线
        fflush(numSave2); // 刷新缓冲区，确保所有数据都写入到文件中
        fsync(fileno(numSave2)); // 确保数据写入到磁盘
    }

void NumIoLoad(const char *name) {
    if (numSave2 == nullptr) {
        std::string SaveFile = "/data/";
        SaveFile += "/";
        SaveFile += name;
        numSave2 = fopen(SaveFile.c_str(), "rb+");
    }
    if (numSave2 != nullptr) {
        fseek(numSave2, 0, SEEK_SET);
        // 读取配置项...
        fread(&globalConfig.FPS, sizeof(int), 1, numSave2);
        fread(&globalConfig.ray, sizeof(bool), 1, numSave2);//天线开关
        fread(&globalConfig.Bone, sizeof(bool), 1, numSave2);//骨骼开关
        fread(&globalConfig.weapon, sizeof(bool), 1, numSave2);//持枪开关
        fread(&globalConfig.OffScreen, sizeof(bool), 1, numSave2);//背线开关
        fread(&globalConfig.Health1, sizeof(bool), 1, numSave2);
        fread(&globalConfig.Health2, sizeof(bool), 1, numSave2);
        fread(&globalConfig.Name, sizeof(bool), 1, numSave2);
        fread(&globalConfig.Distance, sizeof(bool), 1, numSave2);
        fread(&globalConfig.Radar, sizeof(bool), 1, numSave2);     //背敌开关
        fread(&globalConfig.fangkuang, sizeof(bool), 1, numSave2);//方框开关
        fread(&globalConfig.leida, sizeof(bool), 1, numSave2);//雷达
        fread(&globalConfig.Grenade, sizeof(float), 1, numSave2);
        fread(&globalConfig.Car, sizeof(int), 1, numSave2);
        fread(&globalConfig.EscapeBox, sizeof(int), 1, numSave2);
        fread(&globalConfig.Grenade, sizeof(int), 1, numSave2);
        fread(&globalConfig.Box, sizeof(int), 1, numSave2);
        fread(&globalConfig.Xinghao, sizeof(int), 1, numSave2);
        fread(&globalConfig.CharacterDis, sizeof(float), 1, numSave2);//距离限制
        fread(&globalConfig.CharacterAim, sizeof(float), 1, numSave2);//距离限制
        fread(&globalConfig.OffScreenmargin, sizeof(float), 1, numSave2);//背敌间距
        fread(&globalConfig.OffScreenBox, sizeof(int), 1, numSave2);//背敌间距
        fread(&globalConfig.AutoAim, sizeof(bool), 1, numSave2);//自瞄总开关
        fread(&globalConfig.ShotGunAim, sizeof(bool), 1, numSave2);
        fread(&globalConfig.SniperAim, sizeof(int), 1, numSave2);
        fread(&globalConfig.AimingState, sizeof(int), 1, numSave2);//判断方法
        fread(&globalConfig.Radius, sizeof(float), 1, numSave2);//范围大小
        fread(&globalConfig.IgnoreRoBot, sizeof(bool), 1, numSave2);//Aim忽略人机
        fread(&globalConfig.IgnoreFall, sizeof(bool), 1, numSave2);//Aim忽略倒地
        fread(&globalConfig.enemyHealthDecreased, sizeof(bool), 1, numSave2);//掉血自瞄
        fread(&globalConfig.AimLock, sizeof(bool), 1, numSave2);
        fread(&globalConfig.SmokeNoAim, sizeof(bool), 1, numSave2);
        fread(&globalConfig.radarOffsetX, sizeof(float), 1, numSave2);//雷达x
        fread(&globalConfig.radarOffsetY, sizeof(float), 1, numSave2);//雷达y
        fread(&globalConfig.CharacterRadius, sizeof(int), 1, numSave2);//人物身位
        fread(&globalConfig.AimSpeed, sizeof(float), 1, numSave2);//自瞄速度
        fread(&globalConfig.healthEnabled, sizeof(float), 1, numSave2);//血量开关
        fread(&globalConfig.AimPrediction, sizeof(float), 1, numSave2);//自瞄预判
        fread(&globalConfig.Pressure, sizeof(float), 1, numSave2);//站立压枪
        fread(&globalConfig.anglemix, sizeof(float), 1, numSave2);//广角
        fread(&globalConfig.ShotGunDistance, sizeof(float), 1, numSave2);
        fread(&globalConfig.ShotDistance, sizeof(float), 1, numSave2);
        fread(&globalConfig.Aimshenxian, sizeof(float), 1, numSave2);//自瞄射线
        fread(&globalConfig.Yujing, sizeof(float), 1, numSave2);//被瞄
        fread(&globalConfig.radarLineLength, sizeof(float), 1, numSave2);
        } else {// 如果文件不存在或读取失败，则使用默认配置
        // 设置默认配置...
        globalConfig.FPS = 60;//渲染帧数
        globalConfig.healthEnabled = true;//血量开关
        globalConfig.ray = true;//天线开关
        globalConfig.Bone = true;//骨骼开关
        globalConfig.weapon = true;//持枪开关
        globalConfig.Health1 = true;//生命条1
        globalConfig.Name = true;//名称开关
        globalConfig.Distance = true;//距离
        globalConfig.Radar = true;//背敌开关
        globalConfig.Grenade = true;//手雷预警
        globalConfig.Car = false;//车辆
        globalConfig.fangkuang =false;//方框
        globalConfig.leida =false;//雷达
        globalConfig.OffScreenmargin = 350.0f;//背后敌人
        globalConfig.OffScreenBox = 40;//背后敌人方框间距
        globalConfig.CharacterDis = 350.0f;//显示距离
        globalConfig.AutoAim = true; //自瞄总开关
        globalConfig.ShotGunAim = true;//喷子自瞄
        globalConfig.SniperAim = true;//狙击自瞄
        globalConfig.IgnoreRoBot = false;//Aim忽略人机
        globalConfig.IgnoreFall = false;//Aim忽略倒地
        globalConfig.Aimshenxian =false;//自瞄射线
        globalConfig.enemyHealthDecreased = false;//掉血自瞄
        globalConfig.AimLock = true;//持续锁定
        globalConfig.SmokeNoAim = true;//烟雾不瞄
        globalConfig.CharacterRadius = 5;//人物身位
        globalConfig.Radius = 200;//自瞄范围
        
        globalConfig.AimSpeed = 15.0f;//自瞄速
        globalConfig.AimPrediction = 1.2f;//自瞄预判
        globalConfig.CharacterAim = 350.0f;
        globalConfig.Pressure = 7.0f;//站立压枪
        globalConfig.ShotGunDistance = 30.0f;//喷子距离
        globalConfig.ShotDistance = 100.0f;//腰射距离
        globalConfig.radarOffsetX = 0.0f;
        globalConfig.radarOffsetY = 0.0f;
        globalConfig.AimingState = 0;//自瞄开关
        globalConfig.showAimCircle = true; // 默认显示自瞄圆圈
        globalConfig.Yujing = false;//被秒预警
        globalConfig.radarLineLength = 100.0f;
        globalConfig.anglemix = 100.0f;//广角
        // ... 其他默认配置项 ...
    }
}
        void ResetConfig() {
    // 重置配置项为默认值
    globalConfig.FPS = 60;//渲染帧数
    globalConfig.healthEnabled = true;//血量开关
    globalConfig.ray = true;//天线开关
    globalConfig.Bone = true;//骨骼开关
    globalConfig.weapon = true;//持枪开关
    globalConfig.Health1 = true;//生命条1
    globalConfig.Name = true;//名称开关
    globalConfig.Distance = true;//距离
    globalConfig.Radar = true;//背敌开关
    globalConfig.Grenade = true;//手雷预警
    globalConfig.Car = false;//车辆
    globalConfig.fangkuang =false;//方框
    globalConfig.leida =false;//雷达
    globalConfig.OffScreenmargin = 350.0f;//背后敌人
    globalConfig.OffScreenBox = 40;//背后敌人方框间距
    globalConfig.CharacterDis = 350.0f;//显示距离
    globalConfig.AutoAim = true; //自瞄总开关
    globalConfig.ShotGunAim = true;//喷子自瞄
    globalConfig.SniperAim = true;//狙击自瞄
    globalConfig.IgnoreRoBot = false;//Aim忽略人机
    globalConfig.IgnoreFall = false;//Aim忽略倒地
    globalConfig.Aimshenxian =false;//自瞄射线
    globalConfig.enemyHealthDecreased = false;//掉血自瞄
    globalConfig.AimLock = true;//持续锁定
    globalConfig.SmokeNoAim = true;//烟雾不瞄
    globalConfig.CharacterRadius = 5;//人物身位
    globalConfig.Radius = 200;//自瞄范围
    
    globalConfig.AimSpeed = 15.0f;//自瞄速
    globalConfig.AimPrediction = 1.2f;//自瞄预判
    globalConfig.CharacterAim = 350.0f;
    globalConfig.Pressure = 7.0f;//站立压枪
    globalConfig.ShotGunDistance = 30.0f;//喷子距离
    globalConfig.ShotDistance = 100.0f;//腰射距离
    globalConfig.radarOffsetX = 0.0f;
    globalConfig.radarOffsetY = 0.0f;
    globalConfig.AimingState = 0;//自瞄开关
    globalConfig.showAimCircle = true; // 默认显示自瞄圆圈
    globalConfig.Yujing = false;//被秒预警
    globalConfig.radarLineLength = 100.0f;
    globalConfig.anglemix = 100.0f;//广角
    
    ImGuiIO& io = ImGui::GetIO();
    JAAT = io.Fonts->AddFontFromMemoryTTF((void *)Chi_data, Chi_size, 25.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    ImGui::GetStyle().ScrollbarSize = 30.f;
}
void Menu_UI() {
    if (!悬浮窗) return;    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f + 0.5f));
    ImGui::SetNextWindowSize(ImVec2(1200, 900));
    
    ImGui::Begin("LV", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize); {
        const ImVec2 pos = ImGui::GetWindowPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // 小背景板16384
        draw->AddRectFilled(ImVec2(50 + pos.x, 10 + pos.y), ImVec2(970 + pos.x, 95 + pos.y), ImColor(0, 0, 0, 150), 25.0f, ImDrawFlags_RoundCornersAll);                     
        draw->AddText(JAAT, 50.f, ImVec2(pos.x + 100 - 35 / 2 - 60, pos.y + 10 + 35 / 2), ImColor(255, 255, 255, 255), oxorany(""));

        // 大背景板16384
        draw->AddRectFilled(ImVec2(0 + pos.x, 0 + pos.y), ImVec2(1200 + pos.x, 900 + pos.y), ImColor(18, 18, 20, 150), 25.0f, 
                            ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomRight);

        draw->AddImage(WeaponPic[98].DS, ImVec2(0 + pos.x, 0 + pos.y), ImVec2(1200 + pos.x, 900 + pos.y));
  
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 7, 3.8125 });
        if (TABG == 1){

            draw->AddText(NULL, 30, ImVec2(30 + 90 + pos.x, 25 + pos.y), ImColor(255,255,255,255), "绘制界面"); 
            ImGui::SameLine(+500.0f);   
            if (ImGui::Button1("跳转自瞄面板")) {
                TABG = 2;
            }
            ImGui::SetCursorPos( ImVec2( 50, 105 ) );
            ImGui::BeginChild("A", "BASIC ESP", ImVec2(550, 890 - 125) );{
                ImGui::Checkbox("绘制射线", &globalConfig.ray);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制骨骼", &globalConfig.Bone);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制名称", &globalConfig.Name);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制距离", &globalConfig.Distance);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制车辆", &globalConfig.Car);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("显示手持", &globalConfig.weapon);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("背敌预警", &globalConfig.Radar);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("显示方框", &globalConfig.fangkuang);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制血量", &globalConfig.healthEnabled);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("背后预警", &globalConfig.Yujing);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("近身无绘", &globalConfig.jingye);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("手雷预警", &globalConfig.Grenade);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("超体追踪手雷预警", &globalConfig.zhuizongshoulei);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::SliderFloat("绘制距离", &globalConfig.CharacterDis, 50.0f, 350.0f);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 105 ) );
                ImGui::BeginChild("B", "BASIC  ESP", ImVec2(550, 300-115) );{    
                
                if (globalConfig.healthEnabled) {
                int selectedHealth = 0;
                if (globalConfig.Health1) {
                    selectedHealth = 1;
                } else if (globalConfig.Health2) {
                    selectedHealth = 2;
                } else if (globalConfig.Health3) {
                    selectedHealth = 3;
                }
                ImGui::RadioButton("血量1", &selectedHealth, 1);
                ImGui::SameLine();
                ImGui::RadioButton("血量2", &selectedHealth, 2);
                ImGui::SameLine();
                ImGui::RadioButton("血量3", &selectedHealth, 3);

                globalConfig.Health1 = (selectedHealth == 1);
                globalConfig.Health2 = (selectedHealth == 2);
                globalConfig.Health3 = (selectedHealth == 3);
            }
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::SliderInt("渲染帧数", &globalConfig.FPS, 30, 165);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::Text("%.3f ms/帧率 %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                
                //======≠=================
                
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 305 ) );
                ImGui::BeginChild("C", "BASIC   ESP", ImVec2(550, 390 - 115) );{
            ImGui::My_SwitchButton("雷达开关", &globalConfig.leida);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::SliderFloat("雷达 X", &globalConfig.radarOffsetX, -1100.0f, 1100.0f, "%.2f");
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::SliderFloat("雷达 Y", &globalConfig.radarOffsetY, -600.0f, 600.0f, "%.2f");
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::SliderFloat("雷达大小", &globalConfig.radarLineLength, 50.0f, 500.0f, "%.0f");
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                
                //======≠=================
                
                
                ImGui::SetCursorPos(ImVec2(50 + 550 + 10, 605));
ImGui::BeginChild("D", "BASIC    ESP", ImVec2(550, 380 - 115)); {
    if (ImGui::Button1("保存配置")) {
        NumIoSave("LV内核0.12");
    }
    ImGui::ItemSize({0.f, 3.f});
    if (ImGui::Button1("退出辅助")) {
        exit(0);
    }
    ImGui::ItemSize({0.f, 3.f});
    if (ImGui::Button1("重置配置")) {
        ResetConfig();
    }    
} ImGui::EndChild();

    }      
//=====================================| 𝗧𝗔𝗕-2 |=================================//
               
                if (TABG == 2){
                draw->AddText(NULL, 30, ImVec2(30 + 90 + pos.x, 25 + pos.y), ImColor(255,255,255,255), "自瞄界面"); 
                ImGui::SameLine(+500.0f);
                if (ImGui::Button1("跳转物资面板")) {
                    TABG = 3; 
                }
                ImGui::SetCursorPos( ImVec2( 50, 105 ) );
                ImGui::BeginChild("A", "BASIC ESP", ImVec2(550, 660 - 125) );{
                ImGui::My_SwitchButton("自瞄开关", &globalConfig.AutoAim);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("忽略人机", &globalConfig.IgnoreRoBot);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("忽略倒地", &globalConfig.IgnoreFall);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("持续锁定", &globalConfig.AimLock);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("掉血自瞄", &globalConfig.enemyHealthDecreased);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("目标射线", &globalConfig.Aimshenxian);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("显示圆圈", &globalConfig.showAimCircle);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("烟雾不瞄", &globalConfig.SmokeNoAim);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("喷子自瞄", &globalConfig.ShotGunAim);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::Checkbox("狙击自瞄", &globalConfig.SniperAim);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                
                //=================≠=====
                
                ImGui::SetCursorPos( ImVec2( 50, 665 ) );
                ImGui::BeginChild("B", "BASIC  ESP", ImVec2(550, 300-105) );{    
                
                if (globalConfig.AutoAim) {
            int selectedAimingState = globalConfig.AimingState;
            ImGui::RadioButton("圆圈自瞄", &selectedAimingState, 0);
            ImGui::SameLine();
            ImGui::RadioButton("框内自瞄", &selectedAimingState, 1);
            ImGui::SameLine();
            ImGui::RadioButton("视角自瞄", &selectedAimingState, 2);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            ImGui::RadioButton("动态自瞄", &selectedAimingState, 3);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            if (selectedAimingState != globalConfig.AimingState) {
                globalConfig.AimingState = selectedAimingState;
            }

            switch (globalConfig.AimingState) {
                case 0:
                    ImGui::SliderFloat("自瞄范围", &globalConfig.Radius, 1.0f, 500.0f, "%.1f");
                    break;
                case 1:
                    ImGui::SliderFloat("框内范围", &globalConfig.CharacterRadius, 1.0f, 10.0f);
                    break;
                case 2:
                    ImGui::SliderFloat("自瞄范围", &globalConfig.Radius, 1.0f, 500.0f, "%.1f");
                    break;
                case 3:
                    ImGui::SliderFloat("自瞄范围", &globalConfig.Radius, 1.0f, 500.0f, "%.1f");
                    break;
            }
        }
                }ImGui::EndChild( );
                
                //======≠=================
                
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 305 ) );
                ImGui::BeginChild("C", "BASIC  EaP", ImVec2(550, 550 - 135) );{
              ImGui::SliderFloat("自瞄预判", &globalConfig.AimPrediction, 1.0f, 5.0f);
              ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::SliderFloat("自瞄速度", &globalConfig.AimSpeed, 1.0f, 50.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::SliderFloat("压枪部位", &globalConfig.Pressure, 1.0f, 20.0f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::SliderFloat("腰射距离", &globalConfig.ShotDistance, 1.f, 350.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::SliderFloat("喷子距离", &globalConfig.ShotGunDistance, 1.0f, 50.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        ImGui::SliderFloat("自瞄距离", &globalConfig.CharacterAim, 1.0f, 350.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
        }ImGui::EndChild( );
        
        
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 725 ) );
                ImGui::BeginChild("D", "BASIC    ESP", ImVec2(550, 270 - 135) );{
                ImGui::My_SwitchButton("超广角开关", &globalConfig.guangjiao);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
			    ImGui::SliderFloat("大小", &globalConfig.anglemix, 90.f,120.f);
			    
                }ImGui::EndChild( );
                
                
                
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 105 ) );
                ImGui::BeginChild("B", "BASIC  EAP", ImVec2(550, 290-105) );{
                if (ImGui::Button1("启用露手模式")) {
                globalConfig.loushou = true;
                globalConfig.healthEnabled = false;//血量开关
                globalConfig.ray = false;//天线开关
                globalConfig.Bone = false;//骨骼开关
                globalConfig.weapon = false;//持枪开关
                globalConfig.Xinghao = false;//生命条1
                globalConfig.Name = false;//名称开关
                globalConfig.Distance = false;//距离
                globalConfig.Radar = false;//背敌开关
                globalConfig.Grenade = false;//手雷预警
                globalConfig.Car = false;//车辆
                globalConfig.fangkuang =false;//方框
                globalConfig.leida =false;//雷达
                globalConfig.showAimCircle = false; // 默认显示自瞄圆圈
                globalConfig.Yujing = false;//被秒预警
                globalConfig.Aimshenxian = false;
                globalConfig.Box = false;
                }
                ImGui::ItemSize({0.f, 3.f});
                ImGui::Text("注意:使用露手模式后请不要开启其他绘制！！！！");
                ImGui::ItemSize({0.f, 3.f});
                if (ImGui::Button1("启用无触单透模式[重启生效]")) {
            globalConfig.loushou = false;
            globalConfig.healthEnabled = true;//血量开关
            globalConfig.ray = true;//天线开关
            globalConfig.Bone = true;//骨骼开关
            globalConfig.weapon = true;//持枪开关
            globalConfig.Xinghao = true;//生命条1
            globalConfig.Name = true;//名称开关
            globalConfig.Distance = true;//距离
            globalConfig.Radar = true;//背敌开关
            globalConfig.Grenade = true;//手雷预警
            globalConfig.Car = false;//车辆
            globalConfig.fangkuang =true;//方框
            globalConfig.leida =false;//雷达
            globalConfig.showAimCircle = false; // 默认显示自瞄圆圈
            globalConfig.Yujing = false;//被秒预警
            globalConfig.Aimshenxian = false;
            globalConfig.Box = false;
            globalConfig.AutoAim = false;
            std::ofstream outfile(filenama);
        if (outfile.is_open()) {
            outfile << "开" << std::endl;
            outfile.close();
        }     
                }ImGui::EndChild( );
                }
                }
                if (TABG == 3){
                draw->AddText(NULL, 30, ImVec2(30 + 90 + pos.x, 25 + pos.y), ImColor(255,255,255,255), "闲杂界面"); 
                ImGui::SameLine(+500.0f);     
                if (ImGui::Button1("跳转绘制面板")) {
                    TABG = 1; 
                }
                ImGui::SetCursorPos( ImVec2( 50, 105 ) );
                ImGui::BeginChild("A", "BASIC ESP", ImVec2(550, 660 - 125) );{
                ImGui::Checkbox("显示步枪", &globalConfig.buqiang);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制冲锋枪", &globalConfig.chongfeng);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制霰弹枪", &globalConfig.xiandan);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制轻机枪", &globalConfig.qingji);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制狙击枪", &globalConfig.juji);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("显示弹夹", &globalConfig.danjia);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制子弹", &globalConfig.zidan);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("其他配件", &globalConfig.qita);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制握把", &globalConfig.woba);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制倍镜", &globalConfig.beijing);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制投掷", &globalConfig.Grenade);
                }ImGui::EndChild( );
                
                //=================≠=====
                
                ImGui::SetCursorPos( ImVec2( 50, 665 ) );
                ImGui::BeginChild("B", "BASIC  ESP", ImVec2(550, 300-105) );{    
                ImGui::Checkbox("显示物资箱", &globalConfig.EscapeBox);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("显示武器箱", &globalConfig.wuqiBox);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("显示药品箱", &globalConfig.yaoBox);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                
                //======≠=================
                
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 305 ) );
                ImGui::BeginChild("C", "BASIC  EaP", ImVec2(550, 690 - 135) );{
                ImGui::Checkbox("超级物资箱", &globalConfig.chaojiBox);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制背包", &globalConfig.beibao);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制头盔", &globalConfig.toukui);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制护甲", &globalConfig.jia);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制金插", &globalConfig.jincha);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("信号枪", &globalConfig.Xinghao);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                ImGui::SetCursorPos( ImVec2( 50 + 550 + 10, 105 ) );
                ImGui::BeginChild("B", "BASIC  EAP", ImVec2(550, 290-105) );{
                ImGui::Checkbox("绘制盒子", &globalConfig.Box);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制空投", &globalConfig.kongtou);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                ImGui::Checkbox("绘制药品", &globalConfig.yaopin);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
                }ImGui::EndChild( );
                }
                ImGui::End();
                }
                ImGui::Render();
                }
                
extern int DrawItemCount;
extern struct UpItem ItemList[2000];
void Draw_UI(ImVec2 Screen, ImDrawList *imDrawList) {
    Menuscreen = Screen;
    gui_drawList = imDrawList;

    static std::unordered_set<int> processedObjects; 
    static std::vector<UpPlayer> Local_UpList;

    UpData(ImVec2(Screen.x / 2, Screen.y / 2), Local_UpList);
    int playerCount = 0, robotCount = 0;

    if (::obj_pid <= 0) {
        return;
    }

    ImVec2 startPos = ImVec2(Screen.x / 2, 20); 
    float lineSpacing = 30.0f; 
    int index = 0; 

    for (size_t i = 0; i < Local_UpList.size(); ++i) {
        const auto &Object = Local_UpList[i];

        if (Object.Distance > globalConfig.OffScreenmargin) continue;

        if (!Object.RoBot) {
            playerCount += 1;
        } else {
            robotCount += 1;
        }
        static float tm = 255 / 255.f;
        static ImVec4 arr[] = {{144 / 255.f, 238 / 255.f, 144 / 255.f, tm},
                               {135 / 255.f, 206 / 255.f, 255 / 255.f, tm},
                               {255 / 255.f, 0 / 255.f,   0 / 255.f,   tm},
                               {0 / 255.f,   255 / 255.f, 0 / 255.f,   tm},
                               {0 / 255.f,   255 / 255.f, 127 / 255.f, tm},
                               {255 / 255.f, 182 / 255.f, 193 / 255.f, tm},
                               {218 / 255.f, 112 / 255.f, 214 / 255.f, tm},
                               {248 / 255.f, 248 / 255.f, 255 / 255.f, tm},
                               {0 / 255.f,   255 / 255.f, 255 / 255.f, tm},
                               {255 / 255.f, 165 / 255.f, 0 / 255.f,   tm},
                               {153 / 255.f, 204 / 255.f, 255 / 255.f, tm},
                               {204 / 255.f, 255 / 255.f, 153 / 255.f, tm},
                               {255 / 255.f, 255 / 255.f, 153 / 255.f, tm},
                               {255 / 255.f, 153 / 255.f, 153 / 255.f, tm},
                               {153 / 255.f, 153 / 255.f, 204 / 255.f, tm},
                               {204 / 255.f, 204 / 255.f, 204 / 255.f, tm},
                               {102 / 255.f, 204 / 255.f, 153 / 255.f, tm},
                               {255 / 255.f, 102 / 255.f, 0 / 255.f,   tm},
                               {102 / 255.f, 204 / 255.f, 204 / 255.f, tm},
                               {153 / 255.f, 204 / 255.f, 255 / 255.f, tm}
        };
        static int length = sizeof(arr) / 16;

        static float tm1 = 122 / 255.f;
        static ImVec4 arr1[] = {{144 / 255.f, 238 / 255.f, 144 / 255.f, tm1},
                                {135 / 255.f, 206 / 255.f, 255 / 255.f, tm1},
                                {255 / 255.f, 0 / 255.f,   0 / 255.f,   tm1},
                                {0 / 255.f,   255 / 255.f, 0 / 255.f,   tm1},
                                {0 / 255.f,   255 / 255.f, 127 / 255.f, tm1},
                                {255 / 255.f, 182 / 255.f, 193 / 255.f, tm1},
                                {218 / 255.f, 112 / 255.f, 214 / 255.f, tm1},
                                {248 / 255.f, 248 / 255.f, 255 / 255.f, tm1},
                                {0 / 255.f,   255 / 255.f, 255 / 255.f, tm1},
                                {255 / 255.f, 165 / 255.f, 0 / 255.f,   tm1},
                                {153 / 255.f, 204 / 255.f, 255 / 255.f, tm1},
                                {204 / 255.f, 255 / 255.f, 153 / 255.f, tm1},
                                {255 / 255.f, 255 / 255.f, 153 / 255.f, tm1},
                                {255 / 255.f, 153 / 255.f, 153 / 255.f, tm1},
                                {153 / 255.f, 153 / 255.f, 204 / 255.f, tm1},
                                {204 / 255.f, 204 / 255.f, 204 / 255.f, tm1},
                                {102 / 255.f, 204 / 255.f, 153 / 255.f, tm1},
                                {255 / 255.f, 102 / 255.f, 0 / 255.f,   tm1},
                                {102 / 255.f, 204 / 255.f, 204 / 255.f, tm1},
                                {153 / 255.f, 204 / 255.f, 255 / 255.f, tm1}
        };
        static int length1 = sizeof(arr1) / 16;
        
if (globalConfig.leida) {
            float lineLength = globalConfig.radarLineLength;
            imDrawList->AddLine({ Screen.x / 2 - lineLength + globalConfig.radarOffsetX, Screen.y / 2 + globalConfig.radarOffsetY }, { Screen.x / 2 + lineLength + globalConfig.radarOffsetX, Screen.y / 2 + globalConfig.radarOffsetY }, ImColor(255, 255, 255));
            imDrawList->AddLine({ Screen.x / 2 + globalConfig.radarOffsetX, Screen.y / 2 - lineLength + globalConfig.radarOffsetY }, { Screen.x / 2 + globalConfig.radarOffsetX, Screen.y / 2 + lineLength + globalConfig.radarOffsetY }, ImColor(255, 255, 255)); 

            if (CapTivity(700, Object.Radar)) {
                float radius = 10.0f;
                ImColor targetColor = Object.RoBot ? ImColor(0, 255, 0) : ImColor(255, 0, 0);

                ImVec2 radarPos = { Screen.x / 2 + Object.Radar.x + globalConfig.radarOffsetX, Screen.y / 2 + Object.Radar.y + globalConfig.radarOffsetY };

                
                float distanceToCenter = sqrt(pow(radarPos.x - (Screen.x / 2 + globalConfig.radarOffsetX), 2) + pow(radarPos.y - (Screen.y / 2 + globalConfig.radarOffsetY), 2));

                if (distanceToCenter > lineLength) {
                    ImVec2 direction = { radarPos.x - (Screen.x / 2 + globalConfig.radarOffsetX), radarPos.y - (Screen.y / 2 + globalConfig.radarOffsetY) };
                    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

                    ImVec2 adjustedPos = { Screen.x / 2 + globalConfig.radarOffsetX + direction.x * lineLength / length, Screen.y / 2 + globalConfig.radarOffsetY + direction.y * lineLength / length };

                    
                    imDrawList->AddCircleFilled(adjustedPos, radius, targetColor);

                    std::string str = std::to_string(Object.Distance) + "M";
                    ImVec2 textPos = adjustedPos;
                    textPos.y -= 15; 
                    DrawTextY(str, textPos, ImColor(255, 255, 255), true, 15);
                } else {
                    imDrawList->AddCircleFilled(radarPos, radius, targetColor);

                    std::string str = std::to_string(Object.Distance) + "M";
                    ImVec2 textPos = radarPos;
                    textPos.y -= 15; 
                    DrawTextY(str, textPos, ImColor(255, 255, 255), true, 15);
                }
            }
        }
        if (Object.screen.x < Screen.x && Object.screen.x > 0 && Object.screen.y > 0 && Object.screen.y < Screen.y) {

            ImVec2 Distance;

            Distance.x = (Screen.x / 2 + Object.screen.x) / 2;

            if (globalConfig.ray) {
                if (!Object.RoBot) {
// 人机颜色
                    imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                          Object.screen.y - Object.size.y - 10 - 8 / 2 -
                                                                          10 - (25 / 2)),
                                        Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);
                } else {
// 真人颜色
                    imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                          Object.screen.y - Object.size.y - 10 - 8 / 2 -
                                                                          10 - (25 / 2)),
                                        Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);
                }
                Distance.y = (120 + Object.screen.y - Object.size.y - 10 - 8 / 2 - 10 - (25 / 2)) / 2;
            }//ray
            
            if (globalConfig.loushou) {
        float scale = 0.5f;
        float scaledRadius = Object.size.x * scale / 1.0f;
        ImVec2 center = ImVec2(Object.screen.x, Object.screen.y);
        float currentTime = ImGui::GetTime();
        ImU32 fillColor = Object.RoBot ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255);
        ImU32 borderColor = GetGradientColor(currentTime);
        float borderThickness = 2.0f;
        imDrawList->AddCircleFilled(center, scaledRadius, fillColor);
        DrawCircleBorder(imDrawList, center, scaledRadius, borderColor, borderThickness);
    }
            
if (globalConfig.fangkuang) {
    float scale = 2.0; 

    float scaledSizeX = Object.size.x * scale;
    float scaledSizeY = Object.size.y * scale;

    ImVec2 topLeft = ImVec2(Object.screen.x - scaledSizeX / 2, Object.screen.y - scaledSizeY / 2);
    ImVec2 bottomRight = ImVec2(Object.screen.x + scaledSizeX / 2, Object.screen.y + scaledSizeY / 2);

    float cornerWidth = 5.0f;

    ImU32 boxColor = Object.RoBot ? Colour_Bone绿 : Colour_金黄;
    float boxSize = 1.5f; 

    imDrawList->AddLine(ImVec2(topLeft.x, topLeft.y), ImVec2(topLeft.x + cornerWidth, topLeft.y), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(topLeft.x, topLeft.y), ImVec2(topLeft.x, topLeft.y + cornerWidth), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(bottomRight.x, topLeft.y), ImVec2(bottomRight.x - cornerWidth, topLeft.y), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(bottomRight.x, topLeft.y), ImVec2(bottomRight.x, topLeft.y + cornerWidth), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(topLeft.x, bottomRight.y), ImVec2(topLeft.x + cornerWidth, bottomRight.y), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(topLeft.x, bottomRight.y), ImVec2(topLeft.x, bottomRight.y - cornerWidth), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(bottomRight.x, bottomRight.y), ImVec2(bottomRight.x - cornerWidth, bottomRight.y), boxColor, boxSize);
    imDrawList->AddLine(ImVec2(bottomRight.x, bottomRight.y), ImVec2(bottomRight.x, bottomRight.y - cornerWidth), boxColor, boxSize);
}

if (globalConfig.jingye) {
int distance = Object.Distance;

std::string distanceStr = std::to_string(distance);

if (distance == 20) {
    globalConfig.healthEnabled = false;
    globalConfig.Radar = false;
    globalConfig.weapon = false;
    globalConfig.Distance = false;
    globalConfig.Name = false;
} else {
    globalConfig.healthEnabled = true;
    globalConfig.Radar = true;
    globalConfig.weapon = true;
    globalConfig.Distance = true;
    globalConfig.Name = true;
}
}
            
            if (globalConfig.Bone) {
                imDrawList->AddLine({Object.bonesData.pit.x, Object.bonesData.pit.y},
                                    {Object.bonesData.pelvis.x, Object.bonesData.pelvis.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.rcollar.x, Object.bonesData.rcollar.y},
                                    {Object.bonesData.lcollar.x, Object.bonesData.lcollar.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.lcollar.x, Object.bonesData.lcollar.y},
                                    {Object.bonesData.lelbow.x, Object.bonesData.lelbow.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.lelbow.x, Object.bonesData.lelbow.y},
                                    {Object.bonesData.lwrist.x, Object.bonesData.lwrist.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.rcollar.x, Object.bonesData.rcollar.y},
                                    {Object.bonesData.relbow.x, Object.bonesData.relbow.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.relbow.x, Object.bonesData.relbow.y},
                                    {Object.bonesData.rwrist.x, Object.bonesData.rwrist.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.pelvis.x, Object.bonesData.pelvis.y},
                                    {Object.bonesData.lthigh.x, Object.bonesData.lthigh.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.lthigh.x, Object.bonesData.lthigh.y},
                                    {Object.bonesData.lknee.x, Object.bonesData.lknee.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.lknee.x, Object.bonesData.lknee.y},
                                    {Object.bonesData.lankle.x, Object.bonesData.lankle.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.pelvis.x, Object.bonesData.pelvis.y},
                                    {Object.bonesData.rthigh.x, Object.bonesData.rthigh.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.rthigh.x, Object.bonesData.rthigh.y},
                                    {Object.bonesData.rknee.x, Object.bonesData.rknee.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);

                imDrawList->AddLine({Object.bonesData.rknee.x, Object.bonesData.rknee.y},
                                    {Object.bonesData.rankle.x, Object.bonesData.rankle.y},
                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);
            }
            
        if (globalConfig.weapon) {
        if (GetWeaponPic(Object.Weapon) == 0){
        ImVec2 pos1(Object.screen.x - WeaponPic[0].w / 6 + 25,
                     Object.screen.y - Object.size.y  - 120);
        ImVec2 pos2(Object.screen.x - WeaponPic[0].w / 6 + WeaponPic[0].w / 6 + 25,
                     Object.screen.y - Object.size.y  - 120 + WeaponPic[0].h / 6);
        if (globalConfig.Health2) {
            pos1.y += 40; 
            pos2.y += 40; 
        }
        if (globalConfig.Health3) {
            pos1.y += 45;
            pos2.y += 45; 
        }
        imDrawList->AddImage(WeaponPic[0].DS, pos1, pos2);
                }else{
        ImVec2 pos1(Object.screen.x - WeaponPic[0].w / 3 + 40,
                    Object.screen.y - Object.size.y - 120);
        ImVec2 pos2(Object.screen.x - WeaponPic[0].w / 3 + WeaponPic[0].w / 3 + 50,
                    Object.screen.y - Object.size.y - 120 + WeaponPic[0].h / 8);
        
        if (globalConfig.Health2) {
            pos1.y += 40; 
            pos2.y += 40; 
        }
        if (globalConfig.Health3) {
            pos1.y += 45;
            pos2.y += 45; 
        }
        imDrawList->AddImage(WeaponPic[GetWeaponPic(Object.Weapon)].DS, pos1, pos2);
    }
}


//血量
if (globalConfig.healthEnabled) {
if (globalConfig.Health1) {

    std::string duo = std::to_string((int)Object.HP);
    auto textSize = ImGui::CalcTextSize(duo.c_str(), nullptr, 20);  
    float baseRadius = 25.0f;
    ImVec2 center = ImVec2(Object.screen.x, Object.screen.y - Object.size.y - 50);
    float scale = sin(ImGui::GetTime() * 4) * 0.1 + 1; 
    float animatedRadius = baseRadius * scale;
    float animatedLineWidth = 5.0f * scale;
    float angleOffset = sin(ImGui::GetTime() * 5) * 5.0f; 

    float startAngle = (-90.0f + angleOffset) * (3.1415926f / 180.0f);
    float endAngle = ((Object.HP / 100.0f) * 360.0f - 90.0f + angleOffset) * (3.1415926f / 180.0f);

    ImColor barColor;
    if (Object.HP >= 100) {
        barColor = ImColor(0, 255, 0); 
    }
    else if (Object.HP < 75) {
        barColor = ImColor(0, 255, 0); 
    }
    else {
        barColor = ImColor(0, 255, 0); 
    }

    if (Object.HP < 50) {
        float blink = (sin(ImGui::GetTime() * 10) + 1) / 2.0f; 
        barColor = ImColor(255, 0, 0, (int)(blink * 255)); 
    }
    imDrawList->PathArcTo(center, animatedRadius, startAngle, endAngle, 0);
    imDrawList->PathStroke(barColor, 0, animatedLineWidth);

    imDrawList->AddText(ImVec2(Object.screen.x - textSize.x / 2, Object.screen.y - Object.size.y - 65), ImColor(255, 255, 0, 222), duo.c_str());
}
             
if (globalConfig.Health2) {
    float offsetY = 0.0f;

    float borderThickness = 3.5f; 
    float borderPadding = 1.0f;   

    ImVec2 borderTopLeft(
        Object.screen.x - 35 - borderPadding,
        Object.screen.y - Object.size.y - 14 - offsetY - borderPadding
    );
    ImVec2 borderBottomRight(
        Object.screen.x + 35 + borderPadding,
        Object.screen.y - Object.size.y - 8 + borderPadding
    );

    imDrawList->AddRect(
        borderTopLeft,
        borderBottomRight,
        ImColor(173, 216, 230, 255),
        0.0f,
        0,
        borderThickness
    );
    imDrawList->AddLine(
        ImVec2(Object.screen.x - 35, Object.screen.y - Object.size.y - 11 - offsetY),
        ImVec2(Object.screen.x - 35 + Object.HP * 0.7, Object.screen.y - Object.size.y - 11 - offsetY),
        Colour_白色, 6.f
    );

    imDrawList->AddRect(
        ImVec2(Object.screen.x - 35, Object.screen.y - Object.size.y - 14 - offsetY),
        ImVec2((Object.screen.x - 21), Object.screen.y - Object.size.y - 8 - offsetY),
        ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f
    );
    imDrawList->AddRect(
        ImVec2(Object.screen.x - 21, Object.screen.y - Object.size.y - 14 - offsetY),
        ImVec2((Object.screen.x - 7), Object.screen.y - Object.size.y - 8 - offsetY),
        ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f
    );

    imDrawList->AddRect(
        ImVec2(Object.screen.x - 7, Object.screen.y - Object.size.y - 14 - offsetY),
        ImVec2((Object.screen.x + 7), Object.screen.y - Object.size.y - 8 - offsetY),
        ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f
    );

    imDrawList->AddRect(
        ImVec2(Object.screen.x + 7, Object.screen.y - Object.size.y - 14 - offsetY),
        ImVec2((Object.screen.x + 21), Object.screen.y - Object.size.y - 8 - offsetY),
        ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f
    );

    imDrawList->AddRect(
        ImVec2(Object.screen.x + 21, Object.screen.y - Object.size.y - 14 - offsetY),
        ImVec2((Object.screen.x + 35), Object.screen.y - Object.size.y - 8 - offsetY),
        ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f
    );
        
}  
if (globalConfig.Health3) {
    imDrawList->AddLine(ImVec2(Object.screen.x - 35, Object.screen.y - Object.size.y - 11), ImVec2(Object.screen.x - 35 + Object.HP * 0.7, Object.screen.y - Object.size.y - 11), Colour_金黄, 6.f);


    imDrawList->AddRect(ImVec2(Object.screen.x - 35, Object.screen.y - Object.size.y - 14), ImVec2((Object.screen.x - 21), Object.screen.y - Object.size.y - 8), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);


    imDrawList->AddRect(ImVec2(Object.screen.x - 21, Object.screen.y - Object.size.y - 14), ImVec2((Object.screen.x - 7), Object.screen.y - Object.size.y - 8), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);

    imDrawList->AddRect(ImVec2(Object.screen.x - 7, Object.screen.y - Object.size.y - 14), ImVec2((Object.screen.x + 7), Object.screen.y - Object.size.y - 8), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);

    imDrawList->AddRect(ImVec2(Object.screen.x + 7, Object.screen.y - Object.size.y - 14), ImVec2((Object.screen.x + 21), Object.screen.y - Object.size.y - 8), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);

    imDrawList->AddRect(ImVec2(Object.screen.x + 21, Object.screen.y - Object.size.y - 14), ImVec2((Object.screen.x + 35), Object.screen.y - Object.size.y - 8), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);

}
}
if (globalConfig.Name) {
    std::string str = std::to_string(Object.Team) + ":" + Object.name;
    ImVec2 namePos(Object.screen.x - calcTextSizeY(str.c_str(), 20) / 2,
                   Object.screen.y - Object.size.y - 36);

    if (globalConfig.Health1) {
        namePos.y += 15; 
    }

    if (Object.RoBot) {
        std::string AI = "人机";
                    DrawTextY(AI, ImVec2(Object.screen.x - calcTextSizeY(AI.c_str(), 20) / 2,
                                         Object.screen.y - Object.size.y - 36), ImColor(255, 255, 255, 255), true, 20);
    } else {
        DrawTextY(str, namePos, ImColor(255, 255, 255, 255), true, 20);
    }
}


if (globalConfig.Distance) {
                std::string str1 = std::to_string(Object.Distance) + "M";
                DrawTextY(str1, ImVec2(Object.screen.x - calcTextSizeY(str1.c_str(), 20) / 2,
                                       Object.screen.y + Object.size.y + 5), ImColor(255, 255, 255, 255), true, 20);
            }


    if (Object.AimTo.x != 0 && Object.AimTo.y != 0 && globalConfig.Aimshenxian) {
    imDrawList->AddLine(ImVec2(Screen.x / 2, Screen.y / 2), ImVec2(Object.AimTo.x, Object.AimTo.y),
                        ImColor(255, 255, 255, 255), 1.5f);
    imDrawList->AddCircleFilled(ImVec2(Object.AimTo.x, Object.AimTo.y), 3, Colour_Bone绿);
}
if (globalConfig.AimingState == 2) {
       if (Object.AimTo.x != 0 && Object.AimTo.y != 0) {
        imDrawList->AddCircle(ImVec2(Object.AimTo.x, Object.AimTo.y), globalConfig.Radius, ImColor(255, 255, 255, 255), 0, 1.0f);
    }
}

float currentRadius = globalConfig.Radius;

float radiusScale = sin(ImGui::GetTime() * 4) * 0.4 + 1;

if (globalConfig.AimingState == 3) {
        currentRadius = globalConfig.Radius * radiusScale; 
        if (currentRadius < globalConfig.Radius * 0.2f) {
            currentRadius = globalConfig.Radius * 0.2f; 
        
    } else {
        currentRadius = globalConfig.Radius * radiusScale; 
    }
    imDrawList->AddCircle(ImVec2(Screen.x / 2, Screen.y / 2), currentRadius, ImColor(255, 255, 255, 255), 0, 1.0f);
}



        } else {
            ImVec2 BeiAngle = RotateCoord(Object.Angle, ImVec2(Menuscreen.x, 0));

            ImVec2 BeiScreen;
            BeiScreen = RotateCoord(Object.Angle, ImVec2(0, 0));
            BeiScreen.x += Screen.x / 2 + BeiAngle.x;
            BeiScreen.y += Screen.y / 2 + BeiAngle.y;
            FixTriangle(BeiScreen.x, BeiScreen.y, globalConfig.OffScreenBox);

if (globalConfig.Radar) {
    float halfWidth = 35.0f;
    float halfHeight = 20.0f;
    
    ImVec2 topLeft = ImVec2(BeiScreen.x - halfWidth, BeiScreen.y - halfHeight);
    ImVec2 bottomRight = ImVec2(BeiScreen.x + halfWidth, BeiScreen.y + halfHeight);

    if (!Object.RoBot) {
        imDrawList->AddRectFilled(topLeft, bottomRight, Colour_金黄);
    } else {
        imDrawList->AddRectFilled(topLeft, bottomRight, Colour_Bone绿);
    }

    std::string str = std::to_string(Object.Distance) + "米";
    float textWidth = calcTextSizeY(str.c_str(), 30);
    ImVec2 textPos = ImVec2(BeiScreen.x - textWidth / 2, BeiScreen.y - 15);
    DrawTextY(str, textPos, ImColor(255, 255, 255), true, 30);
}
if (globalConfig.Yujing) { // 背后玩家
    std::string infoStr = Object.name + " 在你背后 " + std::to_string((int)Object.Distance) + " 米，使用 " + GetHoldaGunItem(Object.Weapon) + "，还剩 " + std::to_string((int)Object.HP) + "% 血";
    ImVec2 infoPos = ImVec2(Screen.x / 2 - calcTextSizeY(infoStr.c_str(), 20) / 2, startPos.y + index * lineSpacing);
    DrawTextY(infoStr, infoPos, ImColor(255, 255, 255, 255), true, 20);
    index++;
}
}
        if (!globalConfig.OffScreen) {

            if (Object.screen.x < Screen.x && Object.screen.x > 0 && Object.screen.y > 0 &&
                Object.screen.y < Screen.y) {

            } else {

                if (globalConfig.ray) {
                    if (Object.weaponName == "\ue931" || Object.weaponName == "\ue93e" ||
                        Object.weaponName == "\ue941" || Object.weaponName == "\ue929" ||
                        Object.weaponName == "\ue950" || Object.weaponName == "\ue939" ||
                        Object.weaponName == "\ue93c" || Object.weaponName == "\ue944" ||
                        Object.weaponName == "\ue936" || Object.weaponName == "\ue94c" ||
                        Object.weaponName == "\ue938" || Object.weaponName == "\ue926" ||
                        Object.weaponName == "\ue925" || Object.weaponName == "\ue947" ||
                        Object.weaponName == "\ue956" || Object.weaponName == "\ue957" ||
                        Object.weaponName == "\ue958" || Object.weaponName == "\ue959" ||
                        Object.weaponName == "\ue92c" || Object.weaponName == "\ue928" ||
                        Object.weaponName == "\ue95d" || Object.weaponName == "\ue94e" ||
                        Object.weaponName == "\ue930" || Object.weaponName == "\ue94f" ||
                        Object.weaponName == "\ue934" || Object.weaponName == "\ue946" ||
                        Object.weaponName == "\ue943" || Object.weaponName == "\ue927" ||
                        Object.weaponName == "\ue93a" || Object.weaponName == "\ue93d" ||
                        Object.weaponName == "\ue942" || Object.weaponName == "\ue952" ||
                        Object.weaponName == "\ue94b") {
 
if (globalConfig.weapon) {
//判断是否打开了人物持枪
                            if (!Object.RoBot) {
// 人机颜色
                                imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                      Object.screen.y - Object.size.y -
                                                                                      45 - (25 / 2) - 10),
                                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);
                            } else {
// 真人颜色
                                imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                      Object.screen.y - Object.size.y -
                                                                                      45 - (25 / 2) - 10),
                                                    Object.RoBot ? Colour_Bone绿 : Colour_金黄, 1.5f);
                            }
                        } else {
//没打开人物持枪
                            if (!Object.RoBot) {
// 人机颜色
                                imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                      Object.screen.y - Object.size.y -
                                                                                      10 - 8 / 2 - 10 - (25 / 2)),
                                                    Object.RoBot ? Colour_Bone绿 : Colour_Bone深红色, 1.0f);
                            } else {
// 真人颜色
                                imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                      Object.screen.y - Object.size.y -
                                                                                      10 - 8 / 2 - 10 - (25 / 2)),
                                                    Object.RoBot ? Colour_Bone绿 : Colour_Bone深红色, 1.0f);
                            }
                        }

                    } else {

                        if (!Object.RoBot) {
                            // 人机颜色
                            imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                  Object.screen.y - Object.size.y - 10 -
                                                                                  8 / 2 - 10 - (25 / 2)),
                                                Object.RoBot ? Colour_Bone绿 : Colour_Bone深红色, 1.0f);
                        } else {
                            // 真人颜色
                            imDrawList->AddLine(ImVec2(Screen.x / 2, 30), ImVec2(Object.screen.x,
                                                                                  Object.screen.y - Object.size.y - 10 -
                                                                                  8 / 2 - 10 - (25 / 2)),
                                                Object.RoBot ? Colour_Bone绿 : Colour_Bone深红色, 1.0f);
                        }
                    }
                }//ray

            }//不在屏幕内

        }//关掉背敌人开关

    }//for
    
    
std::vector<UpItem> visibleItems;
for (int i = 0; i < DrawItemCount; i++) {
    if (ItemList[i].screen.x < Screen.x && ItemList[i].screen.x > 0 && ItemList[i].screen.y > 0 && ItemList[i].screen.y < Screen.y) {
        if (ItemList[i].Distance < 1000) {
            visibleItems.push_back(ItemList[i]);
        }
    }
}

for (const auto &item : visibleItems) {
    if (globalConfig.Grenade) {
        if (item.Distance < 50) {
            std::string str;
            if (item.Name.find("ProjGrenade_") != std::string::npos) {
                str = "小心手雷";
            } else if (item.Name.find("AttachActor_Ring_BP_C") != std::string::npos) {
                str = "正在捏雷";
            } else if (item.Name.find("ProjSmoke_") != std::string::npos) {
                str = "烟雾弹";
            } else if (item.Name.find("ProjBurn_") != std::string::npos) {
                str = "燃烧瓶";
            } else if (item.Name.find("ProjThermite_") != std::string::npos) {
                str = "铝热弹";
            } else if (item.Name.find("ProjStun_") != std::string::npos) {
                str = "震爆弹";
            } else if (item.Name.find("BP_Grenade_Burn_Sallete_C") != std::string::npos) {
                str = "燃烧范围";
            }

            if (!str.empty()) {
                str += " [";
                str += std::to_string((int)item.Distance);
                str += "M]";
                ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
                DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
            }
        }
    }
if (globalConfig.EscapeBox) {
    if (item.Distance < 120) { 
        std::string str;
        if (item.Name.find("EscapeBoxHight_SupplyBox_Lv1_C")!= std::string::npos) {
            str = "物资箱";
        } else if (item.Name.find("EscapeBoxHight_SupplyBox_Lv2_C")!= std::string::npos) {
            str = "物资箱";
        } else if (item.Name.find("EscapeBoxHight_SupplyBox_Lv3_C")!= std::string::npos) {
            str = "物资箱";
        } else if (item.Name.find("EscapeBoxHight_SupplyBox_Lv4_C")!= std::string::npos) {
            str = "物资箱";
        } else if (item.Name.find("EscapeBoxHight_SupplyBox_Lv5_C")!= std::string::npos) {
            str = "物资箱";
        } else if (item.Name.find("BP_InnerSupplyBoxBase_C")!= std::string::npos) {
            str = "已开启";
        } else if (item.Name.find("BP_EscapeInnerWrapperList_C")!= std::string::npos) {
            str = "已开启";
        }
        
        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
    }
}   
    
    if (globalConfig.wuqiBox) {
    if (item.Distance < 120) { 
        std::string str;
        if (item.Name.find("BP_InnerSupplyBoxBase_C") != std::string::npos ||
           item.Name.find("BP_EscapeInnerWrapperList_C") != std::string::npos) {
            
        } else if (item.Name.find("EscapeBoxHight_Weapon_Lv1_C") != std::string::npos ||
                   item.Name.find("EscapeBoxHight_Weapon_Lv2_C") != std::string::npos ||
                   item.Name.find("EscapeBoxHight_Weapon_Lv3_C") != std::string::npos ||
                   item.Name.find("EscapeBoxHight_Weapon_Lv4_C") != std::string::npos ||
                   item.Name.find("EscapeBoxHight_Weapon_Lv5_C") != std::string::npos) {
            str = "武器箱";
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
        // 其他情况不进行任何操作
    }
}
    if (globalConfig.yaoBox) {
    if (item.Distance < 80) { 
        std::string str;
        if (item.Name.find("BP_InnerSupplyBoxBase_C") != std::string::npos ||
           item.Name.find("BP_EscapeInnerWrapperList_C") != std::string::npos) {
            
        } else if (item.Name.find("EscapeBoxHight_Medical_Lv3_C") != std::string::npos ||
                   item.Name.find("EscapeBoxHight_Medical_Lv2_C") != std::string::npos) {
            str = "药品箱";
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
        // 其他情况不进行任何操作
    }
}
    if (globalConfig.chaojiBox) {
        std::string str;
        if (item.Name.find("EscapeBox_SpeEffect_C") != std::string::npos) {
        
        } else if (item.Name.find("MilitarySupplyBoxBase_Baltic_Classic_C") != std::string::npos) {
            str = "超级物资箱";
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
        // 其他情况不进行任何操作
    }


    if (globalConfig.Xinghao) {
        std::string str;
        if (item.Name.find("Flaregun") != std::string::npos) {
            str = "信号枪";
        } else if (item.Name.find("Flare") != std::string::npos) {
            str = "掩体墙";
        } else if (item.Name.find("BP_Pistol_Flaregun_Wrapper_C") != std::string::npos) {
            str = "信号枪";
        } else if (item.Name.find("RevivalCard_PickUp_C") != std::string::npos) {
            str = "通用召回信标";
        } else if (item.Name.find("BP_Ammo_Flare_Pickup_C") != std::string::npos) {
            str = "信号弹";
        } else if (item.Name.find("_revivalAED_Pickup_C") != std::string::npos) {
            str = "自救器";
        }else if (item.Name.find("BP_Pickup_Finger_C") != std::string::npos) {
            str = "飞索";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.jincha) {
        std::string str;
        if (item.Name.find("PeopleSkill") != std::string::npos) {
            str = "金色插件";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.kongtou) {
        std::string str;
        if (item.Name.find("_AirDropBox_C") != std::string::npos) {
            str = "空投";
        } else if (item.Name.find("_AirDropBox_SuperPeople_C") != std::string::npos) {
            str = "空投";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.beibao) {
        std::string str;
        if (item.Name.find("Bag_Lv3") != std::string::npos) {
            str = "三级包";
        } else if (item.Name.find("ckUp_BP_Bag_Lv3_C") != std::string::npos) {
            str = "三级包";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.toukui) {
        std::string str;
        if (item.Name.find("Helmet_Lv3") != std::string::npos) {
            str = "三级头";
        } else if (item.Name.find("PickUp_BP_Helmet_Lv3_C") != std::string::npos) {
            str = "三级头";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.jia) {
        std::string str;
        if (item.Name.find("Armor_Lv3") != std::string::npos) {
            str = "三级甲";
        } else if (item.Name.find("PickUp_BP_Armor_Lv3_C") != std::string::npos) {
            str = "三级甲";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    
    if (globalConfig.buqiang) {
        std::string str;
        if (item.Name.find("BP_Rifle_AKM_Wrapper_C") != std::string::npos) {
            str = "AKM";
        } else if (item.Name.find("BP_Rifle_M416_Wrapper_C") != std::string::npos) {
            str = "M416";
        } else if (item.Name.find("BP_Pistol_Flaregun_Wrapper_C") != std::string::npos) {
            str = "SCAR-L";
        } else if (item.Name.find("BP_Rifle_M762_Wrapper_C") != std::string::npos) {
            str = "M762";
        } else if (item.Name.find("BP_Rifle_Groza_Wrapper_C") != std::string::npos) {
            str = "GROZA";
        } else if (item.Name.find("BP_Rifle_AUG_Wrapper_C") != std::string::npos) {
            str = "AUG";
        } else if (item.Name.find("BP_Rifle_Famas_Wrapper_C") != std::string::npos) {
            str = "法玛斯";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.qingji) {
        std::string str;
        if (item.Name.find("BP_Other_MG3_Wrapper_C") != std::string::npos) {
            str = "MG3";
        } else if (item.Name.find("BP_Other_M249_Wrapper_C") != std::string::npos) {
            str = "M249";
        } else if (item.Name.find("BP_Other_DP28_Wrapper_C") != std::string::npos) {
            str = "DP28";
        } else if (item.Name.find("BP_Other_PKM_Wrapper_C") != std::string::npos) {
            str = "PKM";
        } else if (item.Name.find("BP_Other_MG36_Wrapper_C") != std::string::npos) {
            str = "MG36";
        } else if (item.Name.find("BP_Other_HuntingBow_Wrapper_C") != std::string::npos) {
            str = "爆炸猎弓";
        } else if (item.Name.find("BP_Other_CrossbowBorderland_Wrapper_C") != std::string::npos) {
            str = "战术弩";
        } else if (item.Name.find("BP_Other_Shield_Wrapper_C") != std::string::npos) {
            str = "盾牌";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.juji) {
        std::string str;
        if (item.Name.find("BP_Sniper_M200_Wrapper_C") != std::string::npos) {
            str = "M200";
        } else if (item.Name.find("BP_Sniper_Win94_Wrapper_C") != std::string::npos) {
            str = "Win94";
        } else if (item.Name.find("BP_Sniper_Kar98k_Wrapper_C") != std::string::npos) {
            str = "Kar98k";
        } else if (item.Name.find("BP_Sniper_M24_Wrapper_C") != std::string::npos) {
            str = "M24";
        } else if (item.Name.find("BP_Sniper_AMR_Wrapper_C") != std::string::npos) {
            str = "AWM";
        } else if (item.Name.find("BP_Sniper_Mosin_Wrapper_C") != std::string::npos) {
            str = "莫辛甘纳";
        } else if (item.Name.find("BP_Sniper_AMR_Wrapper_C") != std::string::npos) {
            str = "AMR";
        } else if (item.Name.find("BP_Sniper_SVD_Wrapper_C") != std::string::npos) {
            str = "SVD";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.zidan) {
        std::string str;
        if (item.Name.find("BP_Ammo_556mm_Pickup_C") != std::string::npos) {
            str = "5.56弹药";
        } else if (item.Name.find("BP_Ammo_762mm_Pickup_C") != std::string::npos) {
            str = "7.62弹药";
        } else if (item.Name.find("BP_Ammo_12Guage_Pickup_C") != std::string::npos) {
            str = "12口径霰弹";
        } else if (item.Name.find("BP_Ammo_45ACP_Pickup_C") != std::string::npos) {
            str = "45毫米子弹";
        } else if (item.Name.find("BP_Ammo_9mm_Pickup_C") != std::string::npos) {
            str = "9毫米子弹";
        } else if (item.Name.find("Ammo_300Magnum") != std::string::npos) {
            str = "300马格南";
        } else if (item.Name.find("Ammo_Bolt") != std::string::npos) {
            str = "箭矢";
        } else if (item.Name.find("Ammo_50BMG") != std::string::npos) {
            str = ".50口径弹药";
        } else if (item.Name.find("Ammo_57mm") != std::string::npos) {
            str = "57毫米子弹";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.chongfeng) {
        std::string str;
        if (item.Name.find("BP_MachineGun_UMP9_Wrapper_C") != std::string::npos) {
            str = "UMP45";
        } else if (item.Name.find("BP_MachineGun_TommyGun_Wrapper_C") != std::string::npos) {
            str = "汤姆逊";
        } else if (item.Name.find("BP_MachineGun_Uzi_Wrapper_C") != std::string::npos) {
            str = "UZI";
        } else if (item.Name.find("BP_MachineGun_Vector_Wrapper_C") != std::string::npos) {
            str = "Vector";
        } else if (item.Name.find("BP_MachineGun_MP5_Wrapper_C") != std::string::npos) {
            str = "MP5K";
        } else if (item.Name.find("BP_MachineGun_P90CG17_Wrapper_C") != std::string::npos) {
            str = "P90";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.beijing) {
        std::string str;
        if (item.Name.find("BP_MZJ_HD_Pickup_C") != std::string::npos) {
            str = "红点";
        } else if (item.Name.find("BP_MZJ_3X_Pickup_C") != std::string::npos) {
            str = "三倍";
        } else if (item.Name.find("BP_MZJ_6X_Pickup_C") != std::string::npos) {
            str = "六倍";
        } else if (item.Name.find("BP_MZJ_4X_Pickup_C") != std::string::npos) {
            str = "四倍";
        } else if (item.Name.find("BP_MZJ_2X_Pickup_C") != std::string::npos) {
            str = "二倍";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.danjia) {
        std::string str;
        if (item.Name.find("BP_DJ_Large_EQ_Pickup_C") != std::string::npos) {
            str = "步枪快扩";
        } else if (item.Name.find("BP_DJ_Large_E_Pickup_C") != std::string::npos) {
            str = "步枪扩容";
        } else if (item.Name.find("BP_DJ_Mid_EQ_Pickup_C") != std::string::npos) {
            str = "冲锋枪快扩";
        } else if (item.Name.find("BP_DJ_Mid_E_Pickup_C") != std::string::npos) {
            str = "冲锋枪扩容";
        } else if (item.Name.find("BP_DJ_Sniper_EQ_Pickup_C") != std::string::npos) {
            str = "狙击枪快扩";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.qita) {
        std::string str;
        if (item.Name.find("BP_QT_A_Pickup_C") != std::string::npos) {
            str = "步枪枪托";
        } else if (item.Name.find("BP_QT_Sniper_Pickup_C") != std::string::npos) {
            str = "托腮板";
        } else if (item.Name.find("BP_QK_Large_Compensator_Pickup_C") != std::string::npos) {
            str = "步枪补偿";
        } else if (item.Name.find("BP_QK_Sniper_Compensator_Pickup_C") != std::string::npos) {
            str = "狙击补偿";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.woba) {
        std::string str;
        if (item.Name.find("BP_WB_LightGmp_Pickup_C") != std::string::npos) {
            str = "轻型握把";
        } else if (item.Name.find("BP_WB_Lasersight_Pickup_C") != std::string::npos) {
            str = "激光握把";
        } else if (item.Name.find("BP_WB_ThumbGrip_Pickup_C") != std::string::npos) {
            str = "拇指握把";
        } else if (item.Name.find("BP_WB_Angled_Pickup_C") != std::string::npos) {
            str = "直角握把";
        } else if (item.Name.find("BP_WB_Vertical_Pickup_C") != std::string::npos) {
            str = "垂直握把";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    if (globalConfig.xiandan) {
        std::string str;
        if (item.Name.find("BP_ShotGun_DP12_Wrapper_C") != std::string::npos) {
            str = "DBS";
        } else if (item.Name.find("BP_ShotGun_S12K_Wrapper_C") != std::string::npos) {
            str = "S12K";
        } else if (item.Name.find("BP_ShotGun_SPAS-12_Wrapper_C") != std::string::npos) {
            str = "SPAS-12";
        } else if (item.Name.find("BP_ShotGun_S686_Wrapper_C") != std::string::npos) {
            str = "S686";
        } else if (item.Name.find("BP_ShotGun_S1897_Wrapper_C") != std::string::npos) {
            str = "S1897";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }
    
    if (globalConfig.zhuizongshoulei) {
    if (item.Distance < 80) { 
        std::string str;
        if (item.Name.find("BP_TraceGrenadeActor_C") != std::string::npos) {
            str = "追踪手雷在他手里！";
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        } else if (item.Name.find("BP_TraceGrenade_Projectile_C") != std::string::npos) {
            str = "追踪手雷来啦！";
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
        // 其他情况不进行任何操作
    }
}
    
    if (globalConfig.yaopin) {
        std::string str;
        if (item.Name.find("Pills_Pickup_C") != std::string::npos) {
            str = "止痛药";
        } else if (item.Name.find("Drink_Pickup_C") != std::string::npos) {
            str = "能量饮料";
        } else if (item.Name.find("Firstaid_Pickup_C") != std::string::npos) {
            str = "急救包";
        } else if (item.Name.find("Injection_Pickup_C") != std::string::npos) {
            str = "肾上腺素";
        } else if (item.Name.find("FirstAidbox_Pickup_C") != std::string::npos) {
            str = "医疗箱";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(0, 255, 0, 222), true, 20);
        }
    }

    if (globalConfig.Box) {
        if (item.Distance < 150) {  // 只有当距离小于100米时，才处理手雷相关的提示
            std::string str;
        if (item.Name.find("BP_PlayerDeadListWrapper_C") != std::string::npos) {
            str = "盒子";
        } else if (item.Name.find("CharacterDeadInventoryBox_") != std::string::npos) {
            str = "盒子";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 0, 0, 222), true, 20);
        }
    }
}
    if (globalConfig.Car) {
        if (item.Distance < 500) {  // 只有当距离小于100米时，才处理手雷相关的提示
            std::string str;
        if (item.Name.find("VH_LostMobile_C") != std::string::npos) {
            str = "主题车";
        } else if (item.Name.find("_StationWagon_C") != std::string::npos) {
            str = "旅行车";
        } else if (item.Name.find("TrackVehicle_BP_C") != std::string::npos) {
            str = "矿车";
        } else if (item.Name.find("Buggy") != std::string::npos) {
            str = "蹦蹦";
        } else if (item.Name.find("CoupeRB") != std::string::npos) {
            str = "跑车";
        } else if (item.Name.find("PlaneCharacter") != std::string::npos) {
            str = "飞机";
        } else if (item.Name.find("UAZ") != std::string::npos) {
            str = "吉普";
        } else if (item.Name.find("LadaNiva") != std::string::npos) {
            str = "吉普";
        } else if (item.Name.find("Motorglider") != std::string::npos) {
            str = "滑翔机";
        } else if (item.Name.find("MotorcycleCart") != std::string::npos) {
            str = "摩托车";
        } else if (item.Name.find("Motorcycle") != std::string::npos) {
            str = "摩托车";
        } else if (item.Name.find("Dacia") != std::string::npos) {
            str = "轿车";
        } else if (item.Name.find("AquaRail") != std::string::npos) {
            str = "冲锋艇";
        } else if (item.Name.find("PG117") != std::string::npos) {
            str = "轮船";
        } else if (item.Name.find("MiniBus") != std::string::npos) {
            str = "大巴车";
        } else if (item.Name.find("Mirado") != std::string::npos) {
            str = "跑车";
        } else if (item.Name.find("Scooter") != std::string::npos) {
            str = "小绵羊";
        } else if (item.Name.find("Rony") != std::string::npos) {
            str = "皮卡车";
        } else if (item.Name.find("Snowbike") != std::string::npos) {
            str = "雪地车";
        } else if (item.Name.find("Snowmobile") != std::string::npos) {
            str = "雪地车";
        } else if (item.Name.find("Tuk") != std::string::npos) {
            str = "三轮";
        } else if (item.Name.find("StationWagon")!= std::string::npos) {
            str = "行李车";
        } else if (item.Name.find("BRDM")!= std::string::npos) {
            str = "装甲车";
        } else if (item.Name.find("Bigfoot")!= std::string::npos) {
            str = "大脚车";
            } else if (item.Name.find("UTV")!= std::string::npos) {
            str = "拉力车";
        } else if (item.Name.find("ATV")!= std::string::npos) {
            str = "沙滩车";
        } else if (item.Name.find("Horse")!= std::string::npos) {
            str = "马";
        } else if (item.Name.find("4SportCar")!= std::string::npos) {
            str = "跑车";
        }

        if (!str.empty()) {
            str += " [";
            str += std::to_string((int)item.Distance);
            str += "M]";
            ImVec2 textPos = ImVec2(item.screen.x, item.screen.y);
            DrawTextY(str, textPos, ImColor(255, 255, 0, 222), true, 20);
        }
    }
}
}
int totalCount = playerCount + robotCount;
std::string strTotal = std::to_string(totalCount);

float textWidthTotal = calcTextSizeY(strTotal.c_str(), 80 * 0.75f); 
float textHeightTotal = 80 * 0.75f; // 
float squareSizeTotal = std::max(textWidthTotal, textHeightTotal) + 40 * 0.75f; 

float offsetXTotal = 0.0f;
float offsetYTotal = 25.0f * 0.75f; 
offsetYTotal += 30.0f; 

ImVec2 topLeftTotal((Screen.x / 2) - squareSizeTotal / 2, 85 - squareSizeTotal / 2 + offsetYTotal);
ImVec2 bottomRightTotal((Screen.x / 2) + squareSizeTotal / 2, 85 + squareSizeTotal / 2 + offsetYTotal);

ImVec2 textPosTotal((Screen.x / 2) - textWidthTotal / 2, 85 - textHeightTotal / 2 + offsetYTotal);
ImColor colorStart(255, 0, 0, 255);
ImColor colorEnd(0, 255, 0, 255);

int cornerExtension = 5 * 0.75f; 

for (int i = 0; i < 4; ++i) {
    float gradient = static_cast<float>(i) / 3.0f;
    ImColor strokeColor(
        static_cast<int>(colorStart.Value.x * (1.0f - gradient) + colorEnd.Value.x * gradient),
        static_cast<int>(colorStart.Value.y * (1.0f - gradient) + colorEnd.Value.y * gradient),
        static_cast<int>(colorStart.Value.z * (1.0f - gradient) + colorEnd.Value.z * gradient),
        255
    );

    gui_drawList->AddLine(
        ImVec2(topLeftTotal.x + i * 0.75f, topLeftTotal.y + i * 0.75f),
        ImVec2(topLeftTotal.x + i + 10 * 0.75f + cornerExtension, topLeftTotal.y + i * 0.75f),
        strokeColor, 2.0f * 0.75f
    );
    gui_drawList->AddLine(
        ImVec2(topLeftTotal.x + i * 0.75f, topLeftTotal.y + i * 0.75f),
        ImVec2(topLeftTotal.x + i * 0.75f, topLeftTotal.y + i + 10 * 0.75f + cornerExtension),
        strokeColor, 2.0f * 0.75f
    );

    gui_drawList->AddLine(
        ImVec2(bottomRightTotal.x - i * 0.75f - 10 * 0.75f - cornerExtension, topLeftTotal.y + i * 0.75f),
        ImVec2(bottomRightTotal.x - i * 0.75f, topLeftTotal.y + i * 0.75f),
        strokeColor, 2.0f * 0.75f
    );
    gui_drawList->AddLine(
        ImVec2(bottomRightTotal.x - i * 0.75f, topLeftTotal.y + i * 0.75f),
        ImVec2(bottomRightTotal.x - i * 0.75f, topLeftTotal.y + i + 10 * 0.75f + cornerExtension),
        strokeColor, 2.0f * 0.75f
    );

    gui_drawList->AddLine(
        ImVec2(topLeftTotal.x + i * 0.75f, bottomRightTotal.y - i * 0.75f - 10 * 0.75f - cornerExtension),
        ImVec2(topLeftTotal.x + i * 0.75f, bottomRightTotal.y - i * 0.75f),
        strokeColor, 2.0f * 0.75f
    );
    gui_drawList->AddLine(
        ImVec2(topLeftTotal.x + i * 0.75f, bottomRightTotal.y - i * 0.75f),
        ImVec2(topLeftTotal.x + i + 10 * 0.75f + cornerExtension, bottomRightTotal.y - i * 0.75f),
        strokeColor, 2.0f * 0.75f
    );
    gui_drawList->AddLine(
        ImVec2(bottomRightTotal.x - i * 0.75f - 10 * 0.75f - cornerExtension, bottomRightTotal.y - i * 0.75f),
        ImVec2(bottomRightTotal.x - i * 0.75f, bottomRightTotal.y - i * 0.75f),
        strokeColor, 2.0f * 0.75f
    );
    gui_drawList->AddLine(
        ImVec2(bottomRightTotal.x - i * 0.75f, bottomRightTotal.y - i * 0.75f),
        ImVec2(bottomRightTotal.x - i * 0.75f, bottomRightTotal.y - i - 10 * 0.75f - cornerExtension),
        strokeColor, 2.0f * 0.75f
    );
}

DrawTextY(strTotal, textPosTotal, ImColor(255, 255, 255), true, 80 * 0.75f);

DrawTextY(strTotal, textPosTotal, ImColor(255, 255, 255), true, 80 * 0.75f);
std::string strLouisVuitton = "LOUIS VUITTON";
float textWidthLouis = calcTextSizeY(strLouisVuitton.c_str(), 40 * 0.75f);
float textHeightLouis = 40 * 0.75f;
ImVec2 textPosLouis((Screen.x / 2) - textWidthLouis / 2, 85 + squareSizeTotal / 2 + offsetYTotal + 20 * 0.75f + textHeightLouis / 2);
ImColor colorStartLouis(255, 0, 0, 255);
ImColor colorEndLouis(0, 255, 0, 255);
auto currentTime = std::chrono::system_clock::now().time_since_epoch();
auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();

float gradientFactor = static_cast<float>((milliseconds / 100) % 1000) / 1000.0f;
ImColor currentColorLouis(
    static_cast<int>(colorStartLouis.Value.x * (1.0f - gradientFactor) + colorEndLouis.Value.x * gradientFactor),
    static_cast<int>(colorStartLouis.Value.y * (1.0f - gradientFactor) + colorEndLouis.Value.y * gradientFactor),
    static_cast<int>(colorStartLouis.Value.z * (1.0f - gradientFactor) + colorEndLouis.Value.z * gradientFactor),
    255
);

DrawTextY(strLouisVuitton, textPosLouis, currentColorLouis, true, 40 * 0.75f);
//自瞄圆圈
   if (globalConfig.AimingState == 0 && globalConfig.showAimCircle) {
    imDrawList->AddCircle(ImVec2(Screen.x / 2, Screen.y / 2), globalConfig.Radius, ImColor(255, 255, 255, 255), 0, 1.0f);
}
}