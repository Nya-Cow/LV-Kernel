
// Created by Administrator on 2024/1/6 0006.
#include <unistd.h>     //sleep
#include <string>       //string
#include <vector>       //vector
#include <time.h>
#include "Yuaner.h"
#include "draw.h"
#include "DataOffset.h" //数
#include "Draw_ESP.h"
std::unique_ptr<AndroidImgui>  graphics;
using namespace std;
struct Object_gg {
    int count = 0;
    struct {
        ImVec3 zzgg = {0, 0, 0};
    } BoneData[256];
};
struct Object_gg *IsObject_gg = (Object_gg *) calloc(1, sizeof(Object_gg));

static float TargetX = 0;
static float TargetY = 0;

float px = -1, py = -1;
float TouchX = (px * 2) * 0.6;
float TouchY = (py * 2) * 0.6;


timer FAim;
extern Config globalConfig;
float tDistance;
int aimcount = 0;
bool enabledAimbot = false;
int bIsFire;
int bIsAds;



int ItemCount = 0;
int DrawItemCount = 0;

struct UpItem ItemList[2000];

int SmokeCount = 0;
ImVec3 SmokePos[20];

int CoverCount = 0;
ImVec3 CoverPos[20];

struct {
    uintptr_t libAddr = 0;
    uintptr_t Gname;
    uintptr_t Gworld;
    uintptr_t playerController;
    std::string playerControllerClassName;
    uintptr_t cameraManager;
    std::string cameraManagerClassName;
    std::string DrawGname;
    uintptr_t selfAddr;
    MinimalViewInfo pov;
    std::vector<StaticObject> UpList;
} OverallData;


// 互斥锁
pthread_mutex_t lock_readall = PTHREAD_MUTEX_INITIALIZER; //静态初始



ImVec2 worldToScreen(ImVec3 worldLocation, MinimalViewInfo pov) {
    FMatrix tempMatrix = rotatorToMatrix(pov.Rotation);
    ImVec3 vAxisX(tempMatrix[0][0], tempMatrix[0][1], tempMatrix[0][2]);
    ImVec3 vAxisY(tempMatrix[1][0], tempMatrix[1][1], tempMatrix[1][2]);
    ImVec3 vAxisZ(tempMatrix[2][0], tempMatrix[2][1], tempMatrix[2][2]);
    ImVec3 vDelta = worldLocation - pov.Location;
    ImVec3 vTransformed(ImVec3::Dot(vDelta, vAxisY), ImVec3::Dot(vDelta, vAxisZ), ImVec3::Dot(vDelta, vAxisX));
    if (vTransformed.z < 1.0f) {
        vTransformed.z = 1.0f;
    }
    ImVec2 screenCoord;
    screenCoord.x = (px + vTransformed.x * (px / tanf(pov.FOV * ((float) M_PI / 360.0f))) / vTransformed.z);
    screenCoord.y = (py - vTransformed.y * (px / tanf(pov.FOV * ((float) M_PI / 360.0f))) / vTransformed.z);
    return screenCoord;
}




void AimBotAuto(ImVec3 自瞄对象屏幕点) {
    ImVec2 SelfScreen = worldToScreen(OverallData.pov.Location, OverallData.pov);
    ImVec2 AimScreen = worldToScreen(自瞄对象屏幕点, OverallData.pov);
    TargetX = SelfScreen.x - AimScreen.x;
    TargetY = SelfScreen.y - ((SelfScreen.y * 2) - AimScreen.y);
    TargetX /= globalConfig.AimSpeed;
    TargetY /= globalConfig.AimSpeed;
    TouchX -= TargetX;
    TouchY -= TargetY;
    if (!isfinite(TouchX) || !isfinite(TouchY) || !isfinite(AimScreen.x) || !isfinite(AimScreen.y)) {
        Touch::Up();
        TouchX = (px * 2) * 0.6;
        TouchY = (py * 2) * 0.6;
    }
    if (TouchX > ((px * 2) * 0.6 + 300) || TouchX < ((px * 2) * 0.6 - 300) || TouchY > ((py * 2) * 0.6 + 300) ||
        TouchY < ((py * 2) * 0.6 - 300)) {
        Touch::Up();
        TouchX = (px * 2) * 0.6;
        TouchY = (py * 2) * 0.6;
    }
    Touch::Down(TouchY, TouchX);
}




