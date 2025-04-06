#ifndef MY_VERIFY_S_H
#define MY_VERIFY_S_H


#include <stdio.h>
#include <stdlib.h>


struct all_tinyCheck {
    int Login_code = 0;
    char APPID[32] = {0};
    char APPKEY[64] = {0};
    char RC4KEY[64] = {0};
    char imei_[128] = {0};
    char key_[128] = {0};
};

//获取数据
extern all_tinyCheck* get_all_tinyCheckData();
//登录验证
extern int wbfd3408c915617b54992c2adf5c9127a(all_tinyCheck *all_check, int *yes, char *ret_err);
//登陆验证(封装可执行文件)
extern void 微验验证(int *yes, char *str_err);
//解绑卡密
extern void bbsjs4646ekkdbdbdbdb(all_tinyCheck *all_check, char *ret_err); 
//获取公告
extern int get_Announcement_data(all_tinyCheck *all_check, char *app_gg); 
//获取到期时间
extern time_t get_Maturity_time();
//心跳线程
extern void *Circular_authentication(void *arg); 

//get_
extern char *getAndroid_boot_serialno();
//get_
extern char* getMacAddresses(const char *str_name);

extern bool loadConfig_ver(char *cfgPath, void *data, size_t size);
extern bool saveConfig_ver(char *cfgPath, void *data, size_t size);


#endif //MY_VERIFY_S_H








