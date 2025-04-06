#include "draw.h"    // 绘制套
#include "AndroidImgui.h"     // 创建绘制套
#include "GraphicsManager.h" // 获取当前渲染模式
#include "Draw_ESP.h"
#include "Yuaner.h"
#include "timer.h"
#include "网络验证/My_verify.h" 
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <iostream>
#include <string>

// 全局变量用于存储验证错误信息
std::string globalErrorMsg;
int Pattern;
bool 悬浮窗 = true; // 默认悬浮窗显示

// 获取输入设备数量
int 数据() {
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    closedir(dir);
    return count ? count : -1;
}

// 音量键开关功能
void 音量键隐藏() {
    int EventCount = 数据();
    if (EventCount < 0) {
        std::cerr << "未找到输入设备" << std::endl;
        exit(1);
    }

    int *fdArray = (int *)malloc(EventCount * sizeof(int));

    for (int i = 0; i < EventCount; i++) {
        char temp[128];
        sprintf(temp, "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
    }

    input_event ev;

    while (1) {
        for (int i = 0; i < EventCount; i++) {
            memset(&ev, 0, sizeof(ev));
            if (read(fdArray[i], &ev, sizeof(ev)) < 0) {
                continue; // 如果读取失败，跳过当前设备
            }
            if (ev.type == EV_KEY && ev.code == KEY_VOLUMEDOWN && ev.value == 1) {
                悬浮窗 = false; // 隐藏悬浮窗
            }
            if (ev.type == EV_KEY && ev.code == KEY_VOLUMEUP && ev.value == 1) {
                悬浮窗 = true; // 显示悬浮窗
            }
        }
    }
usleep(10000); // 10ms延迟减少CPU占用
}
int main(int argc, char *argv[]) {
    int yes = -1;
    char ErrStr[256] = {0};
    微验验证(&yes, (char *)&ErrStr); // 里面有路径
    if (yes == 10086) {
        std::cout << "\033[33;1m"; // 黄色
        std::cout << "[+]到期时间:" << ErrStr << std::endl;
        ::driver = new c_driver(); //(new || 调用安装驱动)
        //DriverInstallVersion(); //调用安装驱动
    } else {
        exit(0);    
    }
    std::cout << "[+]1-----无后台(占用比较高)\n[+]2-----有后台\n[-]3-----退出\n";
    std::cout << "[+]请输入你的选择:";
    std::cin >> Pattern;
    
    if (Pattern == 2) { // 有后台
        goto 继续;
    } else if (Pattern == 1) {  // 无后台
        pid_t pid = fork();
        if (pid > 0) {
            exit(0);
        } else if (pid == 0) {
            // 子进程继续运行
        } else {
            std::cerr << "创建子进程失败" << std::endl;
            exit(1);
        }
    } else if (Pattern == 3) {   // 退出
        exit(1);
    } else {
        exit(1);
    }
    继续:
    pthread_t Menu;
    pthread_create(&Menu, nullptr, main_thread, nullptr);
    pthread_t Aim;
    pthread_create(&Aim, nullptr, AimBot, nullptr);

    ::graphics = GraphicsManager::getGraphicsInterface(GraphicsManager::VULKAN);
    
    ::screen_config(); // 获取屏幕信息

    ::native_window_screen_x = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::native_window_screen_y = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::abs_ScreenX = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::abs_ScreenY = (::displayInfo.height < ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);

    ::window = android::ANativeWindowCreator::Create("test", native_window_screen_x, native_window_screen_y, permeate_record);
    graphics->Init_Render(::window, native_window_screen_x, native_window_screen_y);
    ::init_My_drawdata(); // 初始化绘制数据

    Touch::Init({(float)::abs_ScreenX, (float)::abs_ScreenY}, false);
    Touch::setOrientation(displayInfo.orientation);

    static bool flag = true;

    NumIoLoad("LV内核0.12");
    ImVec2 MenuVe2 = ImVec2(0, 0);
    
    FPS限制.SetFps(globalConfig.FPS);
    FPS限制.AotuFPS_init();
    FPS限制.setAffinity();

    // 启动音量键监听线程
    std::thread 音量键线程(音量键隐藏);
    音量键线程.detach(); // 分离线程，使其独立运行

    while (flag) {
        drawBegin();
        graphics->NewFrame();
        
        Menu_UI();
        if (yes == 10086) {
            if (::displayInfo.orientation == 1 || ::displayInfo.orientation == 3) {
                MenuVe2.x = abs_ScreenX;
                MenuVe2.y = abs_ScreenY;
            } else {
                MenuVe2.x = abs_ScreenY;
                MenuVe2.y = abs_ScreenX;
            }
            
            Draw_UI(MenuVe2, ImGui::GetBackgroundDrawList());
        }
        graphics->EndFrame();
        FPS限制.SetFps(globalConfig.FPS);
        FPS限制.AotuFPS();
    }
    
    // 关闭触摸系统
    Touch::Close();
    graphics->Shutdown();
    android::ANativeWindowCreator::Destroy(::window);
    return 0;
}
