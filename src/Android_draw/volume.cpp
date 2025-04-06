#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <linux/input.h>
#include <sys/epoll.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_EVENTS 10

using namespace std;

extern bool show;

int volume() {
    DIR *dir = opendir("/dev/input/");
    if (!dir) {
        perror("Failed to open /dev/input directory");
        return -1;
    }

    struct dirent *ptr = NULL;
    int eventCount = 0;

    // 计算输入设备数量
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event"))
            eventCount++;
    }
    closedir(dir);

    if (eventCount == 0) {
        printf("未找到输入设备\n");
        return -1;
    }

    // 创建 epoll 实例
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        perror("Failed to create epoll instance");
        return -1;
    }

    // 打开所有输入设备并添加到 epoll
    for (int i = 0; i < eventCount; i++) {
        char temp[128];
        snprintf(temp, sizeof(temp), "/dev/input/event%d", i);
        int fd = open(temp, O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            perror("Failed to open input device");
            close(epollFd);
            return -1;
        }

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("Failed to add file descriptor to epoll");
            close(fd);
            close(epollFd);
            return -1;
        }
    }

    struct input_event ev;
    printf("Listening for volume key events...\n");
    
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            if (read(fd, &ev, sizeof(ev)) > 0) {
                if (ev.type == EV_KEY && (ev.code == KEY_VOLUMEUP || ev.code == KEY_VOLUMEDOWN)) {
                    if (ev.code == KEY_VOLUMEUP && ev.value == 1) {
                        show = true;    
                    } else if (ev.code == KEY_VOLUMEDOWN && ev.value == 1) {
                        show = false;
                    }
                }
            }
        }
    }

    close(epollFd);
    return 0;
}
