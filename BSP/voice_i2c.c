#include "voice_i2c.h"

#define WONDERECHO_I2C_ADDR   (0x34U)
#define WONDERECHO_RESULT_REG (0x64U)

#define CI130X_SDK_I2C_ADDR   (0x64U)
#define CI130X_SDK_CMDID_REG  (0x02U)
#define CI130X_SDK_PACKET_END (0x5AU)

#define VOICE_I2C_TIMEOUT_LOOP (800000U)

#define VOICE_I2C_CLEAR_INTERRUPTS                                      \
    (DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |                              \
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |                           \
        DL_I2C_INTERRUPT_CONTROLLER_NACK |                              \
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST)

#define VOICE_I2C_ERROR_INTERRUPTS                                      \
    (DL_I2C_INTERRUPT_CONTROLLER_NACK |                                 \
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST)

static const char *g_status_text = "VOICE READY";
static uint8_t g_latched_id      = 0U;
static uint8_t g_last_raw_id     = 0U;

static uint8_t VoiceI2C_IsSclHigh(void)
{
    return (DL_I2C_getSCLStatus(VOICE_I2C_INST) ==
            DL_I2C_CONTROLLER_SCL_HIGH)
               ? 1U
               : 0U;
}

static uint8_t VoiceI2C_IsSdaHigh(void)
{
    return (DL_I2C_getSDAStatus(VOICE_I2C_INST) ==
            DL_I2C_CONTROLLER_SDA_HIGH)
               ? 1U
               : 0U;
}

uint8_t VoiceI2C_IsBusIdleHigh(void)
{
    return ((VoiceI2C_IsSclHigh() != 0U) &&
            (VoiceI2C_IsSdaHigh() != 0U))
               ? 1U
               : 0U;
}

const char *VoiceI2C_GetLineText(void)
{
    uint8_t scl = VoiceI2C_IsSclHigh();
    uint8_t sda = VoiceI2C_IsSdaHigh();

    if ((scl != 0U) && (sda != 0U)) {
        return "BUS:S1 D1";
    }
    if ((scl != 0U) && (sda == 0U)) {
        return "BUS:S1 D0";
    }
    if ((scl == 0U) && (sda != 0U)) {
        return "BUS:S0 D1";
    }

    return "BUS:S0 D0";
}

const char *VoiceI2C_GetStatusText(void)
{
    return g_status_text;
}

const char *VoiceI2C_GetProtocolName(void)
{
#if (VOICE_I2C_PROTOCOL == VOICE_I2C_PROTOCOL_WONDERECHO)
    return "WONDERECHO";
#elif (VOICE_I2C_PROTOCOL == VOICE_I2C_PROTOCOL_CI130X_SDK)
    return "CI130X SDK";
#else
    return "UNKNOWN";
#endif
}

uint8_t VoiceI2C_GetLastRawId(void)
{
    return g_last_raw_id;
}

void VoiceI2C_ClearLatch(void)
{
    g_latched_id = 0U;
}

static void VoiceI2C_Recover(void)
{
    DL_I2C_resetControllerTransfer(VOICE_I2C_INST);
    DL_I2C_flushControllerTXFIFO(VOICE_I2C_INST);
    DL_I2C_flushControllerRXFIFO(VOICE_I2C_INST);
    DL_I2C_clearInterruptStatus(VOICE_I2C_INST, VOICE_I2C_CLEAR_INTERRUPTS);
}

void VoiceI2C_InitState(void)
{
    g_status_text = "VOICE READY";
    g_latched_id  = 0U;
    g_last_raw_id = 0U;
    VoiceI2C_Recover();
}

static VoiceI2C_ReadResult VoiceI2C_Fail(const char *text)
{
    g_status_text = text;
    VoiceI2C_Recover();
    return VOICE_I2C_READ_ERROR;
}

static VoiceI2C_ReadResult VoiceI2C_WaitIdle(void)
{
    uint32_t timeout = VOICE_I2C_TIMEOUT_LOOP;

    while (timeout > 0U) {
        if ((DL_I2C_getControllerStatus(VOICE_I2C_INST) &
                DL_I2C_CONTROLLER_STATUS_IDLE) != 0U) {
            return VOICE_I2C_READ_OK;
        }
        timeout--;
    }

    return VoiceI2C_Fail("I2C IDLE TMO");
}

static VoiceI2C_ReadResult VoiceI2C_WaitBusFree(void)
{
    uint32_t timeout = VOICE_I2C_TIMEOUT_LOOP;

    while (timeout > 0U) {
        if ((DL_I2C_getControllerStatus(VOICE_I2C_INST) &
                DL_I2C_CONTROLLER_STATUS_BUSY_BUS) == 0U) {
            return VOICE_I2C_READ_OK;
        }
        timeout--;
    }

    return VoiceI2C_Fail("I2C BUSY");
}

