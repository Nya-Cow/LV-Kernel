//
// Created by Administrator on 2024/1/6 0006.
//

#ifndef ANDROIDSURFACEIMGUI_DRAW_ESP_H
#define ANDROIDSURFACEIMGUI_DRAW_ESP_H

#include "base.h"
#include "MyCustom/imgui_Custom.h"
#include "My_wanbai.hpp"
#define Colour_金黄 0xFFFFFF66
#define Colour_白色 0xFFFFFFFF
#define Colour_Bone绿 0xFF96FF96
#define Colour_Bone深红色 0xFF0000FF

// My_wanbai.hpp 或其他头文件
extern pid_t obj_pid;
extern Config globalConfig;
extern TextureInfo WeaponPic[100];
extern c_driver *driver;
extern ImDrawList* Draws; // 假设你在某个头文件中已经声明了 Draws
extern int getPID(const char *PackageName);
extern bool PidExamIne(pid_t pid_);
extern void Draw_UI(ImVec2 Screen,ImDrawList *imDrawList);//绘图加载
extern void NumIoLoad(const char *name);   //加载记忆cfg
extern void Menu_UI();//菜单加载
extern void LoadPic(); //加载图片
#endif //ANDROIDSURFACEIMGUI_DRAW_ESP_H


