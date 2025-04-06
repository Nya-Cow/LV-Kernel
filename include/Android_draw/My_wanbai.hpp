#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <locale>
#include <malloc.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <thread>
#include <unistd.h>
#include "shm_open_anon.h"
#include <mutex>
#include <regex.h>

class c_driver {
    private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    std::mutex mutex[8];
    int fd;
    pid_t pid;

    typedef struct _COPY_MEMORY
    {
        pid_t pid;
        uintptr_t addr;
        void *buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _MODULE_BASE
    {
        pid_t pid;
        char *name;
        uintptr_t base;
    } MODULE_BASE, *PMODULE_BASE;

    enum OPERATIONS
    {
        OP_INIT_KEY = 0x800,
        OP_READ_MEM = 0x801,
        OP_WRITE_MEM = 0x802,
        OP_MODULE_BASE = 0x803,
    };

    int symbol_file(const char *filename)
    {
        int length = strlen(filename);

        for (int i = 0; i < length; i++)
        {
            if (isupper(filename[i]))
            {
                has_upper = 1;
            }
            else if (islower(filename[i]))
            {
                has_lower = 1;
            }
            else if (ispunct(filename[i]))
            {
                has_symbol = 1;
            }
            else if (isdigit(filename[i]))
            {
                has_digit = 1;
            }
        }

        return has_upper && has_lower && !has_symbol && !has_digit;
    }

    char *execCom(const char *shell)
    {
        FILE *fp = popen(shell, "r");

        if (fp == NULL)
        {
            perror("popen failed");
            return NULL;
        }

        char buffer[256];
        char *result = (char *)malloc(1000); // allocate memory for the result string
        result[0] = '\0';                    // initialize as an empty string

        // Read and append output of the first command to result
        while (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            strcat(result, buffer);
        }
        pclose(fp);
        return result;
    }

    int findFirstMatchingPath(const char *path, regex_t *regex, char *result)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(path)) != NULL)
        {
            while ((entry = readdir(dir)) != NULL)
            {
                char fullpath[1024]; // 适当调整数组大小
                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
                if (entry->d_type == DT_LNK)
                {
                    // 对链接文件进行处理
                    char linkpath[1024]; // 适当调整数组大小
                    ssize_t len = readlink(fullpath, linkpath, sizeof(linkpath) - 1);
                    if (len != -1)
                    {
                        linkpath[len] = '\0';
                        // printf("%s\n", linkpath);
                        // 对链接的实际路径进行正则匹配
                        if (regexec(regex, linkpath, 0, NULL, 0) == 0)
                        {
                            strcpy(result, fullpath);
                            closedir(dir);
                            return 1;
                        }
                    }
                    else
                    {
                        perror("readlink");
                    }
                }
            }
            closedir(dir);
        }
        else
        {
            perror("Unable to open directory");
        }

        return 0;
    }

// 修复createDriverNode和removeDeviceNode函数
  void createDriverNode(char *path, int major_number, int minor_number)
  {
    std::string command = "mknod " + std::string(path) + " c " + std::to_string(major_number) + " " + std::to_string(minor_number);
    system(command.c_str());
    printf("[-]创建 %s\n[-]主设备号：%d\n[-]次设备号：%d\n", path, major_number, minor_number);
  }

  // 删除驱动节点
  // 新的函数，用于删除设备节点
  void removeDeviceNode(char* path) {
    printf("%s\n",path);
    if (unlink(path) == 0) {
      printf("[-]驱动安全守护已激活\n");
       //cerr << "已删除设备节点：" << devicePath << endl;
    } else {
      printf("[-]驱动安全守护执行错误\n");
      // perror("删除设备节点时发生错误");
    }
  }
  
  public:  
	const char* get_dev() {
    const char* command = "for dir in /proc/*/; do cmdline_file=\"cmdline\"; comm_file=\"comm\"; proclj=\"$dir$cmdline_file\"; proclj2=\"$dir$comm_file\"; if [[ -f \"$proclj\" && -f \"$proclj2\" ]]; then cmdline=$(head -n 1 \"$proclj\"); comm=$(head -n 1 \"$proclj2\"); if echo \"$cmdline\" | grep -qE '^/data/[a-z]{6}$'; then sbwj=$(echo \"$comm\"); open_file=\"\"; for file in \"$dir\"/fd/*; do link=$(readlink \"$file\"); if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\"; break; fi; done; if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\"); sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //'); echo \"/dev/$sbwj\"; rm -Rf \"/dev/$sbwj\"; mknod \"/dev/$sbwj\" c \"$sbid\" 0; break; fi; fi; fi; done";
    FILE* file = popen(command, "r");
    if (file == NULL) {
        return NULL;
    }

    char result[512];
    if (fgets(result, sizeof(result), file) == NULL) {
        pclose(file);
        return NULL;
    }
    pclose(file);

    int len = strlen(result);
    if (len > 0 && result[len - 1] == '\n') {
        result[len - 1] = '\0';
    }
    return strdup(result);
    }
    
