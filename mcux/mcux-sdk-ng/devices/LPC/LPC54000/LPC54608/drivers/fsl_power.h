/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016, 2021, 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_POWER_H_
#define _FSL_POWER_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup power
 * @{
 */

/*! @name Driver version */
/*@{*/
/*! @brief power driver version 2.2.0. */
#define FSL_POWER_DRIVER_VERSION (MAKE_VERSION(2, 2, 0))
/*@}*/

#define MAKE_PD_BITS(reg, slot) (((reg) << 8) | (slot))
#define PDRCFG0                 0x0U
#define PDRCFG1                 0x1U

typedef enum pd_bits
{
    kPDRUNCFG_LP_REG       = MAKE_PD_BITS(PDRCFG0, 2U),
    kPDRUNCFG_PD_FRO_EN    = MAKE_PD_BITS(PDRCFG0, 4U),
    kPDRUNCFG_PD_TS        = MAKE_PD_BITS(PDRCFG0, 6U),
    kPDRUNCFG_PD_BOD_RESET = MAKE_PD_BITS(PDRCFG0, 7U),
    kPDRUNCFG_PD_BOD_INTR  = MAKE_PD_BITS(PDRCFG0, 8U),
    kPDRUNCFG_PD_VD2_ANA   = MAKE_PD_BITS(PDRCFG0, 9U),
    kPDRUNCFG_PD_ADC0      = MAKE_PD_BITS(PDRCFG0, 10U),
    kPDRUNCFG_PD_RAM0      = MAKE_PD_BITS(PDRCFG0, 13U),
    kPDRUNCFG_PD_RAM1      = MAKE_PD_BITS(PDRCFG0, 14U),
    kPDRUNCFG_PD_RAM2      = MAKE_PD_BITS(PDRCFG0, 15U),
    kPDRUNCFG_PD_RAM3      = MAKE_PD_BITS(PDRCFG0, 16U),
    kPDRUNCFG_PD_ROM       = MAKE_PD_BITS(PDRCFG0, 17U),
    kPDRUNCFG_PD_VDDA      = MAKE_PD_BITS(PDRCFG0, 19U),
    kPDRUNCFG_PD_WDT_OSC   = MAKE_PD_BITS(PDRCFG0, 20U),
    kPDRUNCFG_PD_USB0_PHY  = MAKE_PD_BITS(PDRCFG0, 21U),
    kPDRUNCFG_PD_SYS_PLL0  = MAKE_PD_BITS(PDRCFG0, 22U),
    kPDRUNCFG_PD_VREFP     = MAKE_PD_BITS(PDRCFG0, 23U),
    kPDRUNCFG_PD_FLASH_BG  = MAKE_PD_BITS(PDRCFG0, 25U),
    kPDRUNCFG_PD_VD3       = MAKE_PD_BITS(PDRCFG0, 26U),
    kPDRUNCFG_PD_VD4       = MAKE_PD_BITS(PDRCFG0, 27U),
    kPDRUNCFG_PD_VD5       = MAKE_PD_BITS(PDRCFG0, 28U),
    kPDRUNCFG_PD_VD6       = MAKE_PD_BITS(PDRCFG0, 29U),
    kPDRUNCFG_REQ_DELAY    = MAKE_PD_BITS(PDRCFG0, 30U),
    kPDRUNCFG_FORCE_RBB    = MAKE_PD_BITS(PDRCFG0, 31U),

    kPDRUNCFG_PD_USB1_PHY  = MAKE_PD_BITS(PDRCFG1, 0U),
    kPDRUNCFG_PD_USB_PLL   = MAKE_PD_BITS(PDRCFG1, 1U),
    kPDRUNCFG_PD_AUDIO_PLL = MAKE_PD_BITS(PDRCFG1, 2U),
    kPDRUNCFG_PD_SYS_OSC   = MAKE_PD_BITS(PDRCFG1, 3U),
    kPDRUNCFG_PD_EEPROM    = MAKE_PD_BITS(PDRCFG1, 5U),
    kPDRUNCFG_PD_rng       = MAKE_PD_BITS(PDRCFG1, 6U),

    /*
    This enum member has no practical meaning,it is used to avoid MISRA issue,
    user should not trying to use it.
    */
    kPDRUNCFG_ForceUnsigned = (int)0x80000000U,
} pd_bit_t;

/* Power mode configuration API parameter */
typedef enum _power_mode_config
{
    kPmu_Sleep          = 0U,
    kPmu_Deep_Sleep     = 1U,
    kPmu_Deep_PowerDown = 2U,
} power_mode_cfg_t;

/*!
 * @brief The enumeration of BOD status flags.
 */
typedef enum _power_bod_status
{
    kBod_ResetStatusFlag     = SYSCON_BODCTRL_BODRSTSTAT_MASK, /*!< BOD reset has occurred. */
    kBod_InterruptStatusFlag = SYSCON_BODCTRL_BODINTSTAT_MASK, /*!< BOD interrupt has occurred */
} power_bod_status_t;

/*!
 * @brief The enumeration of BOD reset level.
 */
typedef enum _power_bod_reset_level
{
    kBod_ResetLevel0 = 0U, /*!< Reset Level0: 1.5V. */
    kBod_ResetLevel1,      /*!< Reset Level0: 1.85V. */
    kBod_ResetLevel2,      /*!< Reset Level0: 2.0V. */
    kBod_ResetLevel3,      /*!< Reset Level0: 2.3V. */
} power_bod_reset_level_t;

/*!
 * @brief The enumeration of BOD interrupt level.
 */
typedef enum _power_bod_interrupt_level
{
    kBod_InterruptLevel0 = 0U, /*!< Interrupt level: 2.05V. */
    kBod_InterruptLevel1,      /*!< Interrupt level: 2.45V. */
    kBod_InterruptLevel2,      /*!< Interrupt level: 2.75V. */
    kBod_InterruptLevel3,      /*!< Interrupt level: 3.05V. */
} power_bod_interrupt_level_t;

/*!
 * @brief The configuration of power bod, including reset level, interrupt level, and so on.
 */
typedef struct _power_bod_config
{
    bool enableReset;                   /*!< Enable/disable BOD reset function. */
    power_bod_reset_level_t resetLevel; /*!< BOD reset level, please refer to @ref power_bod_reset_level_t. */
    bool enableInterrupt;               /*!< Enable/disable BOD interrupt function. */
    power_bod_interrupt_level_t
        interruptLevel; /*!< BOD interrupt level, please refer to @ref power_bod_interrupt_level_t. */
} power_bod_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name Power Configuration
 * @{
 */

/*!
 * @brief API to enable PDRUNCFG bit in the Syscon. Note that enabling the bit powers down the peripheral
 *
 * @param en    peripheral for which to enable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_EnablePD(pd_bit_t en)
{
    /* PDRUNCFGSET */
    SYSCON->PDRUNCFGSET[((uint32_t)en >> 8UL)] = (1UL << ((uint32_t)en & 0xffU));
}

/*!
 * @brief API to disable PDRUNCFG bit in the Syscon. Note that disabling the bit powers up the peripheral
 *
 * @param en    peripheral for which to disable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_DisablePD(pd_bit_t en)
{
    /* PDRUNCFGCLR */
    SYSCON->PDRUNCFGCLR[((uint32_t)en >> 8UL)] = (1UL << ((uint32_t)en & 0xffU));
}