void *main_thread(void *) {
    ::obj_pid = getPID((char *)"com.tencent.tmgp.pubgmhd");
    driver->initialize(::obj_pid);
    OverallData.libAddr = driver->get_module_base((char *)"libUE4.so");

    std::vector<StaticObject> Local_tmpList;
    while (PidExamIne(::obj_pid)) {
        auto selfAddr = OverallData.selfAddr;
        auto playerController = OverallData.playerController;
        auto cameraManager = OverallData.cameraManager;

        OverallData.Gworld = driver->read<uintptr_t>(OverallData.libAddr + DataOffset::Gworld);
        OverallData.Gname = driver->read<uintptr_t>(OverallData.libAddr + DataOffset::Gname);
        OverallData.playerController = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(OverallData.Gworld + DataOffset::PlayerController[0]) + DataOffset::PlayerController[1]) + DataOffset::PlayerController[2]);
        OverallData.selfAddr = driver->read<uintptr_t>(OverallData.playerController + DataOffset::SelfPawn);
        OverallData.cameraManager = driver->read<uintptr_t>(OverallData.playerController + DataOffset::PlayerCameraManager);
        if(globalConfig.guangjiao){
	    driver->write<float>(driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(OverallData.libAddr + 0x10715E70) + 0x30) + 0x30b0) + 0xf70) + 0x2E4,globalConfig.anglemix);
		}
        auto uLevel = driver->read<uintptr_t>(OverallData.Gworld + DataOffset::ObjectCount[0]);
        auto objectArray = driver->read<uintptr_t>(uLevel + DataOffset::ObjectCount[1]);
        int objectCount = driver->read<int>(uLevel + DataOffset::ObjectCount[2]);

        if (objectCount <= 0 || objectCount > 2000) continue;

        ItemCount = 0;
        int SmokeNum = 0;

        for (int index = 0; index < objectCount; index++) {
            auto objectAddr = driver->read<uintptr_t>(objectArray + 8 * index);
            if (objectAddr <= 0xffff || objectAddr <= 0x10000000 || objectAddr % 4 != 0 || objectAddr >= 0x10000000000) {
                continue;
            }

            auto coordAddr = driver->read<uintptr_t>(objectAddr + DataOffset::RootComponent);
            ImVec3 Relative;
            driver->read(coordAddr + DataOffset::RelativeLocation, &Relative, sizeof(Relative));
            ImVec3 Z;
            driver->read(driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::RootComponent) + DataOffset::RelativeLocation, (void *)&Z, sizeof(ImVec3));
            float Distance = ImVec3::Distance(Z, Relative) / 100;

            if (Relative.x == 0 || Relative.y == 0 || Relative.z == 0) continue;

            std::string className = getName(objectAddr);
            if (strstr(className.c_str(), "PlayerPawn") != nullptr) {
                int State = driver->read<int>(objectAddr + DataOffset::State);
                if (State == 1048592 || State == 1048576) {
                    continue;
                }
                int Team = driver->read<int>(objectAddr + DataOffset::Team);
                int myTeam = driver->read<int>(OverallData.playerController + DataOffset::TeamMate);
                if (Team < 0 || Team == myTeam) {
                    continue;
                }
                StaticObject tmpData;
                tmpData.addr = objectAddr;
                tmpData.coordAddr = coordAddr;
                tmpData.team = Team;
                tmpData.RoBot = driver->read<int>(objectAddr + DataOffset::IsRoBot);
                tmpData.name = getPlayerName(driver->read<uintptr_t>(objectAddr + DataOffset::PlayerName));
                Local_tmpList.push_back(tmpData);
            } else {
                ItemList[ItemCount].Name = className;
                ItemList[ItemCount].Distance = Distance;
                ItemList[ItemCount].screen = worldToScreen(Relative, OverallData.pov);
                ItemCount++;

                if (className.find("ProjSmoke_") != string::npos) {
                    SmokePos[SmokeNum] = Relative;
                    SmokeNum++;
                }
            }
        }

        if (Local_tmpList.size() > 0) {
            pthread_mutex_lock(&lock_readall);
            OverallData.UpList.assign(Local_tmpList.begin(), Local_tmpList.end());
            pthread_mutex_unlock(&lock_readall);
            Local_tmpList.clear();
        } else {
            pthread_mutex_lock(&lock_readall);
            if (!OverallData.UpList.empty()) {
                OverallData.UpList.clear();
            }
            pthread_mutex_unlock(&lock_readall);
        }

        SmokeCount = SmokeNum;
        DrawItemCount = ItemCount;
        usleep(10 * 1000);
    }
    return nullptr;
}

