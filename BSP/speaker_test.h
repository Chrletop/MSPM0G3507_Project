#ifndef SPEAKER_TEST_H
#define SPEAKER_TEST_H

#include <stdint.h>

#define SPEAKER_TEST_SAMPLE_RATE_HZ (15625U)

void SpeakerTest_Init(void);
void SpeakerTest_PlayTone(uint16_t frequency_hz, uint16_t duration_ms);
void SpeakerTest_PlaySilence(uint16_t duration_ms);
void SpeakerTest_PlayBeep(void);

#endif