/*!
 * @brief API to enable deep sleep bit in the ARM Core.
 *
 * @return none
 */
static inline void POWER_EnableDeepSleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API to disable deep sleep bit in the ARM Core.
 *
 * @return none
 */
static inline void POWER_DisableDeepSleep(void)
{
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API to power down flash controller.
 *
 * @return none
 */
static inline void POWER_PowerDownFlash(void)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* TURN OFF clock ip_2113 (only needed for FLASH programming, will be turned on by ROM API) */
    CLOCK_DisableClock(kCLOCK_Flash);

    /* TURN OFF clock ip_2113 (only needed for FLASH programming, will be turned on by ROM API) */
    CLOCK_DisableClock(kCLOCK_Fmc);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * @brief API to power up flash controller.
 *
 * @return none
 */
static inline void POWER_PowerUpFlash(void)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* TURN OFF clock ip_2113 (only needed for FLASH programming, will be turned on by ROM API) */
    CLOCK_EnableClock(kCLOCK_Fmc);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * @brief Power Library API to power the PLLs.
 *
 * @return none
 */
void POWER_SetPLL(void);

/*!
 * @brief Power Library API to power the USB PHY.
 *
 * @return none
 */
void POWER_SetUsbPhy(void);

/*!
 * @brief Power Library API to enter different power mode.
 *
 * @param mode  Power mode configuration
 * @param exclude_from_pd  Bit mask of the PDRUNCFG0(low 32bits) and PDRUNCFG1(high 32bits) that needs to be powered on
 * during power mode selected.
 * @return none
 */
void POWER_EnterPowerMode(power_mode_cfg_t mode, uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to enter sleep mode.
 *
 * @return none
 */
void POWER_EnterSleep(void);

/*!
 * @brief Power Library API to enter deep sleep mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG0(low 32bits) and PDRUNCFG1(high 32bits) bits that needs to be
 * powered on during deep sleep
 * @return none
 */
void POWER_EnterDeepSleep(uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to enter deep power down mode.
 *
 * @param exclude_from_pd   Bit mask of the PDRUNCFG0(low 32bits) and PDRUNCFG1(high 32bits) that needs to be powered on
 during deep power
 *                          down mode, but this is has no effect as the voltages are cut off.

 * @return none
 */
void POWER_EnterDeepPowerDown(uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to choose normal regulation and set the voltage for the desired operating frequency.
 *
 * @param freq  - The desired frequency at which the part would like to operate,
 *                note that the voltage and flash wait states should be set before changing frequency
 * @return none
 */
void POWER_SetVoltageForFreq(uint32_t freq);

/*!
 * @brief Power Library API to return the library version.
 *
 * @return version number of the power library
 */
uint32_t POWER_GetLibVersion(void);

/*!
 * @brief Initialize BOD, including enabling/disabling BOD interrupt, enabling/disabling BOD reset, setting BOD
 * interrupt level, and reset level.
 *
 * @param bodConfig Pointer the the structure @ref power_bod_config_t.
 */
void POWER_InitBod(const power_bod_config_t *bodConfig);

/*!
 * @brief Get default BOD configuration.
 *
 * @code
 *      bodConfig->enableReset = true;
 *      bodConfig->resetLevel = kBod_ResetLevel0;
 *      bodConfig->enableInterrupt = false;
 *      bodConfig->interruptLevel = kBod_InterruptLevel0;
 * @endcode
 *
 * @param bodConfig Pointer the the structure @ref power_bod_config_t.
 */
void POWER_GetDefaultBodConfig(power_bod_config_t *bodConfig);

/*!
 * @brief De-initialize BOD.
 */
static inline void POWER_DeinitBod(void)
{
    SYSCON->BODCTRL &= ~(SYSCON_BODCTRL_BODRSTENA_MASK | SYSCON_BODCTRL_BODINTENA_MASK);
}

/*!
 * @brief Get Bod status flags.
 *
 * @return uint32_t
 */
static inline uint32_t POWER_GetBodStatusFlags(void)
{
    return (uint32_t)((SYSCON->BODCTRL) & (SYSCON_BODCTRL_BODRSTSTAT_MASK | SYSCON_BODCTRL_BODINTSTAT_MASK));
}

/*!
 * @brief Clear Bod status flags.
 *
 * @param mask The mask of status flags to clear, should be the OR'ed value of @ref power_bod_status_t.
 */
static inline void POWER_ClearBodStatusFlags(uint32_t mask)
{
    SYSCON->BODCTRL = mask;
}

/*@}*/

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* _FSL_POWER_H_ */