void UpData(ImVec2 Screen, vector<UpPlayer> &UpList) {
    px = Screen.x;
    py = Screen.y;
    UpList.clear();
    OverallData.cameraManagerClassName = getName(OverallData.cameraManager);
    OverallData.playerControllerClassName = getName(OverallData.playerController);
    driver->read(OverallData.cameraManager + DataOffset::ViewTarget + DataOffset::MinimalViewInfo[0], (void *)&OverallData.pov.Location, sizeof(OverallData.pov.Location));
    driver->read(OverallData.cameraManager + DataOffset::ViewTarget + DataOffset::MinimalViewInfo[1], (void *)&OverallData.pov.LocationLocalSpace, sizeof(OverallData.pov.LocationLocalSpace));
    driver->read(OverallData.cameraManager + DataOffset::ViewTarget + DataOffset::MinimalViewInfo[2], (void *)&OverallData.pov.Rotation, sizeof(OverallData.pov.Rotation));
    driver->read(OverallData.cameraManager + DataOffset::ViewTarget + DataOffset::MinimalViewInfo[3], (void *)&OverallData.pov.FOV, sizeof(OverallData.pov.FOV));
    ImVec3 SelfCoord = OverallData.pov.Location;
    float lateralAngleView = driver->read<float>(OverallData.playerController + DataOffset::ControlRotation + 0x4) - 90;
    float RotatorYaw = driver->read<float>(OverallData.playerController + DataOffset::ControlRotation + 0x4);

    pthread_mutex_lock(&lock_readall); // 获取锁
    static std::vector<StaticObject> Local_new_List; Local_new_List = OverallData.UpList;            
    pthread_mutex_unlock(&lock_readall); // 释放锁  

    for (size_t i = 0; i < Local_new_List.size(); ++i) {
        const auto &staticData = Local_new_List[i];
        ImVec3 D;
        driver->read(staticData.coordAddr + DataOffset::RelativeLocation, &D, sizeof(ImVec3)); // AnQu
        ImVec3 Z;
        driver->read(driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::RootComponent) + DataOffset::RelativeLocation, (void *)&Z, sizeof(ImVec3));
        float objectDistance = ImVec3::Distance(SelfCoord, D) / 100; // Pov人物距离


        if (objectDistance == 0 || objectDistance > globalConfig.CharacterDis) {
            continue;        
        }
        auto objectHeight = driver->read<float>(staticData.coordAddr + DataOffset::GetHeight);
        /*
        if (objectHeight < 20) {
            continue;
        }
        if (objectHeight < 50) {
            objectHeight -= 18;
        } else if (objectHeight > 80) {
            objectHeight += 12;
        }

        int State = driver->read<int>(staticData.addr + DataOffset::State);//
        if (State == 1048592 || State == 1048576) {
            continue;
        }
        
        
          bool 奥利给 = driver->read<int>(staticData.addr + 0xe90) & 1; 
          if(奥利给) continue ;

        int Team = driver->read<int>(staticData.addr + DataOffset::Team);
        int myTeam = driver->read<int>(OverallData.playerController + DataOffset::TeamMate);
        if (Team < 0 || Team == myTeam) {
            continue;
        }*/
        auto Mesh = driver->read<uintptr_t>(staticData.addr + DataOffset::Mesh);
        uintptr_t human = Mesh + DataOffset::Human;
        uintptr_t Bone = driver->read<uintptr_t>(Mesh + DataOffset::Bone[0]) + DataOffset::Bone[1];
        int bonecount = driver->read<int>(Mesh + DataOffset::BoneCount);
        int i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15;
        UpPlayer UpDate;
        UpDate.Radar = GetRadar(D,Z,RotatorYaw);
        UpDate.Angle = lateralAngleView - RotateAngle(SelfCoord, D) - 180;
        int Health = driver->read<float>(staticData.addr + DataOffset::GetHP);
        int HealthMax = driver->read<float>(staticData.addr + DataOffset::GetHPMax);
        UpDate.HP = 100.0f * Health / HealthMax;
        UpDate.name = staticData.name;
        UpDate.Distance = objectDistance;
        UpDate.RoBot = staticData.RoBot;
        UpDate.Team = staticData.team;
        
        
        UpDate.Weapon = driver->read<int>(driver->read<uintptr_t>(driver->read<uintptr_t>(staticData.addr + 0x2e70) + 0x658) + 0xb90);//手持武器id
        
        
        
        ImVec2 width, height;
        UpDate.screen = worldToScreen(D, OverallData.pov);
        width = worldToScreen(ImVec3(D.x, D.y, D.z + 100), OverallData.pov);
        height = worldToScreen(ImVec3(D.x, D.y, D.z + objectHeight), OverallData.pov);
        UpDate.size.x = (UpDate.screen.y - width.y) / 2;
        UpDate.size.y = UpDate.screen.y - height.y;
if (globalConfig.AutoAim) {
            for (int i = 0; i < IsObject_gg->count; i++) {
                if (IsObject_gg->BoneData[i].zzgg.x != 0 || IsObject_gg->BoneData[i].zzgg.y != 0 ||
                    IsObject_gg->BoneData[i].zzgg.z != 0) {
                    UpDate.AimTo = worldToScreen({IsObject_gg->BoneData[i].zzgg.x, IsObject_gg->BoneData[i].zzgg.y,
                                                  IsObject_gg->BoneData[i].zzgg.z}, OverallData.pov);
                }
            }
            if (UpDate.AimTo.x == 0 && UpDate.AimTo.y == 0) {
                UpDate.AimTo = {0, 0};
                Touch::Up();
                TouchX = (px * 2) * 0.6f;
                TouchY = (py * 2) * 0.6f;
            }
        }

        if (UpDate.size.x > 0) {
            BonesData bonesData;
            if (bonecount == 68) {
                i1 = 5;
                i2 = 4;
                i3 = 1;
                i4 = 11;
                i5 = 33;
                i6 = 12;
                i7 = 34;
                i8 = 13;
                i9 = 35;
                i10 = 55;
                i11 = 59;
                i12 = 56;
                i13 = 60;
                i14 = 57;
                i15 = 61;
            } else {
                i1 = 5;
                i2 = 4;
                i3 = 1;
                i4 = 11;
                i5 = 32;
                i6 = 12;
                i7 = 33;
                i8 = 63;
                i9 = 62;
                i10 = 53;
                i11 = 56;
                i12 = 53;
                i13 = 57;
                i14 = 54;
                i15 = 58;
            }


if (GetBoneCameraCache(OverallData.pov, human, Bone, i1, bonesData.head))//头
if (GetBoneCameraCache(OverallData.pov, human, Bone, i2, bonesData.pit))//胸口
if (GetBoneCameraCache(OverallData.pov, human, Bone, i3, bonesData.pelvis))//屁股
if (GetBoneCameraCache(OverallData.pov, human, Bone, i4, bonesData.lcollar))//左肩
if (GetBoneCameraCache(OverallData.pov, human, Bone, i5, bonesData.rcollar))//右肩
if (GetBoneCameraCache(OverallData.pov, human, Bone, i6, bonesData.lelbow))//左手肘
if (GetBoneCameraCache(OverallData.pov, human, Bone, i7, bonesData.relbow))//右手肘
if (GetBoneCameraCache(OverallData.pov, human, Bone, i8, bonesData.lwrist))//左手腕
if (GetBoneCameraCache(OverallData.pov, human, Bone, i9, bonesData.rwrist))//右手腕
if (GetBoneCameraCache(OverallData.pov, human, Bone, i10,bonesData.lthigh))//左大腿
if (GetBoneCameraCache(OverallData.pov, human, Bone, i11,bonesData.rthigh))//右大腿
if (GetBoneCameraCache(OverallData.pov, human, Bone, i12,bonesData.lknee))//左膝盖
if (GetBoneCameraCache(OverallData.pov, human, Bone, i13,bonesData.rknee))//右膝盖
if (GetBoneCameraCache(OverallData.pov, human, Bone, i14,bonesData.lankle))//左脚腕
if (GetBoneCameraCache(OverallData.pov, human, Bone, i15,bonesData.rankle))//右脚腕
UpDate.bonesData = bonesData; // 传输 骨骼坐标转屏幕绘图 -> 绘图
}
UpList.push_back(UpDate);
       } // 人物for
    }






