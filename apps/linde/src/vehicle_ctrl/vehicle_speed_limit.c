#include "vehicle_speed_limit.h"
#include "my_misc.h"
#include <gpio.h>
#include <device.h>
#include <board.h>
#include <stdio.h>
#include <stdint.h>
#include "hw_gpio.h"


#define VEHICLE_LIMIT_VAL    1
#define VEHICLE_NOLIMIT_VAL  0
static struct device *g_vehclspeedlimitRelay;

bool vehclSpeedLimitSetup()
{
	
	g_vehclspeedlimitRelay = device_get_binding(GPIO_OUT_PORT);
   if (!g_vehclspeedlimitRelay) {
	 print_log("Cannot find %p!\n", g_vehclspeedlimitRelay);
	 return false;
   }else{
	  print_log("g_vehclspeedlimitRelay [%p]\n",g_vehclspeedlimitRelay);
   }
   if (gpio_pin_configure(g_vehclspeedlimitRelay, GPIO_OUT0_PIN, GPIO_DIR_OUT)) {
	  print_log("Configure g_vehclCtrlRelay failed");
	  return false;
   }
   //初始化设置为0
   if(gpio_pin_write(g_vehclspeedlimitRelay, GPIO_OUT0_PIN, VEHICLE_NOLIMIT_VAL))
    {
        return false;
    }
   /*
   g_vehclspeedlimitRelay = hwGpioPinInit(GPIO_OUT_PORT, 13, GPIO_DIR_OUT);
  	if (!g_vehclspeedlimitRelay) {
  		printk("GPIO %s PIN %d configure failed\n", GPIO_OUT_PORT, GPIO_OUT0_PIN);
  		return false;
  	}
  gpio_pin_write(g_vehclspeedlimitRelay, 13, 1);
  */
   return true;

}
bool vehclSpeedLimitOpen()
{
	if(gpio_pin_write(g_vehclspeedlimitRelay, GPIO_OUT0_PIN, VEHICLE_LIMIT_VAL))
    {
        return false;
    }
    return true;
}
bool vehclSpeedLimitClose()
{
	if(gpio_pin_write(g_vehclspeedlimitRelay, GPIO_OUT0_PIN, VEHICLE_NOLIMIT_VAL))
    {
        return false;
    }
    return true;
}
//返回：1 限速 0不限速
char vehclSpeedLimitState()
{
	char pinValue = 0;
    if(gpio_pin_read(g_vehclspeedlimitRelay,GPIO_OUT0_PIN,&pinValue))
    {
        return -1;
    }
    return pinValue;
}


