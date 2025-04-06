#include <stdio.h>
#include <string>


namespace DataOffset{

    long Gworld = 0x1073E0B8;//Gworld
    
    long Gname = 0x1014B650;//有两个
    
    int PlayerController[3] = {0x98,0x88,0x30};/*人物控制器*/
    
    int SelfPawn = 0x30b0;//Class: STExtraPlayerController.UAEPlayerController.LuaPlayerController.PlayerController.Controller.Actor.Object -> STExtraBaseCharacter* STExtraBaseCharacter;
    
    int ControlRotation = 0x588;//Class: Controller.Actor.Object -> Rotator ControlRotation;
    
    int PlayerCameraManager = 0x5f8;//PlayerController -> PlayerCameraManager* PlayerCameraManager;/*相机管理器*/
    
    int ViewTarget = 0x5d0 + 0x10;//PlayerCameraManager -> CameraCacheEntry CameraCache; + MinimalViewInfo POV;
    
    int MinimalViewInfo[4] = {0x0,0xC,0x18,0x30};//三角函数
    
    int ObjectCount[3] = {0x90,0xA0,0xA8};/*数组*/
    
    int RootComponent = 0x278;//Actor -> 不变SceneComponent* RootComponent; /*对象指针*/
    
    int RelativeLocation = 0x1F0;//可能错SceneComponent -> Vector RelativeLocation /*对象坐标0x17c*/ //  不对0x1bb8
    
    int GetHeight = 0x1ac;//Class: CommonBtnComponent.ActorComponent.Object -> Vector LastCheckLocation; 人物高度
    
    int VelocitySafety = 0xf3c;//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> Vector VelocitySafety  全局向量
    
    int NameData = 0x18;/*name类名*/
    
    int Master = 0xB0;//道具主人 不会变
    
    int TeamMate = 0xab8;// Class: UAEPlayerController.PlayerController.Controller.Actor.Object -> int TeamID  [PlayerController自己队伍]
    
    int Team = 0xa98;// Class: UAECharacter.Character.Pawn.Actor.Object -> int TeamID [Character人物队伍ID]
    
    int IsRoBot = 0xab4;//Class: UAECharacter.Character.Pawn.Actor.Object -> bool bIsAI 人机判断
    
    int PlayerName = 0xa18;//Class: UAECharacter.Character.Pawn.Actor.Object -> FString PlayerName 玩家名字
    
    int bDead = 0xb38;//判断人物是否死亡
    
    int State = 0x1c50;//人物动作 Class: PlayerAntiCheatManager.ActorComponent.Object -> VerifySwitch VSMoveCheatAntiStrategy3;
    
    int OpenFire = 0x2118;//Class: STExtraBaseCharacter.STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> bool bIsWeaponFiring 开火
    
    int OpenTheSight = 0x1570;// Class: STExtraBaseCharacter.STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> bool bIsGunADS 开镜
    
    int GetWeaponOne = 0x30c0 + 0x20;// Class: STExtraBaseCharacter.STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> AnimStatusKeyList LastUpdateStatusKeyList + 0x20 人物手持武器
    
    int ShootMode = 0x1810;//Class: STExtraShootWeapon.STExtraWeapon.Actor.Object -> byte ShootMode; 武器射击模式
    
    int WeaponAttr = 0x17f0;//Class: STExtraShootWeapon.STExtraWeapon.Actor.Object -> ShootWeaponEntity* ShootWeaponEntityComp 武器属性
    
    int AccessoriesVRecoilFactor = 0x1948;//Class: ShootWeaponEntity.WeaponEntity.WeaponLogicBaseComponent.ActorComponent.Object -> float AccessoriesVRecoilFactor; 后坐力系数
    
    int BulletSpeed = 0x1394;//Class: ShootWeaponEntity.WeaponEntity.WeaponLogicBaseComponent.ActorComponent.Object -> float BulletFireSpeed  武器子弹速度
    
    
    int Mesh = 0x5e0;// Class: Character.Pawn.Actor.Object -> SkeletalMeshComponent* Mesh 人物骨骼列阵
    
    int Human = 0x1E0;//SceneComponent* HomingTargetComponent 人物骨骼基矩阵
    
    int Bone[2] = {0x790, 0x30};//UStaticMesh* StaticMesh; 人物骨骼
    
    int BoneCount = 0x790 + 0x8;//骨骼点数
    
    
    int GetHP = 0xe38;//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> float Health 当前血量
    
    int GetHPMax = 0xe40;//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> float HealtMax 最大血量
    
    int NearDeatchComponent = 0x2388;//Class: STExtraBaseCharacter.STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> STCharacterNearDeathComp* NearDeatchComponent; 倒地指针
    
    int CurrentVehicle = 0x1068;// Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> STExtraVehicleBase* CurrentVehicle; 驾驶人物载具
    
    int VehicleCommonComponent = 0xa98;//Class: STExtraVehicleBase.Pawn.Actor.Object -> VehicleCommonComponent* VehicleCommon;
    
    int GetVehicleHP = 0x1F0;//VehicleCommonComponent.VehicleComponent.LuaActorComponent.ActorComponent.Object -> float HP; 载具血量
    
    int GetVehicleFuel = 0x1e4;//VehicleCommonComponent.VehicleComponent.LuaActorComponent.ActorComponent.Object -> float Fuel; 载具油量
    
    int GetVehicleHPMax = 0x1EC;//VehicleCommonComponent.VehicleComponent.LuaActorComponent.ActorComponent.Object -> float HPMax; 载具血量
    
    int GetVehicleFuelMax = 0x1e0;//VehicleCommonComponent.VehicleComponent.LuaActorComponent.ActorComponent.Object -> float FuelMax;载具油量
    
    int BoxPickupWrapperActor = 0x6f8;//Class: PlayerTombBox.Actor.Object -> PickUpListWrapperActor* BoxPickupWrapperActor;/*盒子指针*/
    
    int GoodsList = 0xb10;//Class: PickUpListWrapperActor.PickUpWrapperActor.UAENetActor.Actor.Object -> PickUpItemData[] PickUpDataList; 盒子物资
    
    int DataBase = 0x38;//不会变

}