void *AimBot(void *) {
    FAim.setAffinity();
    while (true) {
        FAim.SetFps(globalConfig.FPS);
        FAim.AotuFPS();

        if (globalConfig.AutoAim && ::obj_pid > 10) {
            uintptr_t myGunBase = driver->read<uintptr_t>(
                driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::GetWeaponOne) + DataOffset::WeaponAttr);
            ImVec3 selfCoord = OverallData.pov.Location;
            ImVec2 selfScreen = worldToScreen(OverallData.pov.Location, OverallData.pov);
            StaticObject aimData;
            ImVec3 aimbotCoord = ImVec3{0, 0, 0};
            float aimbotRadius = globalConfig.Radius;
            aimData.addr = 0;

            pthread_mutex_lock(&lock_readall); // 获取锁
            static std::vector<StaticObject> Local_new_List; Local_new_List = OverallData.UpList;            
            pthread_mutex_unlock(&lock_readall); // 释放锁  

            for (size_t i = 0; i < Local_new_List.size(); ++i) {
                const auto &staticData = Local_new_List[i];
                int State = driver->read<int>(staticData.addr + DataOffset::State);
                if (State == 1048592 || State == 1048576) {
                    continue;
                }
                int Team = driver->read<int>(staticData.addr + DataOffset::Team);
                int myTeam = driver->read<int>(OverallData.playerController + DataOffset::TeamMate);
                if (Team < 0 || Team == myTeam) continue;
                if (driver->read<int>(staticData.addr + DataOffset::IsRoBot) && globalConfig.IgnoreRoBot)
                    continue; // 忽略人机
                
                float GetHP = driver->read<float>(staticData.addr + DataOffset::GetHP);
                float GetHPMax = driver->read<float>(staticData.addr + DataOffset::GetHPMax);
                if ((GetHP * 100 / GetHPMax) < 0.01 && globalConfig.IgnoreFall) continue;//忽略倒地
                if (globalConfig.enemyHealthDecreased) {
                if ((GetHP / GetHPMax) < 0.90f) {
                }
                 else {
                 continue;
                  }
                 }

                // 执行自瞄逻辑...
                auto bulletSpeed = driver->read<float>(myGunBase + DataOffset::BulletSpeed);
                auto meshAddr = driver->read<uintptr_t>(staticData.addr + DataOffset::Mesh);
                uintptr_t humanAddr = meshAddr + DataOffset::Human;
                uintptr_t boneAddr = driver->read<uintptr_t>(meshAddr + DataOffset::Bone[0]) + DataOffset::Bone[1];
                ImVec3 objectCoord = GetBone(humanAddr, boneAddr, 5);
                float objectDistance = ImVec3::Distance(selfCoord, objectCoord) / 100;

                if (objectDistance < 0 || objectDistance > globalConfig.CharacterAim) continue;

                float bulletFlyTime = ImVec3::Distance(selfCoord, objectCoord) / bulletSpeed;
                bulletFlyTime *= 1.2;

                ImVec3 moveCoord;
                driver->read(staticData.addr + DataOffset::VelocitySafety, &moveCoord, 12);

                float predictionFactor = globalConfig.AimPrediction;
                objectCoord.x += moveCoord.x * bulletFlyTime * predictionFactor;
                objectCoord.y += moveCoord.y * bulletFlyTime * predictionFactor;
                objectCoord.z += moveCoord.z * bulletFlyTime * predictionFactor;
                
                auto weaponAddr = driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::GetWeaponOne);

                int Byte = driver->read<int>(weaponAddr + DataOffset::ShootMode);
                bIsFire = driver->read<int>(OverallData.selfAddr + DataOffset::OpenFire);
                bIsAds = driver->read<int>(OverallData.selfAddr + DataOffset::OpenTheSight);
                if (Byte >= 1024) {
                    enabledAimbot = bIsFire == 1;
                } else {
                    enabledAimbot = bIsAds == 257 || bIsAds == 1;
                }
                auto objectHeight = driver->read<float>(staticData.coordAddr + DataOffset::GetHeight);
                ImVec2 playerScreen = worldToScreen(objectCoord, OverallData.pov);
                ImVec2 playerScreensize, width, height;
                width = worldToScreen(
                        ImVec3(objectCoord.x, objectCoord.y, objectCoord.z + (100 * globalConfig.CharacterRadius)),
                        OverallData.pov);
                height = worldToScreen(ImVec3(objectCoord.x, objectCoord.y,
                                              objectCoord.z + (objectHeight * globalConfig.CharacterRadius)), OverallData.pov);
                playerScreensize.x = (playerScreen.y - width.y) / 2;
                playerScreensize.y = playerScreen.y - height.y;
                bool Ismode;
                bool IsmodeTw;
                switch (globalConfig.AimingState) {
                    case 0:
                        Ismode = playerScreen.x < (selfScreen.x * 2) && playerScreen.x > 0 && playerScreen.y > 0 &&
                                 playerScreen.y < (selfScreen.y * 2) &&
                                 GetInsideFov(selfScreen.x, selfScreen.y, playerScreen, aimbotRadius) && myGunBase;
                        IsmodeTw = tDistance <= aimbotRadius;
                        break;
                    case 1:
                        Ismode = GetInsideFov(selfScreen.x, selfScreen.y, playerScreen, aimbotRadius) &&
                                 playerScreen.x < (selfScreen.x * 2) && playerScreen.x > 0 && playerScreen.y > 0 &&
                                 playerScreen.y < (selfScreen.y * 2) &&
                                 !(playerScreen.x + playerScreensize.x < selfScreen.x ||
                                   playerScreen.x - playerScreensize.x > selfScreen.x ||
                                   playerScreen.y + playerScreensize.y < selfScreen.y ||
                                   playerScreen.y - playerScreensize.y > selfScreen.y) && myGunBase;
                        IsmodeTw = tDistance <= aimbotRadius;
                        break;
                    case 2:
    // 检查敌人是否在屏幕内且在自瞄范围内
                         Ismode = playerScreen.x < (selfScreen.x * 2) && playerScreen.x > 0 && playerScreen.y > 0 &&
                         playerScreen.y < (selfScreen.y * 2) &&
                         GetInsideFov(selfScreen.x, selfScreen.y, playerScreen, aimbotRadius) && myGunBase;  
                         IsmodeTw = tDistance <= aimbotRadius;
                         break;
                         case 3:
    // 检查敌人是否在屏幕内且在自瞄范围内
                         Ismode = playerScreen.x < (selfScreen.x * 2) && playerScreen.x > 0 && playerScreen.y > 0 &&
                         playerScreen.y < (selfScreen.y * 2) &&
                         GetInsideFov(selfScreen.x, selfScreen.y, playerScreen, aimbotRadius) && myGunBase;  
                         IsmodeTw = tDistance <= aimbotRadius;
                         
                         break;
                }
                                if (Ismode) {
                    tDistance = GetCenterOffsetForVector(selfScreen, playerScreen);
                    if (IsmodeTw) {
                    
                    
                        if (globalConfig.AimLock){
                        
                        
                            if (!enabledAimbot || aimData.locked){
                                aimbotRadius = tDistance;
                                aimbotCoord = objectCoord;
                                aimData = staticData;
                                aimData.locked = true;
                            }
                        }else{
                            aimbotRadius = tDistance;
                            aimbotCoord = objectCoord;
                            aimData = staticData;
                        }
                    } else {
                        aimbotCoord = {0, 0, 0};
                    }
                }
            }

if (myGunBase && aimData.addr != 0 && aimbotCoord.x != 0 && aimbotCoord.y != 0 && aimbotCoord.z != 0) {
    
    IsObject_gg->BoneData[aimcount].zzgg = {aimbotCoord.x, aimbotCoord.y, aimbotCoord.z};
    aimcount++;
    
    float distance1 = ImVec3::Distance(selfCoord, aimbotCoord) / 100;
    
    auto objectHeight = driver->read<float>(
            driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::RootComponent) + DataOffset::GetHeight);
    if (objectHeight < 50) {
        objectHeight -= 18;
    } else if (objectHeight > 80) {
        objectHeight += 12;
    }
    
    auto weaponAddr = driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::GetWeaponOne);
    int Byte = driver->read<int>(weaponAddr + DataOffset::ShootMode);
    auto weaponAttrAddr = driver->read<uintptr_t>(weaponAddr + DataOffset::WeaponAttr);
    auto YuCurrentVehicle = driver->read<uintptr_t>(aimData.addr + DataOffset::CurrentVehicle);
    auto CurrentVehicle = driver->read<uintptr_t>(OverallData.selfAddr + DataOffset::CurrentVehicle);
    auto AccessoriesVRecoilFactor = driver->read<float>(weaponAttrAddr + DataOffset::AccessoriesVRecoilFactor);
    static float shangtai = 0.0f;
    shangtai = AccessoriesVRecoilFactor;
    if (objectHeight < 48.f  || objectHeight < 54.f) {
        shangtai /= 8.5f;
    } else {
        if (isVaildPtr(CurrentVehicle)) {
            shangtai /= 2.f;
        } else {
            shangtai /= 2.5f;
        }
    }
    if (driver->read<int>(OverallData.selfAddr + DataOffset::OpenFire) == 1) {
        if (Byte >= 1024) {
            aimbotCoord.z -= distance1 * globalConfig.Pressure * shangtai;
        }
    }

    if (distance1 > globalConfig.ShotDistance && bIsFire == 1 && bIsAds != 1 && bIsAds != 257){
        enabledAimbot = false;
    }

    if (globalConfig.SmokeNoAim){
        int InSmoke = 0;
        for (int i = 0; i < SmokeCount; i++) {
            float SmokeDistance =  ImVec3::Distance(SmokePos[i], aimbotCoord) / 100;
            if (SmokeDistance < 20) InSmoke++;
        }
        if (InSmoke != 0) enabledAimbot = false;
    }

                
                                  
                string WeaponName = getName(weaponAddr);
                                
