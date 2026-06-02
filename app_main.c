#include "board.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
#include "bsp_sg90.h"

int main(void)
{
    unsigned int servo_angle = 0;
    int servo_step = 5;
    unsigned int frame = 0;

    board_init();

    LCD_Init();
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    LCD_ShowString(12, 12, (uint8_t *) "LCD + SERVO", RED, WHITE, 16, 0);
    LCD_ShowString(0, 40, (uint8_t *) "LCD_W:", RED, WHITE, 16, 0);
    LCD_ShowIntNum(48, 40, LCD_W, 3, RED, WHITE, 16);
    LCD_ShowString(88, 40, (uint8_t *) "LCD_H:", RED, WHITE, 16, 0);
    LCD_ShowIntNum(136, 40, LCD_H, 3, RED, WHITE, 16);
    LCD_ShowString(0, 70, (uint8_t *) "SERVO:", BLUE, WHITE, 16, 0);

    while (1) {
        Set_Servo_Angle(servo_angle);

        LCD_Fill(56, 70, 96, 86, WHITE);
        LCD_ShowIntNum(56, 70, servo_angle, 3, BLUE, WHITE, 16);

        if ((frame & 1U) == 0U) {
            LCD_ShowPicture(40, 120, 40, 40, gImage_1);
            LCD_ShowPicture(80, 120, 40, 40, gImage_1);
        } else {
            LCD_Fill(40, 120, 120, 160, WHITE);
            Display_Asc_String(28, 132, 16, (uint8_t *) "RUNNING", RED, WHITE);
        }

        if (servo_angle >= 180U) {
            servo_step = -5;
        } else if (servo_angle == 0U) {
            servo_step = 5;
        }
        servo_angle = (unsigned int) ((int) servo_angle + servo_step);
        frame++;

        delay_ms(20);
    }
}
