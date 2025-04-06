//
//  CustomStrct.h
//  Dolphins
//
//  Created by 天溪 on 2022/4/24.
//

#ifndef CustomStrct_h
#define CustomStrct_h

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <fstream>
#include <__threading_support>
#include <unistd.h>
#include <linux/mman.h>
#include <cstdlib>
#include <codecvt>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <android/native_window.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "AndroidImgui.h"

#include "timer.h"
#include <Log.h>

/*ImVec4*/
struct Quat {
    float x;
    float y;
    float z;
    float w;
};

struct ImVec3
{
    float x, y, z;
    ImVec3() { 
        x = y = z = 0.0f; 
    }

    ImVec3(float _x, float _y, float _z)  { x = _x; y = _y; z = _z; }

    ImVec3 operator+(const ImVec3 &v) const {
        return ImVec3(x + v.x, y + v.y, z + v.z);
    }

    ImVec3 operator-(const ImVec3 &v) const {
        return ImVec3(x - v.x, y - v.y, z - v.z);
    }

    bool operator==(const ImVec3 &v) {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const ImVec3 &v) {
        return !(x == v.x && y == v.y && z == v.z);
    }

    static ImVec3 Zero() {
        return ImVec3(0.0f, 0.0f, 0.0f);
    }

    static float Dot(ImVec3 lhs, ImVec3 rhs) {
        return (((lhs.x * rhs.x) + (lhs.y * rhs.y)) + (lhs.z * rhs.z));
    }

    static float Distance(ImVec3 a, ImVec3 b) {
        ImVec3 vector = ImVec3(a.x - b.x, a.y - b.y, a.z - b.z);
        return sqrt(((vector.x * vector.x) + (vector.y * vector.y)) + (vector.z * vector.z));
    }
};

/*变换*/
struct FTransform {
    Quat Rotation;
    ImVec3 Translation;
    ImVec3 Scale3D;
};

struct Camera {
    float x, y, z;
    float yaw, pitch, roll;
    float fov;
};

/*矩阵*/
struct FMatrix {
    float M[4][4];
    float *operator[](int index) {
        return M[index];  // 重载 [] 运算符，使得可以像数组一样访问矩阵的每一行
    }
};

/*骨骼*/
struct BonesData {
    ImVec2 head;
    ImVec2 pit;
    ImVec2 pelvis;
    ImVec2 lcollar;
    ImVec2 rcollar;
    ImVec2 lelbow;
    ImVec2 relbow;
    ImVec2 lwrist;
    ImVec2 rwrist;
    ImVec2 lthigh;
    ImVec2 rthigh;
    ImVec2 lknee;
    ImVec2 rknee;
    ImVec2 lankle;
    ImVec2 rankle;
};

/*跟绘图交互人物数据*/
struct UpPlayer {
    int Team;//队伍编号
    float HP;//人物血量
    int RoBot;//是否人机
    int Distance;//人物距离
    float Angle;//自身 旋转
    int Weapon;//人物手持武器
    ImVec2 size;//人物宽度高度
    BonesData bonesData;//骨骼
    std::string name;//人物名字
    ImVec2 screen;//人物屏幕坐标
    ImVec2 AimTo;//自瞄对象屏幕坐标
    ImVec2 Radar;
    std::string weaponName;//人物手持武器
};

struct UpItem{
    std::string Name;
    float Distance;
    ImVec2 screen;
    struct {
        float x, y, z;
    } worldPosition;
};

/*旋转*/
struct FRotator {
    float Pitch;
    float Yaw;
    float Roll;
};

/*三角函数*/
struct MinimalViewInfo {
    ImVec3 Location;  // 相机位置
    ImVec3 LocationLocalSpace;//  本地空间
    FRotator Rotation; // 相机旋转信息
    float FOV; // 视场角 (Field of View)
};

