#include <stdint.h>

#include "board.h"
#include "lcd_init.h"
#include "lcd.h"
#include "servo_controller.h"

#define ACTION_DELAY_MS       (700U)
#define ACTION_REPEAT_DELAY_MS (1200U)

typedef void (*ActionFn)(void);

typedef struct {
    const char *name;
    ActionFn run;
} MotorDebugAction;

static void App_InitForMotorDebug(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_init();
    SYSCFG_DL_PWM_LEG_FL_init();
    SYSCFG_DL_PWM_LEG_FR_init();
    SYSCFG_DL_PWM_LEG_BL_init();
    SYSCFG_DL_PWM_LEG_BR_init();
    SYSCFG_DL_SYSTICK_init();
}

static void LCD_ShowHeader(void)
{
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    LCD_ShowString(0, 8, (const uint8_t *) "MOTOR DEBUG", RED, WHITE, 16, 0);
    LCD_ShowString(0, 28, (const uint8_t *) "PB23 RESERVED", BLUE, WHITE, 16, 0);
    LCD_ShowString(0, 48, (const uint8_t *) "PWM: A28 A15 A1", BLACK, WHITE, 16, 0);
    LCD_ShowString(0, 68, (const uint8_t *) "     B6  A0", BLACK, WHITE, 16, 0);
}

static void LCD_ShowAction(const char *name)
{
    LCD_Fill(0, 96, LCD_W, 150, WHITE);
    LCD_ShowString(0, 96, (const uint8_t *) "ACTION:", BLUE, WHITE, 16, 0);
    LCD_ShowString(0, 116, (const uint8_t *) name, GREEN, WHITE, 16, 0);
}

static void LCD_ShowAngles(void)
{
    LCD_Fill(0, 160, LCD_W, 224, WHITE);
    LCD_ShowString(0, 160, (const uint8_t *) "H:", BLACK, WHITE, 16, 0);
    LCD_ShowIntNum(24, 160, ServoController_GetAngle(SERVO_MOTOR_HEAD), 3,
        BLACK, WHITE, 16);
    LCD_ShowString(72, 160, (const uint8_t *) "FL:", BLACK, WHITE, 16, 0);
    LCD_ShowIntNum(112, 160, ServoController_GetAngle(SERVO_MOTOR_LEG_FL), 3,
        BLACK, WHITE, 16);

    LCD_ShowString(0, 180, (const uint8_t *) "FR:", BLACK, WHITE, 16, 0);
    LCD_ShowIntNum(40, 180, ServoController_GetAngle(SERVO_MOTOR_LEG_FR), 3,
        BLACK, WHITE, 16);
    LCD_ShowString(88, 180, (const uint8_t *) "BL:", BLACK, WHITE, 16, 0);
    LCD_ShowIntNum(128, 180, ServoController_GetAngle(SERVO_MOTOR_LEG_BL), 3,
        BLACK, WHITE, 16);

    LCD_ShowString(0, 200, (const uint8_t *) "BR:", BLACK, WHITE, 16, 0);
    LCD_ShowIntNum(40, 200, ServoController_GetAngle(SERVO_MOTOR_LEG_BR), 3,
        BLACK, WHITE, 16);
}

static void RunAction(const MotorDebugAction *action)
{
    LCD_ShowAction(action->name);
    delay_ms(ACTION_DELAY_MS);
    action->run();
    LCD_ShowAngles();
    delay_ms(ACTION_REPEAT_DELAY_MS);
}

static void Action_Stand(void)
{
    ServoController_SetStandPose();
}

static void Action_HeadLeft(void)
{
    Pony_TurnHeadLeft();
}

static void Action_HeadRight(void)
{
    Pony_TurnHeadRight();
}

static void Action_HeadCenter(void)
{
    Pony_TurnHeadCenter();
}

static void Action_Forward(void)
{
    Pony_Forward();
}

static void Action_Backward(void)
{
    Pony_Backward();
}

static void Action_MoveForward2(void)
{
    Pony_MoveForward(2U);
}

static void Action_MoveBackward2(void)
{
    Pony_MoveBackward(2U);
}

static void Action_TurnLeft(void)
{
    Pony_TurnLeft();
}

static void Action_TurnRight(void)
{
    Pony_TurnRight();
}

static void Action_TurnLeft2(void)
{
    Pony_TurnLeftCycles(2U);
}

static void Action_TurnRight2(void)
{
    Pony_TurnRightCycles(2U);
}

static void Action_Posture1(void)
{
    Pony_Posture1();
}

static void Action_Posture2(void)
{
    Pony_Posture2();
}

static const MotorDebugAction g_actions[] = {
    {"STAND", Action_Stand},
    {"HEAD LEFT", Action_HeadLeft},
    {"HEAD RIGHT", Action_HeadRight},
    {"HEAD CENTER", Action_HeadCenter},
    {"FORWARD", Action_Forward},
    {"BACKWARD", Action_Backward},
    {"MOVE FW x2", Action_MoveForward2},
    {"MOVE BW x2", Action_MoveBackward2},
    {"TURN LEFT", Action_TurnLeft},
    {"TURN RIGHT", Action_TurnRight},
    {"TURN LEFT x2", Action_TurnLeft2},
    {"TURN RIGHT x2", Action_TurnRight2},
    {"POSTURE 1", Action_Posture1},
    {"POSTURE 2", Action_Posture2},
};

int main(void)
{
    uint32_t i;

    App_InitForMotorDebug();

    LCD_BLK_Set();
    LCD_Init();
    LCD_ShowHeader();

    LCD_ShowAction("INIT STAND");
    ServoController_Init();
    LCD_ShowAngles();
    delay_ms(ACTION_REPEAT_DELAY_MS);

    while (1) {
        for (i = 0U; i < (sizeof(g_actions) / sizeof(g_actions[0])); i++) {
            RunAction(&g_actions[i]);
        }
    }
}