if (globalConfig.ShotGunAim) {
    if (strstr(WeaponName.c_str(), "BP_ShotGun_SPAS-12_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_DP12_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S12K_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_AA12_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S1897_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S686_Wrappe_1_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S686_Wrappe_2_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S686_Wrappe_3_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S1897_Wrapper_2_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S1897_Wrapper_3_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S1897_Wrapper_4_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_DP12_Wrapper_2_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_DP12_Wrapper_3_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_DP12_Wrapper_4_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_AA12_Wrapper_2_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_AA12_Wrapper_3_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_AA12_Wrapper_4_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_SPAS-12_Wrapper_2_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_SPAS-12_Wrapper_3_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_SPAS-12_Wrapper_4_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_AA12_C") != nullptr ||
        strstr(WeaponName.c_str(), "BP_ShotGun_S686_C") != nullptr) {
            if (distance1 <= globalConfig.ShotGunDistance) {
                enabledAimbot = true;
            }
        }
    }
    
                    bool isSniper = strstr(WeaponName.c_str(), "BP_Sniper_M200_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M24_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AMR_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AWM_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AWM_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AWM_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AWM_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M200_Wrapper_3_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M200_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M200_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M200_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AMR_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AMR_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_AMR_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_1_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_2_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_3_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_Wrapper_2_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_Wrapper_3_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Mosin_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_Wrapper_2_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_Wrapper_3_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Kar98k_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M24_Wrapper_3_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M24_Wrapper_4_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M24_Wrapper_5_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_M24_Wrapper_6_C") != nullptr ||
                    strstr(WeaponName.c_str(), "BP_Sniper_Win94_C") != nullptr;
                    
                    
                    
                    
            
                   bool isAiming = bIsAds == 257 || bIsAds == 1;
                    
             
           if ((isSniper && isAiming && globalConfig.SniperAim) || (myGunBase && enabledAimbot)) {
             AimBotAuto(aimbotCoord);
           } else {
                         

        Touch::Up();
        TouchX = (px * 2) * 0.6;
        TouchY = (py * 2) * 0.6;
 
         }
     }
            IsObject_gg->count = aimcount;
            aimcount = 0;
        
       }
	   usleep(2 * 1000);
    }//while
    return nullptr;
}


           

            










