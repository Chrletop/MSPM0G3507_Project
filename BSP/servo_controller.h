#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "board.h"

typedef enum {
    SERVO_MOTOR_HEAD = 0,
    SERVO_MOTOR_LEG_FL,
    SERVO_MOTOR_LEG_FR,
    SERVO_MOTOR_LEG_BL,
    SERVO_MOTOR_LEG_BR,
    SERVO_MOTOR_COUNT
} ServoMotor;

#define SERVO_MOTOR_FRONT_LEFT  SERVO_MOTOR_LEG_FL
#define SERVO_MOTOR_FRONT_RIGHT SERVO_MOTOR_LEG_FR
#define SERVO_MOTOR_REAR_LEFT   SERVO_MOTOR_LEG_BL
#define SERVO_MOTOR_REAR_RIGHT  SERVO_MOTOR_LEG_BR

void ServoController_Init(void);
void ServoController_SetAngle(ServoMotor motor, unsigned int angle);
unsigned int ServoController_GetAngle(ServoMotor motor);
void ServoController_SetStandPose(void);

void Pony_Forward(void);
void Pony_Backward(void);
void Pony_MoveForward(unsigned int step_count);
void Pony_MoveBackward(unsigned int step_count);
void Pony_TurnRight(void);
void Pony_TurnLeft(void);
void Pony_TurnRightCycles(unsigned int cycle_count);
void Pony_TurnLeftCycles(unsigned int cycle_count);
void Pony_Posture1(void);
void Pony_Posture2(void);
void Pony_TurnHeadLeft(void);
void Pony_TurnHeadRight(void);
void Pony_TurnHeadCenter(void);

#endif
