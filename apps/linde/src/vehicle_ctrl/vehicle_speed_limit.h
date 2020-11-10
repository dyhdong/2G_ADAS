#ifndef __VEHICLE_SPEED_LIMIT_H__
#define __VEHICLE_SPEED_LIMIT_H__

#include <stdint.h>
#include <stdbool.h>
#include <kernel.h>
#define OPEN_LIMT 1		//打开限速的状态
#define CLOSE_LIMT 0	//关闭限速的状态
typedef struct{
	int state;
}speedLimitFile_t;

bool vehclSpeedLimitSetup();
bool vehclSpeedLimitOpen();
bool vehclSpeedLimitClose();
char vehclSpeedLimitState();

#endif

