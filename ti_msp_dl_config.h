/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM */
#define PWM_INST                                                           TIMG7
#define PWM_INST_IRQHandler                                     TIMG7_IRQHandler
#define PWM_INST_INT_IRQN                                       (TIMG7_INT_IRQn)
#define PWM_INST_CLK_FREQ                                                  20000
/* GPIO defines for channel 0 */
#define GPIO_PWM_C0_PORT                                                   GPIOA
#define GPIO_PWM_C0_PIN                                           DL_GPIO_PIN_28
#define GPIO_PWM_C0_IOMUX                                         (IOMUX_PINCM3)
#define GPIO_PWM_C0_IOMUX_FUNC                        IOMUX_PINCM3_PF_TIMG7_CCP0
#define GPIO_PWM_C0_IDX                                      DL_TIMER_CC_0_INDEX

/* Defines for PWM_LEG_FL */
#define PWM_LEG_FL_INST                                                    TIMA1
#define PWM_LEG_FL_INST_IRQHandler                              TIMA1_IRQHandler
#define PWM_LEG_FL_INST_INT_IRQN                                (TIMA1_INT_IRQn)
#define PWM_LEG_FL_INST_CLK_FREQ                                           20000
/* GPIO defines for channel 0 */
#define GPIO_PWM_LEG_FL_C0_PORT                                            GPIOA
#define GPIO_PWM_LEG_FL_C0_PIN                                    DL_GPIO_PIN_15
#define GPIO_PWM_LEG_FL_C0_IOMUX                                 (IOMUX_PINCM37)
#define GPIO_PWM_LEG_FL_C0_IOMUX_FUNC                IOMUX_PINCM37_PF_TIMA1_CCP0
#define GPIO_PWM_LEG_FL_C0_IDX                               DL_TIMER_CC_0_INDEX

/* Defines for PWM_LEG_FR */
#define PWM_LEG_FR_INST                                                    TIMG8
#define PWM_LEG_FR_INST_IRQHandler                              TIMG8_IRQHandler
#define PWM_LEG_FR_INST_INT_IRQN                                (TIMG8_INT_IRQn)
#define PWM_LEG_FR_INST_CLK_FREQ                                           20000
/* GPIO defines for channel 0 */
#define GPIO_PWM_LEG_FR_C0_PORT                                            GPIOA
#define GPIO_PWM_LEG_FR_C0_PIN                                     DL_GPIO_PIN_1
#define GPIO_PWM_LEG_FR_C0_IOMUX                                  (IOMUX_PINCM2)
#define GPIO_PWM_LEG_FR_C0_IOMUX_FUNC                 IOMUX_PINCM2_PF_TIMG8_CCP0
#define GPIO_PWM_LEG_FR_C0_IDX                               DL_TIMER_CC_0_INDEX

/* Defines for PWM_LEG_BL */
#define PWM_LEG_BL_INST                                                    TIMG6
#define PWM_LEG_BL_INST_IRQHandler                              TIMG6_IRQHandler
#define PWM_LEG_BL_INST_INT_IRQN                                (TIMG6_INT_IRQn)
#define PWM_LEG_BL_INST_CLK_FREQ                                           20000
/* GPIO defines for channel 0 */
#define GPIO_PWM_LEG_BL_C0_PORT                                            GPIOB
#define GPIO_PWM_LEG_BL_C0_PIN                                     DL_GPIO_PIN_6
#define GPIO_PWM_LEG_BL_C0_IOMUX                                 (IOMUX_PINCM23)
#define GPIO_PWM_LEG_BL_C0_IOMUX_FUNC                IOMUX_PINCM23_PF_TIMG6_CCP0
#define GPIO_PWM_LEG_BL_C0_IDX                               DL_TIMER_CC_0_INDEX

/* Defines for PWM_LEG_BR */
#define PWM_LEG_BR_INST                                                    TIMA0
#define PWM_LEG_BR_INST_IRQHandler                              TIMA0_IRQHandler
#define PWM_LEG_BR_INST_INT_IRQN                                (TIMA0_INT_IRQn)
#define PWM_LEG_BR_INST_CLK_FREQ                                           20000
/* GPIO defines for channel 0 */
#define GPIO_PWM_LEG_BR_C0_PORT                                            GPIOA
#define GPIO_PWM_LEG_BR_C0_PIN                                     DL_GPIO_PIN_0
#define GPIO_PWM_LEG_BR_C0_IOMUX                                  (IOMUX_PINCM1)
#define GPIO_PWM_LEG_BR_C0_IOMUX_FUNC                 IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWM_LEG_BR_C0_IDX                               DL_TIMER_CC_0_INDEX




