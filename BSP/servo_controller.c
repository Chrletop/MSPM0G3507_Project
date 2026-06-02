#include "servo_controller.h"

#define SERVO_PWM_MIN_COUNT (10U)
#define SERVO_PWM_MAX_COUNT (50U)
#define SERVO_MAX_ANGLE     (180U)

#define PONY_HEAD_CENTER_ANGLE (90U)
#define PONY_HEAD_LEFT_ANGLE   (45U)
#define PONY_HEAD_RIGHT_ANGLE  (135U)

#define PONY_LEG_CENTER_ANGLE   (90U)
#define PONY_SMOOTH_STEP_DEG    (4U)
#define PONY_SMOOTH_DELAY_MS    (15U)

typedef struct {
    GPTIMER_Regs *timer;
    DL_TIMER_CC_INDEX cc_index;
} ServoPwmOutput;

typedef struct {
    unsigned int fl;
    unsigned int fr;
    unsigned int bl;
    unsigned int br;
} PonyLegPose;

static unsigned int g_servo_angles[SERVO_MOTOR_COUNT] = {
    PONY_HEAD_CENTER_ANGLE,
    PONY_LEG_CENTER_ANGLE,
    PONY_LEG_CENTER_ANGLE,
    PONY_LEG_CENTER_ANGLE,
    PONY_LEG_CENTER_ANGLE,
};

static const ServoPwmOutput g_servo_outputs[SERVO_MOTOR_COUNT] = {
#if defined(PWM_INST) && defined(GPIO_PWM_C0_IDX)
    {PWM_INST, GPIO_PWM_C0_IDX},
#else
    {(GPTIMER_Regs *) 0, DL_TIMER_CC_0_INDEX},
#endif

#if defined(PWM_LEG_FL_INST) && defined(GPIO_PWM_LEG_FL_C0_IDX)
    {PWM_LEG_FL_INST, GPIO_PWM_LEG_FL_C0_IDX},
#else
    {(GPTIMER_Regs *) 0, DL_TIMER_CC_0_INDEX},
#endif

#if defined(PWM_LEG_FR_INST) && defined(GPIO_PWM_LEG_FR_C0_IDX)
    {PWM_LEG_FR_INST, GPIO_PWM_LEG_FR_C0_IDX},
#else
    {(GPTIMER_Regs *) 0, DL_TIMER_CC_0_INDEX},
#endif

#if defined(PWM_LEG_BL_INST) && defined(GPIO_PWM_LEG_BL_C0_IDX)
    {PWM_LEG_BL_INST, GPIO_PWM_LEG_BL_C0_IDX},
#else
    {(GPTIMER_Regs *) 0, DL_TIMER_CC_0_INDEX},
#endif

#if defined(PWM_LEG_BR_INST) && defined(GPIO_PWM_LEG_BR_C0_IDX)
    {PWM_LEG_BR_INST, GPIO_PWM_LEG_BR_C0_IDX},
#else
    {(GPTIMER_Regs *) 0, DL_TIMER_CC_0_INDEX},
#endif
};

static const PonyLegPose g_forward_sequence[] = {
    {90U, 130U, 50U, 90U},
    {130U, 130U, 50U, 50U},
    {130U, 90U, 90U, 50U},
    {90U, 90U, 90U, 90U},
    {50U, 90U, 90U, 130U},
    {50U, 50U, 130U, 130U},
    {90U, 50U, 130U, 90U},
    {90U, 90U, 90U, 90U},
};

static const PonyLegPose g_backward_sequence[] = {
    {90U, 50U, 130U, 90U},
    {50U, 50U, 130U, 130U},
    {50U, 90U, 90U, 130U},
    {90U, 90U, 90U, 90U},
    {130U, 90U, 90U, 50U},
    {130U, 130U, 50U, 50U},
    {90U, 130U, 50U, 90U},
    {90U, 90U, 90U, 90U},
};

static const PonyLegPose g_turn_right_sequence[] = {
    {90U, 50U, 50U, 90U},
    {130U, 50U, 50U, 130U},
    {130U, 90U, 90U, 130U},
    {90U, 90U, 90U, 90U},
};

static const PonyLegPose g_turn_left_sequence[] = {
    {130U, 90U, 90U, 130U},
    {130U, 50U, 50U, 130U},
    {90U, 50U, 50U, 90U},
    {90U, 90U, 90U, 90U},
};

static const PonyLegPose g_posture_1_sequence[] = {
    {90U, 110U, 90U, 90U},
    {90U, 145U, 90U, 90U},
    {90U, 180U, 90U, 90U},
    {90U, 145U, 90U, 90U},
};