static VoiceI2C_ReadResult VoiceI2C_WaitInterrupt(uint32_t done_mask,
    const char *timeout_text)
{
    uint32_t timeout = VOICE_I2C_TIMEOUT_LOOP;
    uint32_t interrupt_status;
    uint32_t controller_status;

    while (timeout > 0U) {
        interrupt_status = DL_I2C_getRawInterruptStatus(
            VOICE_I2C_INST, done_mask | VOICE_I2C_ERROR_INTERRUPTS);

        if ((interrupt_status & DL_I2C_INTERRUPT_CONTROLLER_NACK) != 0U) {
            return VoiceI2C_Fail("I2C NACK");
        }

        if ((interrupt_status &
                DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST) != 0U) {
            return VoiceI2C_Fail("I2C ARB LOST");
        }

        controller_status = DL_I2C_getControllerStatus(VOICE_I2C_INST);
        if ((controller_status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
            return VoiceI2C_Fail("I2C STATUS ERR");
        }

        if ((interrupt_status & done_mask) != 0U) {
            return VOICE_I2C_READ_OK;
        }

        timeout--;
    }

    return VoiceI2C_Fail(timeout_text);
}

static VoiceI2C_ReadResult VoiceI2C_PrepareTransfer(void)
{
    if (VoiceI2C_IsBusIdleHigh() == 0U) {
        g_status_text = VoiceI2C_GetLineText();
        VoiceI2C_Recover();
        return VOICE_I2C_READ_ERROR;
    }

    if (VoiceI2C_WaitIdle() != VOICE_I2C_READ_OK) {
        return VOICE_I2C_READ_ERROR;
    }

    if (VoiceI2C_WaitBusFree() != VOICE_I2C_READ_OK) {
        return VOICE_I2C_READ_ERROR;
    }

    DL_I2C_flushControllerTXFIFO(VOICE_I2C_INST);
    DL_I2C_flushControllerRXFIFO(VOICE_I2C_INST);
    DL_I2C_clearInterruptStatus(VOICE_I2C_INST, VOICE_I2C_CLEAR_INTERRUPTS);

    return VOICE_I2C_READ_OK;
}

static VoiceI2C_ReadResult VoiceI2C_ReadRxBytes(uint8_t *data, uint8_t length)
{
    uint8_t count     = 0U;
    uint32_t timeout  = VOICE_I2C_TIMEOUT_LOOP;
    uint32_t status   = 0U;
    uint32_t int_stat = 0U;

    while ((timeout > 0U) && (count < length)) {
        status = DL_I2C_getControllerStatus(VOICE_I2C_INST);
        if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
            return VoiceI2C_Fail("I2C STATUS ERR");
        }

        int_stat = DL_I2C_getRawInterruptStatus(
            VOICE_I2C_INST, VOICE_I2C_ERROR_INTERRUPTS);

        if ((int_stat & DL_I2C_INTERRUPT_CONTROLLER_NACK) != 0U) {
            return VoiceI2C_Fail("I2C NACK");
        }

        if ((int_stat & DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST) != 0U) {
            return VoiceI2C_Fail("I2C ARB LOST");
        }

        if (!DL_I2C_isControllerRXFIFOEmpty(VOICE_I2C_INST)) {
            data[count] = DL_I2C_receiveControllerData(VOICE_I2C_INST);
            count++;
            timeout = VOICE_I2C_TIMEOUT_LOOP;
        } else {
            timeout--;
        }
    }

    if (count != length) {
        return VoiceI2C_Fail("I2C RX TMO");
    }

    if (VoiceI2C_WaitBusFree() != VOICE_I2C_READ_OK) {
        return VOICE_I2C_READ_ERROR;
    }

    return VOICE_I2C_READ_OK;
}

static VoiceI2C_ReadResult VoiceI2C_ReadWonderEcho(uint8_t *cmd_id)
{
    uint8_t value = 0U;
    VoiceI2C_ReadResult result;

    result = VoiceI2C_PrepareTransfer();
    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    DL_I2C_transmitControllerData(VOICE_I2C_INST, WONDERECHO_RESULT_REG);
    DL_I2C_startControllerTransfer(VOICE_I2C_INST, WONDERECHO_I2C_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1U);

    if (VoiceI2C_WaitBusFree() != VOICE_I2C_READ_OK) {
        return VOICE_I2C_READ_ERROR;
    }

    if (VoiceI2C_WaitIdle() != VOICE_I2C_READ_OK) {
        return VOICE_I2C_READ_ERROR;
    }

    DL_I2C_flushControllerTXFIFO(VOICE_I2C_INST);
    DL_I2C_flushControllerRXFIFO(VOICE_I2C_INST);
    DL_I2C_clearInterruptStatus(VOICE_I2C_INST, VOICE_I2C_CLEAR_INTERRUPTS);

    DL_I2C_startControllerTransfer(VOICE_I2C_INST, WONDERECHO_I2C_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 1U);

    result = VoiceI2C_ReadRxBytes(&value, 1U);
    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    *cmd_id = value;
    if (value == 0U) {
        g_status_text = "NO CMD";
        return VOICE_I2C_READ_NO_DATA;
    }

    g_status_text = "I2C READ OK";
    return VOICE_I2C_READ_OK;
}