//自瞄结束
ImVec3 GetBone(uintptr_t human, uintptr_t bones, int part) {
    FTransform actorftf;
    driver->read(human, &actorftf.Rotation, sizeof(ImVec4));
    driver->read(human + 0x10, &actorftf.Translation, sizeof(ImVec3));
    driver->read(human + 0x20, &actorftf.Scale3D, sizeof(ImVec3));
    FMatrix actormatrix = TransformToMatrix(actorftf);
    FTransform boneftf;
    driver->read(bones + part * 48, &boneftf.Rotation, sizeof(ImVec4));
    driver->read(bones + part * 48 + 0x10, &boneftf.Translation, sizeof(ImVec3));
    driver->read(bones + part * 48 + 0x20, &boneftf.Scale3D, sizeof(ImVec3));
    FMatrix bonematrix = TransformToMatrix(boneftf);
    return matrixToVector(MatrixMulti(bonematrix, actormatrix));
}

bool GetBoneCameraCache(MinimalViewInfo pov, uintptr_t human, uintptr_t bones, int part, ImVec2 &buf) {
    ImVec3 newmatrix = GetBone(human, bones, part);
    buf = worldToScreen(newmatrix, pov);
    return buf.x != 0 && buf.y != 0;
}

bool isVaildPtr(uintptr_t ptr) {
    return (ptr > 0x10000000) && (ptr < 0x10000000000) ? true : false;
}