static const PonyLegPose g_posture_2_sequence[] = {
    {30U, 150U, 30U, 150U},
    {90U, 90U, 90U, 90U},
    {150U, 30U, 150U, 30U},
    {90U, 90U, 90U, 90U},
};

static unsigned int ServoController_ClampAngle(unsigned int angle)
{
    if (angle > SERVO_MAX_ANGLE) {
        angle = SERVO_MAX_ANGLE;
    }

    return angle;
}

static uint32_t ServoController_AngleToCount(unsigned int angle)
{
    angle = ServoController_ClampAngle(angle);
    return SERVO_PWM_MIN_COUNT +
           (((uint32_t) angle * (SERVO_PWM_MAX_COUNT - SERVO_PWM_MIN_COUNT) +
             (SERVO_MAX_ANGLE / 2U)) /
            SERVO_MAX_ANGLE);
}

static unsigned int ServoController_AbsDiff(unsigned int a, unsigned int b)
{
    return (a > b) ? (a - b) : (b - a);
}

static unsigned int ServoController_Max(unsigned int a, unsigned int b)
{
    return (a > b) ? a : b;
}

static unsigned int ServoController_Interpolate(unsigned int start,
                                                unsigned int target,
                                                unsigned int step,
                                                unsigned int total_steps)
{
    int diff = (int) target - (int) start;
    int value;

    if (total_steps == 0U) {
        return target;
    }

    value = (int) start + ((diff * (int) step) / (int) total_steps);
    if (value < 0) {
        return 0U;
    }

    return ServoController_ClampAngle((unsigned int) value);
}

static void Pony_SetLegPoseImmediate(const PonyLegPose *pose)
{
    ServoController_SetAngle(SERVO_MOTOR_LEG_FL, pose->fl);
    ServoController_SetAngle(SERVO_MOTOR_LEG_FR, pose->fr);
    ServoController_SetAngle(SERVO_MOTOR_LEG_BL, pose->bl);
    ServoController_SetAngle(SERVO_MOTOR_LEG_BR, pose->br);
}

static void Pony_MoveLegPoseSmooth(const PonyLegPose *target)
{
    PonyLegPose start = {
        ServoController_GetAngle(SERVO_MOTOR_LEG_FL),
        ServoController_GetAngle(SERVO_MOTOR_LEG_FR),
        ServoController_GetAngle(SERVO_MOTOR_LEG_BL),
        ServoController_GetAngle(SERVO_MOTOR_LEG_BR),
    };
    PonyLegPose clamped_target = {
        ServoController_ClampAngle(target->fl),
        ServoController_ClampAngle(target->fr),
        ServoController_ClampAngle(target->bl),
        ServoController_ClampAngle(target->br),
    };
    unsigned int max_delta;
    unsigned int total_steps;
    unsigned int step;

    max_delta = ServoController_Max(
        ServoController_AbsDiff(start.fl, clamped_target.fl),
        ServoController_AbsDiff(start.fr, clamped_target.fr));
    max_delta = ServoController_Max(
        max_delta, ServoController_AbsDiff(start.bl, clamped_target.bl));
    max_delta = ServoController_Max(
        max_delta, ServoController_AbsDiff(start.br, clamped_target.br));

    total_steps = (max_delta + PONY_SMOOTH_STEP_DEG - 1U) / PONY_SMOOTH_STEP_DEG;
    if (total_steps == 0U) {
        Pony_SetLegPoseImmediate(&clamped_target);
        return;
    }

    for (step = 1U; step <= total_steps; step++) {
        PonyLegPose pose = {
            ServoController_Interpolate(start.fl, clamped_target.fl, step, total_steps),
            ServoController_Interpolate(start.fr, clamped_target.fr, step, total_steps),
            ServoController_Interpolate(start.bl, clamped_target.bl, step, total_steps),
            ServoController_Interpolate(start.br, clamped_target.br, step, total_steps),
        };

        Pony_SetLegPoseImmediate(&pose);
        delay_ms(PONY_SMOOTH_DELAY_MS);
    }
}

static void Pony_RunLegSequence(const PonyLegPose *sequence,
                                unsigned int pose_count,
                                unsigned int cycle_count)
{
    unsigned int cycle;
    unsigned int pose;

    for (cycle = 0U; cycle < cycle_count; cycle++) {
        for (pose = 0U; pose < pose_count; pose++) {
            Pony_MoveLegPoseSmooth(&sequence[pose]);
        }
    }
}

