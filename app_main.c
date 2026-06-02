#include <stdio.h>

#include "board.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
#include "bsp_sg90.h"
#include "servo_controller.h"

static void Test_ShowStep(const char *name)
{
    LCD_Fill(0, 70, LCD_W, 110, WHITE);
    LCD_ShowString(0, 70, (const uint8_t *) name, BLUE, WHITE, 16, 0);
    printf("%s\r\n", name);
}

static void Test_PrintAngles(void)
{
    printf("HEAD=%u, FL=%u, FR=%u, BL=%u, BR=%u, OLD=%u\r\n",
           ServoController_GetAngle(SERVO_MOTOR_HEAD),
           ServoController_GetAngle(SERVO_MOTOR_LEG_FL),
           ServoController_GetAngle(SERVO_MOTOR_LEG_FR),
           ServoController_GetAngle(SERVO_MOTOR_LEG_BL),
           ServoController_GetAngle(SERVO_MOTOR_LEG_BR),
           Get_Servo_Angle());
}

int main(void)
{
    board_init();

    LCD_Init();
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    LCD_ShowString(12, 12, (uint8_t *) "SERVO API TEST", RED, WHITE, 16, 0);
    LCD_ShowString(0, 40, (uint8_t *) "LCD_W:", RED, WHITE, 16, 0);
    LCD_ShowIntNum(48, 40, LCD_W, 3, RED, WHITE, 16);
    LCD_ShowString(88, 40, (uint8_t *) "LCD_H:", RED, WHITE, 16, 0);
    LCD_ShowIntNum(136, 40, LCD_H, 3, RED, WHITE, 16);

    while (1) {
        Test_ShowStep("INIT");
        ServoController_Init();
        Test_PrintAngles();
        delay_ms(1000);

        Test_ShowStep("STAND");
        ServoController_SetStandPose();
        Test_PrintAngles();
        delay_ms(1000);

        Test_ShowStep("HEAD LEFT");
        Pony_TurnHeadLeft();
        Test_PrintAngles();
        delay_ms(1000);

        Test_ShowStep("HEAD RIGHT");
        Pony_TurnHeadRight();
        Test_PrintAngles();
        delay_ms(1000);

        Test_ShowStep("HEAD CTR");
        Pony_TurnHeadCenter();
        Test_PrintAngles();
        delay_ms(1000);

        Test_ShowStep("FORWARD");
        Pony_Forward();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("BACKWARD");
        Pony_Backward();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("TURN R");
        Pony_TurnRight();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("TURN L");
        Pony_TurnLeft();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("FWD x2");
        Pony_MoveForward(2U);
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("BWD x2");
        Pony_MoveBackward(2U);
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("POSTURE1");
        Pony_Posture1();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("POSTURE2");
        Pony_Posture2();
        Test_PrintAngles();
        delay_ms(800);

        Test_ShowStep("STAND");
        ServoController_SetStandPose();
        Test_PrintAngles();
        delay_ms(1500);
    }
}