void getName(int id, char *bbb)
{
int classname;
int m = 0;
if (id > 0 && id < 2000000)
{
int ye = id / 16384;
int xu = id % 16384;
auto ddz = driver->read<uintptr_t>(driver->read<uintptr_t>(OverallData.libAddr + DataOffset::Gname)  + ye * 8);
auto namedz = driver->read<uintptr_t>(ddz + xu * 8);
for (int i = 0; i < 8; i++)
{
classname = driver->read<int>(namedz + 12 + i * 4);
memcpy(&bbb[m], &classname, 4);
m += 4;
}
}
}
string GetFNameFromID(unsigned int index) {
char name[128]="";
getName(index,name);
return reinterpret_cast<char *>(name);
}
int getNameID(uintptr_t object) {
return driver->read<int>(object + DataOffset::NameData);
}
string getName(uintptr_t object) {
return GetFNameFromID(getNameID(object));
}

char *getPlayerName(uintptr_t addr) {
    char *buf = (char *) malloc(448);
    unsigned short buf16[16] = {0};
    driver->read(addr, buf16, 28);
    unsigned short *tempbuf16 = buf16;
    char *tempbuf8 = buf;
    char *buf8 = tempbuf8 + 32;
    while (tempbuf16 < tempbuf16 + 28) {
        if (*tempbuf16 <= 0x007F && tempbuf8 + 1 < buf8) {
            *tempbuf8++ = (char) *tempbuf16;
        } else if (*tempbuf16 >= 0x0080 && *tempbuf16 <= 0x07FF && tempbuf8 + 2 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 6) | 0xC0;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else if (*tempbuf16 >= 0x0800 && *tempbuf16 <= 0xFFFF && tempbuf8 + 3 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 12) | 0xE0;
            *tempbuf8++ = ((*tempbuf16 >> 6) & 0x3F) | 0x80;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else {
            break;
        }
        tempbuf16++;
    }
    return buf;
}

ImVec2 GetRadar(ImVec3 Pos,ImVec3 CameraPos,float Yaw){
    float s = sin((Yaw - 90) * M_PI / 180);
    float c = cos((Yaw - 90) * M_PI / 180);
    float objRadar_x = (CameraPos.x - Pos.x) / 50.f;
    float objRadar_y = (CameraPos.y - Pos.y) / 50.f;

    ImVec2 Point;
    Point.x = objRadar_x * c + objRadar_y * s;
    Point.y = -objRadar_x * s + objRadar_y * c;

    return Point;
}

bool CapTivity(float Range, ImVec2 Pos) {
    int lx = abs((px + Pos.x) - px);
    int ly = abs((py + Pos.y) - py);
    float SideLength = sqrt((lx * lx) + (ly * ly));
    if (SideLength <= Range) {
        return true;
    }
    return false;
}

