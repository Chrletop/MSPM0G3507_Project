#ifndef VOICE_I2C_H
#define VOICE_I2C_H

#include <stdint.h>

#include "board.h"

#define VOICE_I2C_PROTOCOL_WONDERECHO (1U)
#define VOICE_I2C_PROTOCOL_CI130X_SDK (2U)

#ifndef VOICE_I2C_PROTOCOL
#define VOICE_I2C_PROTOCOL VOICE_I2C_PROTOCOL_WONDERECHO
#endif

typedef enum {
    VOICE_I2C_READ_OK = 0,
    VOICE_I2C_READ_NO_DATA,
    VOICE_I2C_READ_ERROR,
} VoiceI2C_ReadResult;

typedef void (*VoiceI2C_ActionFn)(void);

typedef struct {
    uint8_t cmd_id;
    const char *name;
    VoiceI2C_ActionFn action;
} VoiceI2C_CommandAction;

typedef struct {
    uint8_t cmd_id;
    const char *name;
    VoiceI2C_ActionFn action;
} VoiceI2C_Event;

void VoiceI2C_InitState(void);
VoiceI2C_ReadResult VoiceI2C_ReadRawCommand(uint8_t *cmd_id);
VoiceI2C_ReadResult VoiceI2C_PollEvent(VoiceI2C_Event *event,
    const VoiceI2C_CommandAction *actions, uint32_t action_count);

void VoiceI2C_ClearLatch(void);
uint8_t VoiceI2C_GetLastRawId(void);
uint8_t VoiceI2C_IsBusIdleHigh(void);
const char *VoiceI2C_GetLineText(void);
const char *VoiceI2C_GetStatusText(void);
const char *VoiceI2C_GetProtocolName(void);

#endif