/*跟线程交互人物迭代*/
struct StaticObject {
    uintptr_t addr;//传输人物指针
    uintptr_t coordAddr;//传输人物坐标指针
    std::string name;//传输人物名字
    int team;//传输人物队伍
    bool RoBot;//传输人物是否人机
    bool locked;
};

struct Config {
    int FPS;//渲染帧数
    bool kongtou;
    bool wuqiBox;
    bool yaoBox;
    bool jingye;
    bool qingji;
    bool juji;
    bool qita;
    bool zhuizongshoulei;
    bool buqiang;
    bool chaojiBox;
    bool chongfeng;
    bool xiandan;
    bool yaopin;
    bool woba;
    bool zidan;
    bool jincha;
    bool beibao;
    bool toukui;
    bool jia;
    bool danjia;
    bool beijing;
    bool loushou;
    bool dantou;
    bool ray;//天线开关
    bool Bone;//骨骼开关
    bool weapon;//持枪开关
    bool OffScreen;//背敌开关
    bool Health1;
    bool Health2;
    bool Health3;
    bool Shoulei;
    bool Name;
    bool Distance;
    bool Radar;
    bool Grenade;
    bool Car;
    bool EscapeBox;
    bool Xinghao;
    bool Flare;
    bool Box;
    bool fangkuang;
    bool leida;
    bool showAimCircle; // 控制自瞄圆圈显示的变量
    bool Yujing;//被瞄
    bool Xuncha;
    float Thickness; // 添加这一行
    int SwitchOffScreen;//背敌样式
    float OffScreenmargin;//背后敌人间距
    int OffScreenBox;//背后敌人方框间距
    float CharacterDis;//显示距离
    float CharacterAim;//自瞄距离
    bool AutoAim; //自瞄总开关
    bool healthEnabled = true; // 默认开启血量显示
    bool ShotGunAim;
    bool enemyHealthDecreased;
    bool SniperAim;
    bool IgnoreRoBot;//Aim忽略人机
    bool IgnoreFall;//Aim忽略倒地
    bool Aimshenxian;//自瞄射线
    bool AimLock;
    bool SmokeNoAim;
    bool displayItemNames;
    bool SniperScopeAim;
    float CharacterRadius;//人物身位
    bool guangjiao;//广角开关
    float anglemix; //超广角数值
    float Radius;//自瞄范围
    float AimSpeed;//自瞄速度
    float AimPrediction; // 自瞄预判值，默认值可以在初始化时设定
    float Pressure;//压枪幅度
    float ShotGunDistance;
    float SniperDistance;
    float ShotDistance;
    float accelerate; //加速数值
    int AimingState;//自瞄模式
    float radarOffsetX;
    float radarOffsetY;
    float RadarSize;
    float radarLineLength; // 新增字段，用于存储雷达中心线长度
};

/*矩阵的变换*/
FMatrix TransformToMatrix(FTransform transform);

/*两个矩阵相乘*/
FMatrix MatrixMulti(FMatrix m1, FMatrix m2);

/*旋转矩阵*/
FMatrix rotatorToMatrix(FRotator rotation);

/*rotatorToMatrix*/
void GetAxes(FRotator Rotation, ImVec3 *X, ImVec3 *Y, ImVec3 *Z);

/*矩阵到向量*/
ImVec3 matrixToVector(FMatrix matrix);

/*计算2维距离*/
float GetDis2D(ImVec2 Screen, ImVec2 Object);

/*是否在范围内*/
bool GetInsideFov(float ScreenWidth, float ScreenHeight, ImVec2 PlayerBone, float FovRadius);

/*计算2D距离*/
float GetCenterOffsetForVector(ImVec2 point, ImVec2 CanvasSize);

/*雷达旋转*/
float RotateAngle(ImVec3 selfCoord, ImVec3 targetCoord);

/*计算旋转坐标*/
ImVec2 RotateCoord(float angle, ImVec2 coord);

//修改
float change(float num);

#endif /* CustomStrct_h */
