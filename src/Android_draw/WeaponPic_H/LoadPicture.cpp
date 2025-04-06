#include "Draw_ESP.h"
#include "draw.h"
//#include "人物倒地.h"
#include "警告.h"
#include "ID0.h"
#include "ID101001.h"
#include "ID101002.h"
#include "ID101003.h"
#include "ID101004.h"
#include "ID101005.h"
#include "ID101006.h"
#include "ID101007.h"
#include "ID101008.h"
#include "ID101009.h"
#include "ID101010.h"
#include "ID101011.h"
#include "ID101012.h"
#include "ID102001.h"
#include "ID102002.h"
#include "ID102003.h"
#include "ID102004.h"
#include "ID102005.h"
#include "ID102007.h"
#include "ID102105.h"
#include "ID103001.h"
#include "ID103002.h"
#include "ID103003.h"
#include "ID103004.h"
#include "ID103005.h"
#include "ID103006.h"
#include "ID103007.h"
#include "ID103008.h"
#include "ID103009.h"
#include "ID103010.h"
#include "ID103011.h"
#include "ID103012.h"
#include "ID103013.h"
#include "ID103014.h"
#include "ID103015.h"
#include "ID103100.h"
#include "ID103901.h"
#include "ID103902.h"
#include "ID103903.h"
#include "ID104001.h"
#include "ID104002.h"
#include "ID104003.h"
#include "ID104004.h"
#include "ID104100.h"
#include "ID105001.h"
#include "ID105002.h"
#include "ID105010.h"
#include "ID106001.h"
#include "ID106002.h"
#include "ID106003.h"
#include "ID106004.h"
#include "ID106005.h"
#include "ID106006.h"
#include "ID106007.h"
#include "ID106008.h"
#include "ID106010.h"
#include "ID107001.h"
#include "ID107006.h"
#include "ID107007.h"
#include "ID107986.h"
#include "ID108001.h"
#include "ID108002.h"
#include "ID108003.h"
#include "ID108004.h"
#include "ID602001.h"
#include "ID602002.h"
#include "ID602003.h"
#include "ID602004.h"
#include "ID602011.h"
#include "ID602030.h"
#include "ID602041.h"
#include "ID602042.h"
#include "ID602043.h"
#include "ID812146.h"
#include "ID812147.h"
#include "ID812148.h"
#include "ID812149.h"