//武器
char *GetHoldaGunItem(int WeaponId) {
    switch (WeaponId) {
        //突击步枪
        case 101008:
            return (char *)"M762";
            break;
        case 101001:
            return (char *)"AKM";
            break;
        case 101004:
            return (char *)"M416";
            break;
        case 101003:
            return (char *)"SCAR-L";
            break;
        case 101002:
            return (char *)"M16A4";
            break;
        case 101009:
            return (char *)"Mk47";
            break;
        case 101006:
            return (char *)"AUG";
            break;
        case 101005:
            return (char *)"Groza";
            break;
        case 101010:
            return (char *)"G36C";
            break;
        case 101007:
            return (char *)"QBZ";
            break;
        case 101011:
            return (char *)"AC-VAL";
            break;
        case 101012:
            return (char *)"蜜獾突击步枪";
            break;
            //连狙
        case 103009:
            return (char *)"SLR";
            break;
        case 103005:
            return (char *)"VSS";
            break;
        case 103006:
            return (char *)"Mini14";
            break;
        case 103010:
            return (char *)"QBU";
            break;
        case 103004:
            return (char *)"SKS";
            break;
        case 103007:
            return (char *)"MK14";
            break;
        case 103014:
            return (char *)"MK20-H";
            break;
        case 103013:
            return (char *)"M417";
            break;
            //连狙
        case 103012:
            return (char *)"AMR";
            break;
        case 103003:
            return (char *)"AWM";
            break;
        case 103002:
            return (char *)"M24";
            break;
        case 103011:
            return (char *)"莫甘娜辛";
            break;
        case 103001:
            return (char *)"Kar98K";
            break;
        case 103008:
            return (char *)"Win94";
            break;
            //机关枪
        case 105001:
            return (char *)"M249";
            break;
        case 105002:
            return (char *)"DP-28";
        case 105010:
            return (char *)"MG3";
            break;
        case 107001:
            return (char *)"十字弩";
            break;
        case 107007:
            return (char *)"爆炸猎弓";
            break;
            //冲锋枪
        case 102001:
            return (char *)"UZI";
            break;
        case 102003:
            return (char *)"Vector";
            break;
        case 100103:
            return (char *)"PP-19";
            break;
        case 102007:
            return (char *)"MP5K";
            break;
        case 102002:
            return (char *)"UMP-45";
            break;
        case 102004:
            return (char *)"汤姆逊";
            break;
        case 102105:
            return (char *)"P90";
            break;
        case 102005:
            return (char *)"野牛";
            break;
            //霰弹枪
        case 104001:
            return (char *)"S686";
            break;
        case 104002:
            return (char *)"S1897";
            break;
        case 104003:
            return (char *)"S12K";
            break;
        case 104004:
            return (char *)"DBS";
            break;
        case 104005:
            return (char *)"AA12-G";
            break;
        case 104100:
            return (char *)"SPAS-12";
            break;
            //投掷爆炸物
        case 602004:
            return (char *)"手榴弹";
            break;
        case 602003:
            return (char *)"燃烧瓶";
            break;
        case 602002:
            return (char *)"烟雾弹";
            break;
        case 602001:
            return (char *)"震撼弹";
            break;
            //近战武器
        case 108003:
            return (char *)"镰刀";
            break;
        case 108002:
            return (char *)"撬棍";
            break;
        case 108001:
            return (char *)"大砍刀";
            break;
        case 108004:
            return (char *)"平底锅";
            break;
        default:
            return (char *)"拳头";
            break;
    }
}

int GetWeaponPic(int WeaponID) {
    switch(WeaponID) {
        case 101001: return 1;break;
        case 101002: return 2;break;
        case 101003: return 3;break;
        case 101004: return 4;break;
        case 101005: return 5;break;
        case 101006: return 6;break;
        case 101007: return 7;break;
        case 101008: return 8;break;
        case 101009: return 9;break;
        case 101010: return 10;break;
        case 101011: return 11;break;
        case 101012: return 12;break;
        case 102001: return 13;break;
        case 102002: return 14;break;
        case 102003: return 15;break;
        case 102004: return 16;break;
        case 102005: return 17;break;
        case 102007: return 18;break;
        case 102105: return 19;break;
        case 103001: return 20;break;
        case 103002: return 21;break;
        case 103003: return 22;break;
        case 103004: return 23;break;
        case 103005: return 24;break;
        case 103006: return 25;break;
        case 103007: return 26;break;
        case 103008: return 27;break;
        case 103009: return 28;break;
        case 103010: return 29;break;
        case 103011: return 30;break;
        case 103012: return 31;break;
        case 103013: return 32;break;
        case 103014: return 33;break;
        case 103015: return 34;break;
        case 103100: return 35;break;
        case 103901: return 36;break;
        case 103902: return 37;break;
        case 103903: return 38;break;
        case 104001: return 39;break;
        case 104002: return 40;break;
        case 104003: return 41;break;
        case 104004: return 42;break;
        case 104100: return 43;break;
        case 105001: return 44;break;
        case 105002: return 45;break;
        case 105010: return 46;break;
        case 106001: return 47;break;
        case 106002: return 48;break;
        case 106003: return 49;break;
        case 106004: return 50;break;
        case 106005: return 51;break;
        case 106006: return 52;break;
        case 106007: return 53;break;
        case 106008: return 54;break;
        case 106010: return 55;break;
        case 107001: return 56;break;
        case 107006: return 57;break;
        case 107007: return 58;break;
        case 107986: return 59;break;
        case 108001: return 60;break;
        case 108002: return 61;break;
        case 108003: return 62;break;
        case 108004: return 63;break;
        case 602001: return 64;break;
        case 602002: return 65;break;
        case 602003: return 66;break;
        case 602004: return 67;break;
        case 602011: return 68;break;
        case 602030: return 69;break;
        case 602041: return 70;break;
        case 602042: return 71;break;
        case 602043: return 72;break;
        case 812146: return 73;break;
        case 812147: return 74;break;
        case 812148: return 75;break;
        case 812149: return 76;break;
        default:
            return 0;
            break;
    }
}