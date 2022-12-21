#include "boardtype_friendlyelec.h"
#include <ctype.h>

const char* s5p_board_cputempfile = "/sys/class/hwmon/hwmon0/device/temp_label";
const char* s5p_board_max_cputempfile = "/sys/class/hwmon/hwmon0/device/temp_max";
const char* allwinner_tempfile = "/sys/class/thermal/thermal_zone0/temp";

#define LOGD printf
#define LOGE printf

BoardHardwareInfo gAllBoardHardwareInfo[] = {
    {"Allwinnersun8iFamily", 0, NanoPi_NEO_Air, "NanoPi-NEO-Air", "2(0)"},
};

static int getFieldValueInCpuInfo(char* hardware, int hardwareMaxLen, char* revision, int revisionMaxLen )
{
    int n,i,j;
    char lineUntrim[1024], line[1024],line2[1024],*p,*p2;
    FILE *f;
    int isGotHardware = 0;
    int isGotRevision = 0;

    if (!(f = fopen("/sys/devices/platform/board/info", "r"))) {
        if (!(f = fopen("/proc/cpuinfo", "r"))) {
            LOGE("open /proc/cpuinfo failed.");
            return -1;
        }
    }

    while (!feof(f)) {
        if(!fgets(lineUntrim, sizeof(lineUntrim), f)) {
            break;
        } else {
            j=0;
            for(i=0; i<strlen(lineUntrim);i++) {
                if (lineUntrim[i] == ' ' || lineUntrim[i] == '\t' || lineUntrim[i] == '\r' || lineUntrim[i] == '\n') {
                } else {
                    line[j++]=lineUntrim[i];
                }
            }
            line[j] = 0x00;
            n = strlen(line);
            if (n>0) {
                //LOGD("LINE: %s\n", line);
                #define GetKeyValue(isGot,valP,keyName,buff,buffLen) \
                if (isGot==0) { \
                    strcpy(line2, line); \
                    if (valP=strtok(line2, ":")) { \
                        if (strncasecmp(valP,keyName,strlen(keyName))==0) { \
                            if (valP=strtok(0, ":")) { \
                                memset(buff,0,buffLen); \
                                strncpy(buff,valP,buffLen-1); \
                                isGot=1; \
                            } \
                            continue; \
                        } \
                    } \
                }
                GetKeyValue(isGotHardware,p,"Hardware",hardware,hardwareMaxLen);
                GetKeyValue(isGotRevision,p2,"Revision",revision,revisionMaxLen);

                if (isGotHardware == 1 && isGotRevision == 1) {
                    break;
                }
            }
        }
    }
    fclose(f);
    return isGotHardware + isGotRevision;
}


static int getAllwinnerBoardID(char* boardId, int boardIdMaxLen )
{
    int n,i,j;
    char lineUntrim[1024], line[1024],*p;
    const char* sunxi_board_id_fieldname = "sunxi_board_id";
    FILE *f;
    int ret = -1;

    if (!(f = fopen("/sys/class/sunxi_info/sys_info", "r"))) {
        LOGE("open /sys/class/sunxi_info/sys_info failed.");
        return -1;
    }

    while (!feof(f)) {
        if(!fgets(lineUntrim, sizeof(lineUntrim), f)) {
            break;
        } else {
            j=0;
            for(i=0; i<strlen(lineUntrim);i++) {
                if (lineUntrim[i] == ' ' || lineUntrim[i] == '\t' || lineUntrim[i] == '\r' || lineUntrim[i] == '\n') {
                } else {
                    line[j++]=lineUntrim[i];
                }
            }
            line[j] = 0x00;
            n = strlen(line);
            if (n>0) {
                //LOGD("LINE: %s\n", line);
                if (p = strtok(line, ":")) {
                    if (strncasecmp(p, sunxi_board_id_fieldname, strlen(sunxi_board_id_fieldname)) == 0) {
                        //LOGD("\t\tkey=\"%s\"\n", p);
                        if (p = strtok(0, ":")) {
                            //LOGD("\t\tv=\"%s\"\n", p);
                            memset(boardId,0,boardIdMaxLen);
                            strncpy(boardId, p, boardIdMaxLen-1);
                            ret = 0;
                            break;
                        }
                    }
                }  
            }
        }
    }
    fclose(f);
    return ret;
}

int getBoardType(BoardHardwareInfo** retBoardInfo) {
    return gAllBoardHardwareInfo[0].boardTypeId;
}

/*
int main() {
    BoardHardwareInfo* retBoardInfo;
    int boardId;
    boardId = getBoardType(&retBoardInfo);
    if (boardId >= 0) {
        printf("boardName:%s,boardId:%d\n", retBoardInfo->boardDisplayName, boardId);
    } else {
        printf("%s, ret:%d\n", "failed", boardId);
    }

    return 0;
}
*/