void LoadPic() {
    WeaponPic[98] = graphics->LoadTextureFromMemory((void *)警告_PNG, sizeof(警告_PNG));
    WeaponPic[0] = graphics->LoadTextureFromMemory((void *)ID0_PNG, sizeof(ID0_PNG));
    WeaponPic[1] = graphics->LoadTextureFromMemory((void *)ID101001_PNG, sizeof(ID101001_PNG));
    WeaponPic[2] = graphics->LoadTextureFromMemory((void *)ID101002_PNG, sizeof(ID101002_PNG));
    WeaponPic[3] = graphics->LoadTextureFromMemory((void *)ID101003_PNG, sizeof(ID101003_PNG));
    WeaponPic[4] = graphics->LoadTextureFromMemory((void *)ID101004_PNG, sizeof(ID101004_PNG));
    WeaponPic[5] = graphics->LoadTextureFromMemory((void *)ID101005_PNG, sizeof(ID101005_PNG));
    WeaponPic[6] = graphics->LoadTextureFromMemory((void *)ID101006_PNG, sizeof(ID101006_PNG));
    WeaponPic[7] = graphics->LoadTextureFromMemory((void *)ID101007_PNG, sizeof(ID101007_PNG));
    WeaponPic[8] = graphics->LoadTextureFromMemory((void *)ID101008_PNG, sizeof(ID101008_PNG));
    WeaponPic[9] = graphics->LoadTextureFromMemory((void *)ID101009_PNG, sizeof(ID101009_PNG));
    WeaponPic[10] = graphics->LoadTextureFromMemory((void *)ID101010_PNG, sizeof(ID101010_PNG));
    WeaponPic[11] = graphics->LoadTextureFromMemory((void *)ID101011_PNG, sizeof(ID101011_PNG));
    WeaponPic[12] = graphics->LoadTextureFromMemory((void *)ID101012_PNG, sizeof(ID101012_PNG));
    WeaponPic[13] = graphics->LoadTextureFromMemory((void *)ID102001_PNG, sizeof(ID102001_PNG));
    WeaponPic[14] = graphics->LoadTextureFromMemory((void *)ID102002_PNG, sizeof(ID102002_PNG));
    WeaponPic[15] = graphics->LoadTextureFromMemory((void *)ID102003_PNG, sizeof(ID102003_PNG));
    WeaponPic[16] = graphics->LoadTextureFromMemory((void *)ID102004_PNG, sizeof(ID102004_PNG));
    WeaponPic[17] = graphics->LoadTextureFromMemory((void *)ID102005_PNG, sizeof(ID102005_PNG));
    WeaponPic[18] = graphics->LoadTextureFromMemory((void *)ID102007_PNG, sizeof(ID102007_PNG));
    WeaponPic[19] = graphics->LoadTextureFromMemory((void *)ID102105_PNG, sizeof(ID102105_PNG));
    WeaponPic[20] = graphics->LoadTextureFromMemory((void *)ID103001_PNG, sizeof(ID103001_PNG));
    WeaponPic[21] = graphics->LoadTextureFromMemory((void *)ID103002_PNG, sizeof(ID103002_PNG));
    WeaponPic[22] = graphics->LoadTextureFromMemory((void *)ID103003_PNG, sizeof(ID103003_PNG));
    WeaponPic[23] = graphics->LoadTextureFromMemory((void *)ID103004_PNG, sizeof(ID103004_PNG));
    WeaponPic[24] = graphics->LoadTextureFromMemory((void *)ID103005_PNG, sizeof(ID103005_PNG));
    WeaponPic[25] = graphics->LoadTextureFromMemory((void *)ID103006_PNG, sizeof(ID103006_PNG));
    WeaponPic[26] = graphics->LoadTextureFromMemory((void *)ID103007_PNG, sizeof(ID103007_PNG));
    WeaponPic[27] = graphics->LoadTextureFromMemory((void *)ID103008_PNG, sizeof(ID103008_PNG));
    WeaponPic[28] = graphics->LoadTextureFromMemory((void *)ID103009_PNG, sizeof(ID103009_PNG));
    WeaponPic[29] = graphics->LoadTextureFromMemory((void *)ID103010_PNG, sizeof(ID103010_PNG));
    WeaponPic[30] = graphics->LoadTextureFromMemory((void *)ID103011_PNG, sizeof(ID103011_PNG));
    WeaponPic[31] = graphics->LoadTextureFromMemory((void *)ID103012_PNG, sizeof(ID103012_PNG));
    WeaponPic[32] = graphics->LoadTextureFromMemory((void *)ID103013_PNG, sizeof(ID103013_PNG));
    WeaponPic[33] = graphics->LoadTextureFromMemory((void *)ID103014_PNG, sizeof(ID103014_PNG));
    WeaponPic[34] = graphics->LoadTextureFromMemory((void *)ID103015_PNG, sizeof(ID103015_PNG));
    WeaponPic[35] = graphics->LoadTextureFromMemory((void *)ID103100_PNG, sizeof(ID103100_PNG));
    WeaponPic[36] = graphics->LoadTextureFromMemory((void *)ID103901_PNG, sizeof(ID103901_PNG));
    WeaponPic[37] = graphics->LoadTextureFromMemory((void *)ID103902_PNG, sizeof(ID103902_PNG));
    WeaponPic[38] = graphics->LoadTextureFromMemory((void *)ID103903_PNG, sizeof(ID103903_PNG));
    WeaponPic[39] = graphics->LoadTextureFromMemory((void *)ID104001_PNG, sizeof(ID104001_PNG));
    WeaponPic[40] = graphics->LoadTextureFromMemory((void *)ID104002_PNG, sizeof(ID104002_PNG));
    WeaponPic[41] = graphics->LoadTextureFromMemory((void *)ID104003_PNG, sizeof(ID104003_PNG));
    WeaponPic[42] = graphics->LoadTextureFromMemory((void *)ID104004_PNG, sizeof(ID104004_PNG));
    WeaponPic[43] = graphics->LoadTextureFromMemory((void *)ID104100_PNG, sizeof(ID104100_PNG));
    WeaponPic[44] = graphics->LoadTextureFromMemory((void *)ID105001_PNG, sizeof(ID105001_PNG));
    WeaponPic[45] = graphics->LoadTextureFromMemory((void *)ID105002_PNG, sizeof(ID105002_PNG));
    WeaponPic[46] = graphics->LoadTextureFromMemory((void *)ID105010_PNG, sizeof(ID105010_PNG));
    WeaponPic[47] = graphics->LoadTextureFromMemory((void *)ID106001_PNG, sizeof(ID106001_PNG));
    WeaponPic[48] = graphics->LoadTextureFromMemory((void *)ID106002_PNG, sizeof(ID106002_PNG));
    WeaponPic[49] = graphics->LoadTextureFromMemory((void *)ID106003_PNG, sizeof(ID106003_PNG));
    WeaponPic[50] = graphics->LoadTextureFromMemory((void *)ID106004_PNG, sizeof(ID106004_PNG));
    WeaponPic[51] = graphics->LoadTextureFromMemory((void *)ID106005_PNG, sizeof(ID106005_PNG));
    WeaponPic[52] = graphics->LoadTextureFromMemory((void *)ID106006_PNG, sizeof(ID106006_PNG));
    WeaponPic[53] = graphics->LoadTextureFromMemory((void *)ID106007_PNG, sizeof(ID106007_PNG));
    WeaponPic[54] = graphics->LoadTextureFromMemory((void *)ID106008_PNG, sizeof(ID106008_PNG));
    WeaponPic[55] = graphics->LoadTextureFromMemory((void *)ID106010_PNG, sizeof(ID106010_PNG));
    WeaponPic[56] = graphics->LoadTextureFromMemory((void *)ID107001_PNG, sizeof(ID107001_PNG));
    WeaponPic[57] = graphics->LoadTextureFromMemory((void *)ID107006_PNG, sizeof(ID107006_PNG));
    WeaponPic[58] = graphics->LoadTextureFromMemory((void *)ID107007_PNG, sizeof(ID107007_PNG));
    WeaponPic[59] = graphics->LoadTextureFromMemory((void *)ID107986_PNG, sizeof(ID107986_PNG));
    WeaponPic[60] = graphics->LoadTextureFromMemory((void *)ID108001_PNG, sizeof(ID108001_PNG));
    WeaponPic[61] = graphics->LoadTextureFromMemory((void *)ID108002_PNG, sizeof(ID108002_PNG));
    WeaponPic[62] = graphics->LoadTextureFromMemory((void *)ID108003_PNG, sizeof(ID108003_PNG));
    WeaponPic[63] = graphics->LoadTextureFromMemory((void *)ID108004_PNG, sizeof(ID108004_PNG));
    WeaponPic[64] = graphics->LoadTextureFromMemory((void *)ID602001_PNG, sizeof(ID602001_PNG));
    WeaponPic[65] = graphics->LoadTextureFromMemory((void *)ID602002_PNG, sizeof(ID602002_PNG));
    WeaponPic[66] = graphics->LoadTextureFromMemory((void *)ID602003_PNG, sizeof(ID602003_PNG));
    WeaponPic[67] = graphics->LoadTextureFromMemory((void *)ID602004_PNG, sizeof(ID602004_PNG));
    WeaponPic[68] = graphics->LoadTextureFromMemory((void *)ID602011_PNG, sizeof(ID602011_PNG));
    WeaponPic[69] = graphics->LoadTextureFromMemory((void *)ID602030_PNG, sizeof(ID602030_PNG));
    WeaponPic[70] = graphics->LoadTextureFromMemory((void *)ID602041_PNG, sizeof(ID602041_PNG));
    WeaponPic[71] = graphics->LoadTextureFromMemory((void *)ID602042_PNG, sizeof(ID602042_PNG));
    WeaponPic[72] = graphics->LoadTextureFromMemory((void *)ID602043_PNG, sizeof(ID602043_PNG));
    WeaponPic[73] = graphics->LoadTextureFromMemory((void *)ID812146_PNG, sizeof(ID812146_PNG));
    WeaponPic[74] = graphics->LoadTextureFromMemory((void *)ID812147_PNG, sizeof(ID812147_PNG));
    WeaponPic[75] = graphics->LoadTextureFromMemory((void *)ID812148_PNG, sizeof(ID812148_PNG));
    WeaponPic[76] = graphics->LoadTextureFromMemory((void *)ID812149_PNG, sizeof(ID812149_PNG));
}