char *gtqwq() {
	// 打开目录
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL){
			printf("无法打开/dev目录\n");
			return NULL;
		}

		const char *files[] = { "wanbai", "CheckMe", "Ckanri", "lanran","video188"};
		struct dirent *entry;
		char *gtfile_path = NULL;
		while ((entry = readdir(dir)) != NULL) {
			// 跳过当前目录和上级目录
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
			gtfile_path = (char *)malloc(path_length);
			snprintf(gtfile_path, path_length, "%s/%s", dev_path, entry->d_name);
			for (int i = 0; i < 5; i++) {
				if (strcmp(entry->d_name, files[i]) == 0) {
					printf("驱动文件：%s\n", gtfile_path);
					closedir(dir);
					return gtfile_path;
				}
			}

			// 获取文件stat结构
			struct stat file_info;
			if (stat(gtfile_path, &file_info) < 0) {
				free(gtfile_path);
				gtfile_path = NULL;
				continue;
			}

			// 跳过gpio接口
			if (strstr(entry->d_name, "gpiochip") != NULL) {
				free(gtfile_path);
				gtfile_path = NULL;
				continue;
			}

			// 检查是否为驱动文件
			if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
				&& strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
				// 过滤标准输入输出
				if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0
					|| strcmp(entry->d_name, "stderr") == 0) {
					free(gtfile_path);
					gtfile_path = NULL;
					continue;
				}
				
				size_t file_name_length = strlen(entry->d_name);
				time_t current_time;
				time(&current_time);
				int current_year = localtime(&current_time)->tm_year + 1900;
				int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
				//跳过1980年前的文件
				if (file_year <= 1980) {
					free(gtfile_path);
					gtfile_path = NULL;
					continue;
				}
				
				time_t atime = file_info.st_atime;
				time_t ctime = file_info.st_ctime;
				// 检查最近访问时间和修改时间是否一致并且文件名是否是symbol文件
				if ((atime = ctime)/* && symbol_file(entry->d_name)*/) {
					//检查mode权限类型是否为S_IFREG(普通文件)和大小还有gid和uid是否为0(root)并且文件名称长度在7位或7位以下
					if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0
						&& file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
						printf("驱动文件：%s\n", gtfile_path);
						closedir(dir);
						return gtfile_path;
					}
				}
			}
			free(gtfile_path);
			gtfile_path = NULL;
		}
		closedir(dir);
		return NULL;
	}
	
    c_driver(){
printf("[+]1--------QX驱动1(读取慢,兼容性高)"); 
printf("\n[+]2--------GT驱动");
printf("\n[+]3--------QX驱动2");
int 选择值;
printf("\n[-]请输入序号");
scanf("%d",&选择值);
    if (选择值 ==1){
	char dev_path[64];
	strcpy(dev_path,get_dev());
	fd = open(dev_path, O_RDWR);
	if (fd>0){
	printf("驱动文件：%s\n", dev_path);
	unlink(dev_path);
	//return 0;
	} else {
	printf("无法找到驱动文件！\n");
	exit(0);
	//return -1;
	}	
	}
	if (选择值 ==2){
	char *device_name = gtqwq();
	fd = open(device_name, O_RDWR);
	if (fd == -1) {
			printf("[-] 打开驱动程序失败，请重新刷入gt驱动\n");
			free(device_name);
			exit(0);
		}
		free(device_name);
	}
    
	if (选择值 ==3){
	char *output = execCom("ls -l /proc/*/exe 2>/dev/null | grep -E \"/data/[a-z]{6} \\(deleted\\)\"");
        char filePath[256];
        char pid[56];
        if (output != NULL) {
        printf("调试输出:%s", output);
        char *procStart = strstr(output, "/proc/");

        // Extracting process ID
        char *pidStart = procStart + 6; // Move to the position after "/proc/"
        char *pidEnd = strchr(pidStart, '/');

        strncpy(pid, pidStart, pidEnd - pidStart);
        pid[pidEnd - pidStart] = '\0';
        
        char *arrowStart = strstr(output, "->");
        // Extracting file path
        char *start = arrowStart + 3; // Move to the position after "->"
        char *end = strchr(output, '(') - 1; // Find the position before '('
        strncpy(filePath, start, end - start + 1);
        filePath[end - start] = '\0';

        // Replace "data" with "dev" in filePath
		
        char *replacePtr = strstr(filePath, "data");
		   
        if (replacePtr != NULL ) {
            memmove(replacePtr + 2, replacePtr + 3, strlen(replacePtr + 3) + 1);
            memmove(replacePtr, "dev", strlen("dev"));
        }
		 
        } else {
            printf("执行脚本时出错\n");
        }
       
        char cmd[256];
        // Construct the command to get fdInfo using the extracted pid
        sprintf(cmd, "ls -al -L /proc/%s/fd/3", pid);
        // Execute the command and get fdInfo
        char *fdInfo = execCom(cmd);
        int major_number, minor_number;
        sscanf(fdInfo, "%*s %*d %*s %*s %d, %d", &major_number, &minor_number);
        

        if (strcmp(filePath, "0") != 0) {
		createDriverNode(filePath, major_number, minor_number);
      }
         sleep(1);
		char *search2 = filePath;
        fd = open(search2, O_RDWR); // Use c_str() to get a C-style string
		 if (fd == -1) {
		 printf("\033[31m[X] 驱动连接失败 \033[0m\n");
	   //  removeDeviceNode(filePath);		      
		  exit(0);
            } else {
               printf("%s", "[+]链接成功\n");
			   printf("[-] 驱动文件：%s\n",filePath);
                removeDeviceNode(filePath);
            }
	      }
	}		
	
