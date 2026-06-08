#include "speaker_test.h"

#include "board.h"

#if !defined(GPIO_SPEAKER_SPI_IOMUX_SCLK) || !defined(GPIO_SPEAKER_SPI_IOMUX_PICO)
#error "SPEAKER SPI pins must be configured in SysConfig before using speaker_test."
#endif

#if !defined(SPEAKER_PORT) || !defined(SPEAKER_LRC_PIN)
#error "SPEAKER LRC GPIO must be configured in SysConfig before using speaker_test."
#endif

#define SPEAKER_TEST_BCLK_DELAY_CYCLES (16U)
#define SPEAKER_TEST_SLOT_BITS         (32U)
#define SPEAKER_TEST_PCM_BITS          (16U)
#define SPEAKER_TEST_BEEP_HZ           (1000U)
#define SPEAKER_TEST_BEEP_MS           (350U)
#define SPEAKER_TEST_BEEP_AMPLITUDE    (1800)

static void SpeakerTest_WriteBit(uint32_t din_mask)
{
    if (din_mask != 0U) {
        GPIO_SPEAKER_SPI_PICO_PORT->DOUTSET31_0 = GPIO_SPEAKER_SPI_PICO_PIN;
    } else {
        GPIO_SPEAKER_SPI_PICO_PORT->DOUTCLR31_0 = GPIO_SPEAKER_SPI_PICO_PIN;
    }

    GPIO_SPEAKER_SPI_SCLK_PORT->DOUTSET31_0 = GPIO_SPEAKER_SPI_SCLK_PIN;
    DL_Common_delayCycles(SPEAKER_TEST_BCLK_DELAY_CYCLES);
    GPIO_SPEAKER_SPI_SCLK_PORT->DOUTCLR31_0 = GPIO_SPEAKER_SPI_SCLK_PIN;
    DL_Common_delayCycles(SPEAKER_TEST_BCLK_DELAY_CYCLES);
}

static void SpeakerTest_WriteChannel(uint32_t lrc_pin_state, int16_t sample)
{
    uint16_t value = (uint16_t) sample;
    int32_t bit;
    uint32_t padding_bits;

    if (lrc_pin_state != 0U) {
        SPEAKER_PORT->DOUTSET31_0 = SPEAKER_LRC_PIN;
    } else {
        SPEAKER_PORT->DOUTCLR31_0 = SPEAKER_LRC_PIN;
    }

    /* I2S: LRCLK changes one BCLK before the sample MSB. */
    SpeakerTest_WriteBit(0U);
    for (bit = 15; bit >= 0; bit--) {
        SpeakerTest_WriteBit((uint32_t) value & (1UL << bit));
    }

    padding_bits = SPEAKER_TEST_SLOT_BITS - SPEAKER_TEST_PCM_BITS - 1U;
    while (padding_bits > 0U) {
        SpeakerTest_WriteBit(0U);
        padding_bits--;
    }
}

static void SpeakerTest_WriteStereo(int16_t sample)
{
    SpeakerTest_WriteChannel(0U, sample);
    SpeakerTest_WriteChannel(SPEAKER_LRC_PIN, sample);
}

static uint32_t SpeakerTest_FramesFromMs(uint16_t duration_ms)
{
    return (((uint32_t) duration_ms * SPEAKER_TEST_SAMPLE_RATE_HZ) + 999U) / 1000U;
}

void SpeakerTest_Init(void)
{
#if defined(SPEAKER_SPI_INST)
    DL_SPI_disable(SPEAKER_SPI_INST);
#endif

    DL_GPIO_initDigitalOutput(GPIO_SPEAKER_SPI_IOMUX_SCLK);
    DL_GPIO_initDigitalOutput(GPIO_SPEAKER_SPI_IOMUX_PICO);
    DL_GPIO_initDigitalOutput(SPEAKER_LRC_IOMUX);

    DL_GPIO_clearPins(GPIO_SPEAKER_SPI_SCLK_PORT, GPIO_SPEAKER_SPI_SCLK_PIN);
    DL_GPIO_clearPins(GPIO_SPEAKER_SPI_PICO_PORT, GPIO_SPEAKER_SPI_PICO_PIN);
    DL_GPIO_clearPins(SPEAKER_PORT, SPEAKER_LRC_PIN);

    DL_GPIO_enableOutput(GPIO_SPEAKER_SPI_SCLK_PORT, GPIO_SPEAKER_SPI_SCLK_PIN);
    DL_GPIO_enableOutput(GPIO_SPEAKER_SPI_PICO_PORT, GPIO_SPEAKER_SPI_PICO_PIN);
    DL_GPIO_enableOutput(SPEAKER_PORT, SPEAKER_LRC_PIN);

    SpeakerTest_PlaySilence(300U);
}

void SpeakerTest_PlayTone(uint16_t frequency_hz, uint16_t duration_ms)
{
    uint32_t frame_count;
    uint32_t frame;
    uint32_t half_period_frames;
    uint32_t half_period_count;
    int16_t sample;

    if ((frequency_hz == 0U) || (duration_ms == 0U)) {
        SpeakerTest_PlaySilence(duration_ms);
        return;
    }

    if (frequency_hz >= (SPEAKER_TEST_SAMPLE_RATE_HZ / 2U)) {
        frequency_hz = (SPEAKER_TEST_SAMPLE_RATE_HZ / 2U) - 1U;
    }

    frame_count = SpeakerTest_FramesFromMs(duration_ms);
    half_period_frames = SPEAKER_TEST_SAMPLE_RATE_HZ / (frequency_hz * 2U);
    if (half_period_frames == 0U) {
        half_period_frames = 1U;
    }

    half_period_count = 0U;
    sample            = SPEAKER_TEST_BEEP_AMPLITUDE;
    for (frame = 0U; frame < frame_count; frame++) {
        SpeakerTest_WriteStereo(sample);

        half_period_count++;
        if (half_period_count >= half_period_frames) {
            sample = (sample > 0) ? -SPEAKER_TEST_BEEP_AMPLITUDE : SPEAKER_TEST_BEEP_AMPLITUDE;
            half_period_count = 0U;
        }
    }
}

void SpeakerTest_PlaySilence(uint16_t duration_ms)
{
    uint32_t frame_count;
    uint32_t frame;

    frame_count = SpeakerTest_FramesFromMs(duration_ms);

    for (frame = 0U; frame < frame_count; frame++) {
        SpeakerTest_WriteStereo(0);
    }
}

void SpeakerTest_PlayBeep(void)
{
    SpeakerTest_PlaySilence(600U);
    SpeakerTest_PlayTone(SPEAKER_TEST_BEEP_HZ, SPEAKER_TEST_BEEP_MS);
    SpeakerTest_PlaySilence(250U);
    SpeakerTest_PlayTone(SPEAKER_TEST_BEEP_HZ, SPEAKER_TEST_BEEP_MS);
    SpeakerTest_PlaySilence(250U);
    SpeakerTest_PlayTone(SPEAKER_TEST_BEEP_HZ, SPEAKER_TEST_BEEP_MS);
    SpeakerTest_PlaySilence(600U);
}
