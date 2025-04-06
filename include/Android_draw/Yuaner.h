//
// Created by Administrator on 2024/1/6 0006.
//

#ifndef ANDROIDSURFACEIMGUI_YUANER_H
#define ANDROIDSURFACEIMGUI_YUANER_H

#include "base.h"

#include <unordered_set>






typedef unsigned short UTF16;

typedef char UTF8;

// 互斥锁
extern pthread_mutex_t lock_readall;



//主线程
void *main_thread(void*);

//数据线程
void UpData(ImVec2 Screen,std::vector<UpPlayer> &UpList);

//自瞄线程
void *AimBot(void *);


//人物名字
char *getPlayerName(uintptr_t addr);

/*是否可见*/
bool isCoordVisibility(ImVec3 coord);

/*是否有效*/
bool isVaildPtr(uintptr_t ptr);

//获取骨骼坐标
ImVec3 GetBone(uintptr_t human, uintptr_t bones, int part);

/*骨骼转屏幕*/
bool GetBoneCameraCache(MinimalViewInfo pov, uintptr_t human, uintptr_t bones, int part ,ImVec2 &buf);

/*屏幕坐标计算人物高度宽度*/
ImVec2 Screen(ImVec3 objectCoord,MinimalViewInfo pov);

//Name 读类名
std::string getName(uintptr_t object);

ImVec2 GetRadar(ImVec3 Pos,ImVec3 CameraPos,float Yaw);

bool CapTivity(float Range, ImVec2 Pos);

char *GetHoldaGunItem(int WeaponId);

int GetWeaponPic(int WeaponID);

#endif //ANDROIDSURFACEIMGUI_YUANER_H
