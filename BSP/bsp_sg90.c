/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-08     LCKFB-LP    first version
 */
#include "bsp_sg90.h"

static unsigned int g_servo_angle = 0;

void Set_Servo_Angle(unsigned int angle)
{
    float min_count = 10.0f;
    float max_count = 50.0f;
    float servo_count;

    if (angle > 180U) {
        angle = 180U;
    }

    g_servo_angle = angle;
    servo_count   = min_count + (((float) angle / 180.0f) * (max_count - min_count));

    DL_TimerG_setCaptureCompareValue(
        PWM_INST, (uint32_t) (servo_count + 0.5f), GPIO_PWM_C0_IDX);
}

unsigned int Get_Servo_Angle(void)
{
    return g_servo_angle;
}
