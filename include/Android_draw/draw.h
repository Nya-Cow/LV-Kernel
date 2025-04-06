#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H

#include <stdio.h>
#include <stdlib.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "native_surface/ANativeWindowCreator.h"

#include "AndroidImgui.h"
#include "TouchHelperA.h"//触摸
#include "my_imgui.h"     //字体
#include "MyCustom/imgui_Custom.h"

#include <cstdlib>
#include <dlfcn.h>
#include "timer.h"
#include <Log.h>




extern ANativeWindow *window;// 屏幕信息
extern android::ANativeWindowCreator::DisplayInfo displayInfo;// 窗口信息
extern ImGuiWindow *g_window;// 绝对屏幕X _ Y
extern int abs_ScreenX, abs_ScreenY;
extern int native_window_screen_x, native_window_screen_y;

extern std::unique_ptr<AndroidImgui>  graphics;

extern ImFont* zh_font;
extern ImFont* icon_font_0;
extern ImFont* icon_font_1;
extern ImFont* icon_font_2;


//是否过录制
extern bool permeate_record;

extern timer FPS限制;
extern float FPF显示;

// 获取屏幕信息
extern void screen_config();

extern void drawBegin();

extern void init_My_drawdata();


extern void DrawInit();




#endif //NATIVESURFACE_DRAW_H