static void ServoController_MoveAngleSmooth(ServoMotor motor, unsigned int angle)
{
    unsigned int start = ServoController_GetAngle(motor);
    unsigned int target = ServoController_ClampAngle(angle);
    unsigned int max_delta = ServoController_AbsDiff(start, target);
    unsigned int total_steps =
        (max_delta + PONY_SMOOTH_STEP_DEG - 1U) / PONY_SMOOTH_STEP_DEG;
    unsigned int step;

    if (total_steps == 0U) {
        ServoController_SetAngle(motor, target);
        return;
    }

    for (step = 1U; step <= total_steps; step++) {
        ServoController_SetAngle(
            motor, ServoController_Interpolate(start, target, step, total_steps));
        delay_ms(PONY_SMOOTH_DELAY_MS);
    }
}

void ServoController_Init(void)
{
    ServoController_SetStandPose();
}

void ServoController_SetAngle(ServoMotor motor, unsigned int angle)
{
    const ServoPwmOutput *output;

    if ((unsigned int) motor >= (unsigned int) SERVO_MOTOR_COUNT) {
        return;
    }

    angle                 = ServoController_ClampAngle(angle);
    g_servo_angles[motor] = angle;
    output                = &g_servo_outputs[motor];

    if (output->timer == (GPTIMER_Regs *) 0) {
        return;
    }

    DL_Timer_setCaptureCompareValue(
        output->timer, ServoController_AngleToCount(angle), output->cc_index);
}

unsigned int ServoController_GetAngle(ServoMotor motor)
{
    if ((unsigned int) motor >= (unsigned int) SERVO_MOTOR_COUNT) {
        return 0U;
    }

    return g_servo_angles[motor];
}

void ServoController_SetStandPose(void)
{
    static const PonyLegPose stand_pose = {
        PONY_LEG_CENTER_ANGLE,
        PONY_LEG_CENTER_ANGLE,
        PONY_LEG_CENTER_ANGLE,
        PONY_LEG_CENTER_ANGLE,
    };

    ServoController_MoveAngleSmooth(SERVO_MOTOR_HEAD, PONY_HEAD_CENTER_ANGLE);
    Pony_MoveLegPoseSmooth(&stand_pose);
}

void Pony_MoveForward(unsigned int step_count)
{
    Pony_RunLegSequence(g_forward_sequence,
                        sizeof(g_forward_sequence) / sizeof(g_forward_sequence[0]),
                        step_count);
}

void Pony_MoveBackward(unsigned int step_count)
{
    Pony_RunLegSequence(g_backward_sequence,
                        sizeof(g_backward_sequence) / sizeof(g_backward_sequence[0]),
                        step_count);
}

void Pony_Forward(void)
{
    Pony_MoveForward(1U);
}

void Pony_Backward(void)
{
    Pony_MoveBackward(1U);
}

void Pony_TurnRightCycles(unsigned int cycle_count)
{
    Pony_RunLegSequence(g_turn_right_sequence,
                        sizeof(g_turn_right_sequence) /
                            sizeof(g_turn_right_sequence[0]),
                        cycle_count);
}

void Pony_TurnLeftCycles(unsigned int cycle_count)
{
    Pony_RunLegSequence(g_turn_left_sequence,
                        sizeof(g_turn_left_sequence) /
                            sizeof(g_turn_left_sequence[0]),
                        cycle_count);
}

void Pony_TurnRight(void)
{
    Pony_TurnRightCycles(1U);
}

void Pony_TurnLeft(void)
{
    Pony_TurnLeftCycles(1U);
}

void Pony_Posture1(void)
{
    Pony_RunLegSequence(g_posture_1_sequence,
                        sizeof(g_posture_1_sequence) /
                            sizeof(g_posture_1_sequence[0]),
                        1U);
}

void Pony_Posture2(void)
{
    Pony_RunLegSequence(g_posture_2_sequence,
                        sizeof(g_posture_2_sequence) /
                            sizeof(g_posture_2_sequence[0]),
                        1U);
}

void Pony_TurnHeadLeft(void)
{
    ServoController_MoveAngleSmooth(SERVO_MOTOR_HEAD, PONY_HEAD_LEFT_ANGLE);
}

void Pony_TurnHeadRight(void)
{
    ServoController_MoveAngleSmooth(SERVO_MOTOR_HEAD, PONY_HEAD_RIGHT_ANGLE);
}

void Pony_TurnHeadCenter(void)
{
    ServoController_MoveAngleSmooth(SERVO_MOTOR_HEAD, PONY_HEAD_CENTER_ANGLE);
}
