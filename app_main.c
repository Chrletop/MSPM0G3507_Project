#include <stdint.h>

#include "board.h"
#include "lcd_init.h"
#include "lcd.h"
#include "servo_controller.h"
#include "voice_i2c.h"

#define POLL_DELAY_MS        (100U)
#define ACTION_DONE_DELAY_MS (500U)
#define STATUS_REFRESH_TICKS (5U)

#define VOICE_ACTION_COUNT \
    ((uint32_t) (sizeof(g_voice_actions) / sizeof(g_voice_actions[0])))

static void App_Init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();

    SYSCFG_DL_PWM_init();
    SYSCFG_DL_PWM_LEG_FL_init();
    SYSCFG_DL_PWM_LEG_FR_init();
    SYSCFG_DL_PWM_LEG_BL_init();
    SYSCFG_DL_PWM_LEG_BR_init();

    SYSCFG_DL_VOICE_I2C_init();
    SYSCFG_DL_SYSTICK_init();
}

static void Action_Forward(void)
{
    Pony_Forward();
}

static void Action_Backward(void)
{
    Pony_Backward();
}

static void Action_TurnLeft(void)
{
    Pony_TurnLeft();
}

static void Action_TurnRight(void)
{
    Pony_TurnRight();
}

static void Action_Stand(void)
{
    ServoController_SetStandPose();
}

static void Action_Posture1(void)
{
    Pony_Posture1();
}

static void Action_Posture2(void)
{
    Pony_Posture2();
}

static void Action_ShakeHead(void)
{
    Pony_TurnHeadLeft();
    Pony_TurnHeadRight();
    Pony_TurnHeadCenter();
}

static const VoiceI2C_CommandAction g_voice_actions[] = {
    {1U, "RUN", Action_Forward},
    {2U, "BACK", Action_Backward},
    {3U, "LEFT", Action_TurnLeft},
    {4U, "RIGHT", Action_TurnRight},
    {5U, "STOP", Action_Stand},
    {6U, "POSTURE1", Action_Posture1},
    {7U, "SHAKE", Action_ShakeHead},

    {11U, "RUN", Action_Forward},
    {12U, "BACK", Action_Backward},
    {13U, "LEFT", Action_TurnLeft},
    {14U, "RIGHT", Action_TurnRight},
    {15U, "STOP", Action_Stand},
    {16U, "POSTURE1", Action_Posture1},
    {17U, "POSTURE2", Action_Posture2},
};

static void LCD_ShowHeader(void)
{
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    LCD_ShowString(0, 8, (const uint8_t *) "VOICE I2C MOTOR", RED, WHITE, 16,
        0);
    LCD_ShowString(0, 28, (const uint8_t *) "PB2 SCL  PB3 SDA", BLUE, WHITE,
        16, 0);
    LCD_ShowString(0, 48, (const uint8_t *) "PROTO:", BLACK, WHITE, 16, 0);
    LCD_ShowString(64, 48, (const uint8_t *) VoiceI2C_GetProtocolName(), BLACK,
        WHITE, 16, 0);
}

static void LCD_ShowStatus(uint8_t raw_id)
{
    uint16_t color;

    color = (VoiceI2C_IsBusIdleHigh() != 0U) ? GREEN : RED;

    LCD_Fill(0, 72, LCD_W, 150, WHITE);
    LCD_ShowString(0, 72, (const uint8_t *) VoiceI2C_GetLineText(), color,
        WHITE, 16, 0);

    LCD_ShowString(0, 96, (const uint8_t *) "RAW:", BLUE, WHITE, 16, 0);
    LCD_ShowIntNum(48, 96, raw_id, 3, BLUE, WHITE, 16);

    LCD_ShowString(0, 120, (const uint8_t *) VoiceI2C_GetStatusText(), color,
        WHITE, 16, 0);
}

static void LCD_ShowAction(const char *name, const char *state)
{
    if (name == (const char *) 0) {
        name = "UNKNOWN";
    }

    if (state == (const char *) 0) {
        state = "";
    }

    LCD_Fill(0, 160, LCD_W, 224, WHITE);
    LCD_ShowString(0, 160, (const uint8_t *) "ACTION:", BLUE, WHITE, 16, 0);
    LCD_ShowString(72, 160, (const uint8_t *) name, GREEN, WHITE, 16, 0);
    LCD_ShowString(0, 188, (const uint8_t *) state, BLACK, WHITE, 16, 0);
}

int main(void)
{
    VoiceI2C_Event event;
    VoiceI2C_ReadResult read_result;
    uint32_t refresh_count = 0U;

    App_Init();

    LCD_BLK_Set();
    LCD_Init();
    LCD_ShowHeader();

    ServoController_Init();
    VoiceI2C_InitState();

    LCD_ShowStatus(0U);
    LCD_ShowAction("WAIT VOICE", "IDLE");

    while (1) {
        read_result = VoiceI2C_PollEvent(
            &event, g_voice_actions, VOICE_ACTION_COUNT);

        if (read_result == VOICE_I2C_READ_OK) {
            LCD_ShowStatus(event.cmd_id);

            if (event.action != (VoiceI2C_ActionFn) 0) {
                LCD_ShowAction(event.name, "RUN");
                event.action();

                LCD_ShowStatus(event.cmd_id);
                LCD_ShowAction(event.name, "DONE");
                delay_ms(ACTION_DONE_DELAY_MS);
            } else {
                LCD_ShowAction(event.name, "NO FUNC");
            }

            refresh_count = 0U;
        } else if (read_result == VOICE_I2C_READ_ERROR) {
            LCD_ShowStatus(VoiceI2C_GetLastRawId());
            LCD_ShowAction("I2C ERROR", "CHECK WIRE");
            refresh_count = 0U;
        } else {
            refresh_count++;
            if (refresh_count >= STATUS_REFRESH_TICKS) {
                LCD_ShowStatus(0U);
                LCD_ShowAction("WAIT VOICE", "IDLE");
                refresh_count = 0U;
            }
        }

        delay_ms(POLL_DELAY_MS);
    }
}