~c_driver() {
        //wont be called
        if (fd > 0)
            close(fd);
    }


    void initialize(pid_t pid) {
        std::lock_guard<std::mutex> lock(mutex[0]); // 加锁
        this->pid = pid;
    }


    bool read(uintptr_t addr, void *buffer, size_t size) {
        std::lock_guard<std::mutex> lock(mutex[1]); // 加锁
        COPY_MEMORY cm;

        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    bool write(uintptr_t addr, void *buffer, size_t size) {
        std::lock_guard<std::mutex> lock(mutex[2]); // 加锁
        COPY_MEMORY cm;

        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    template<typename T>
    T read(uintptr_t addr) {
        std::lock_guard<std::mutex> lock(mutex[3]); // 加锁
        T res;
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return{};
    }

    template<typename T>
    bool write(uintptr_t addr, T value) {
        std::lock_guard<std::mutex> lock(mutex[4]); // 加锁
        return this->write(addr, &value, sizeof(T));
    }
    
      


    pid_t get_name_pid(char *name) {
        std::lock_guard<std::mutex> lock(mutex[5]); // 加锁
        FILE *fp;
        pid_t pid;
        char cmd[0x100] = "pidof ";

        strcat(cmd, name);
        fp = popen(cmd, "r");
        fscanf(fp, "%d", &pid);
        pclose(fp);
        return pid;
    }

    uintptr_t get_module_base(char *name) {
        std::lock_guard<std::mutex> lock(mutex[6]); // 加锁
        MODULE_BASE mb;
        char buf[0x100];
        strcpy(buf, name);
        mb.pid = this->pid;
        mb.name = buf;

        if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) {
            return 0;
        }
        return mb.base;
    }
};