/* Defines for VOICE_I2C */
#define VOICE_I2C_INST                                                      I2C1
#define VOICE_I2C_INST_IRQHandler                                I2C1_IRQHandler
#define VOICE_I2C_INST_INT_IRQN                                    I2C1_INT_IRQn
#define VOICE_I2C_BUS_SPEED_HZ                                            100000
#define GPIO_VOICE_I2C_SDA_PORT                                            GPIOB
#define GPIO_VOICE_I2C_SDA_PIN                                     DL_GPIO_PIN_3
#define GPIO_VOICE_I2C_IOMUX_SDA                                 (IOMUX_PINCM16)
#define GPIO_VOICE_I2C_IOMUX_SDA_FUNC                  IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_VOICE_I2C_SCL_PORT                                            GPIOB
#define GPIO_VOICE_I2C_SCL_PIN                                     DL_GPIO_PIN_2
#define GPIO_VOICE_I2C_IOMUX_SCL                                 (IOMUX_PINCM15)
#define GPIO_VOICE_I2C_IOMUX_SCL_FUNC                  IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                            4000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_4_MHZ_9600_BAUD                                         (26)
#define UART_0_FBRD_4_MHZ_9600_BAUD                                          (3)




/* Defines for SPEAKER_SPI */
#define SPEAKER_SPI_INST                                                   SPI1
#define SPEAKER_SPI_INST_IRQHandler                             SPI1_IRQHandler
#define SPEAKER_SPI_INST_INT_IRQN                                 SPI1_INT_IRQn
#define GPIO_SPEAKER_SPI_PICO_PORT                                        GPIOB
#define GPIO_SPEAKER_SPI_PICO_PIN                                 DL_GPIO_PIN_8
#define GPIO_SPEAKER_SPI_IOMUX_PICO                             (IOMUX_PINCM25)
#define GPIO_SPEAKER_SPI_IOMUX_PICO_FUNC             IOMUX_PINCM25_PF_SPI1_PICO
#define GPIO_SPEAKER_SPI_POCI_PORT                                        GPIOB
#define GPIO_SPEAKER_SPI_POCI_PIN                                DL_GPIO_PIN_14
#define GPIO_SPEAKER_SPI_IOMUX_POCI                             (IOMUX_PINCM31)
#define GPIO_SPEAKER_SPI_IOMUX_POCI_FUNC             IOMUX_PINCM31_PF_SPI1_POCI
/* GPIO configuration for SPEAKER_SPI */
#define GPIO_SPEAKER_SPI_SCLK_PORT                                        GPIOB
#define GPIO_SPEAKER_SPI_SCLK_PIN                                 DL_GPIO_PIN_9
#define GPIO_SPEAKER_SPI_IOMUX_SCLK                             (IOMUX_PINCM26)
#define GPIO_SPEAKER_SPI_IOMUX_SCLK_FUNC             IOMUX_PINCM26_PF_SPI1_SCLK



/* Port definition for Pin Group SPEAKER */
#define SPEAKER_PORT                                                     (GPIOB)

/* Defines for LRC: GPIOB.7 with pinCMx 24 on package pin 59 */
#define SPEAKER_LRC_PIN                                          (DL_GPIO_PIN_7)
#define SPEAKER_LRC_IOMUX                                        (IOMUX_PINCM24)
/* Port definition for Pin Group LCD */
#define LCD_PORT                                                         (GPIOA)

/* Defines for SCL: GPIOA.7 with pinCMx 14 on package pin 49 */
#define LCD_SCL_PIN                                              (DL_GPIO_PIN_7)
#define LCD_SCL_IOMUX                                            (IOMUX_PINCM14)
/* Defines for SDA: GPIOA.12 with pinCMx 34 on package pin 5 */
#define LCD_SDA_PIN                                             (DL_GPIO_PIN_12)
#define LCD_SDA_IOMUX                                            (IOMUX_PINCM34)
/* Defines for RES: GPIOA.23 with pinCMx 53 on package pin 24 */
#define LCD_RES_PIN                                             (DL_GPIO_PIN_23)
#define LCD_RES_IOMUX                                            (IOMUX_PINCM53)
/* Defines for DC: GPIOA.16 with pinCMx 38 on package pin 9 */
#define LCD_DC_PIN                                              (DL_GPIO_PIN_16)
#define LCD_DC_IOMUX                                             (IOMUX_PINCM38)
/* Defines for FSO: GPIOA.18 with pinCMx 40 on package pin 11 */
#define LCD_FSO_PIN                                             (DL_GPIO_PIN_18)
#define LCD_FSO_IOMUX                                            (IOMUX_PINCM40)
/* Defines for CS1: GPIOA.17 with pinCMx 39 on package pin 10 */
#define LCD_CS1_PIN                                             (DL_GPIO_PIN_17)
#define LCD_CS1_IOMUX                                            (IOMUX_PINCM39)
/* Defines for CS2: GPIOA.14 with pinCMx 36 on package pin 7 */
#define LCD_CS2_PIN                                             (DL_GPIO_PIN_14)
#define LCD_CS2_IOMUX                                            (IOMUX_PINCM36)
/* Defines for BLK: GPIOA.13 with pinCMx 35 on package pin 6 */
#define LCD_BLK_PIN                                             (DL_GPIO_PIN_13)
#define LCD_BLK_IOMUX                                            (IOMUX_PINCM35)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_init(void);
void SYSCFG_DL_PWM_LEG_FL_init(void);
void SYSCFG_DL_PWM_LEG_FR_init(void);
void SYSCFG_DL_PWM_LEG_BL_init(void);
void SYSCFG_DL_PWM_LEG_BR_init(void);
void SYSCFG_DL_VOICE_I2C_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_SPEAKER_SPI_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