static VoiceI2C_ReadResult VoiceI2C_ReadCI130XSDK(uint8_t *cmd_id)
{
    uint8_t packet[3];
    uint8_t checksum;
    VoiceI2C_ReadResult result;

    result = VoiceI2C_PrepareTransfer();
    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    DL_I2C_transmitControllerData(VOICE_I2C_INST, CI130X_SDK_CMDID_REG);
    DL_I2C_startControllerTransferAdvanced(VOICE_I2C_INST, CI130X_SDK_I2C_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1U, DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_DISABLE, DL_I2C_CONTROLLER_ACK_ENABLE);

    result = VoiceI2C_WaitInterrupt(
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE, "I2C TX TMO");
    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    DL_I2C_clearInterruptStatus(VOICE_I2C_INST, VOICE_I2C_CLEAR_INTERRUPTS);
    DL_I2C_flushControllerRXFIFO(VOICE_I2C_INST);

    DL_I2C_startControllerTransferAdvanced(VOICE_I2C_INST, CI130X_SDK_I2C_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 3U, DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE, DL_I2C_CONTROLLER_ACK_ENABLE);

    result = VoiceI2C_ReadRxBytes(packet, 3U);
    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    if (packet[0] == 0U) {
        *cmd_id       = 0U;
        g_status_text = "NO CMD";
        return VOICE_I2C_READ_NO_DATA;
    }

    checksum = (uint8_t) (CI130X_SDK_CMDID_REG + packet[0]);
    if ((packet[1] != checksum) || (packet[2] != CI130X_SDK_PACKET_END)) {
        return VoiceI2C_Fail("I2C PKT ERR");
    }

    *cmd_id       = packet[0];
    g_status_text = "I2C READ OK";
    return VOICE_I2C_READ_OK;
}

VoiceI2C_ReadResult VoiceI2C_ReadRawCommand(uint8_t *cmd_id)
{
    if (cmd_id == (uint8_t *) 0) {
        return VoiceI2C_Fail("NULL CMD PTR");
    }

    *cmd_id = 0U;

#if (VOICE_I2C_PROTOCOL == VOICE_I2C_PROTOCOL_WONDERECHO)
    return VoiceI2C_ReadWonderEcho(cmd_id);
#elif (VOICE_I2C_PROTOCOL == VOICE_I2C_PROTOCOL_CI130X_SDK)
    return VoiceI2C_ReadCI130XSDK(cmd_id);
#else
#error "Unsupported VOICE_I2C_PROTOCOL"
#endif
}

static const VoiceI2C_CommandAction *VoiceI2C_FindAction(
    const VoiceI2C_CommandAction *actions, uint32_t action_count, uint8_t cmd_id)
{
    uint32_t i;

    if (actions == (const VoiceI2C_CommandAction *) 0) {
        return (const VoiceI2C_CommandAction *) 0;
    }

    for (i = 0U; i < action_count; i++) {
        if (actions[i].cmd_id == cmd_id) {
            return &actions[i];
        }
    }

    return (const VoiceI2C_CommandAction *) 0;
}

VoiceI2C_ReadResult VoiceI2C_PollEvent(VoiceI2C_Event *event,
    const VoiceI2C_CommandAction *actions, uint32_t action_count)
{
    uint8_t cmd_id = 0U;
    VoiceI2C_ReadResult result;
    const VoiceI2C_CommandAction *action;

    if (event != (VoiceI2C_Event *) 0) {
        event->cmd_id = 0U;
        event->name   = "NONE";
        event->action = (VoiceI2C_ActionFn) 0;
    }

    result = VoiceI2C_ReadRawCommand(&cmd_id);
    g_last_raw_id = cmd_id;

    if (result == VOICE_I2C_READ_NO_DATA) {
        g_latched_id = 0U;
        return VOICE_I2C_READ_NO_DATA;
    }

    if (result != VOICE_I2C_READ_OK) {
        return result;
    }

    if (cmd_id == g_latched_id) {
        g_status_text = "VOICE HOLD";
        return VOICE_I2C_READ_NO_DATA;
    }

    g_latched_id = cmd_id;
    action       = VoiceI2C_FindAction(actions, action_count, cmd_id);

    if (event != (VoiceI2C_Event *) 0) {
        event->cmd_id = cmd_id;
        if (action != (const VoiceI2C_CommandAction *) 0) {
            event->name   = action->name;
            event->action = action->action;
        } else {
            event->name   = "UNKNOWN";
            event->action = (VoiceI2C_ActionFn) 0;
        }
    }

    if (action == (const VoiceI2C_CommandAction *) 0) {
        g_status_text = "UNKNOWN CMD";
    } else {
        g_status_text = "VOICE EVENT";
    }

    return VOICE_I2C_READ_OK;
}
