/*
 * Copyright 2018-2021,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>

#include "mflash_drv.h"

#include "fsl_flexspi.h"
#include "fsl_cache.h"
#include "pin_mux.h"

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL        7
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST          13
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD     0
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS         1
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE        2
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR        3
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD   4
#define NOR_CMD_LUT_SEQ_IDX_READID             8
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG     9
#define NOR_CMD_LUT_SEQ_IDX_ENTERQPI           10
#define NOR_CMD_LUT_SEQ_IDX_EXITQPI            11
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG      12
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP          5

#define CUSTOM_LUT_LENGTH        60
#define FLASH_BUSY_STATUS_POL    1
#define FLASH_BUSY_STATUS_OFFSET 0

#define FLASH_SIZE 0x4000 /* 128Mb / 1KByte */

#ifndef XIP_EXTERNAL_FLASH
flexspi_device_config_t deviceconfig = {
    .flexspiRootClk       = 133000000UL,
    .flashSize            = MFLASH_BSIZE / 1024U,
    .CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval           = 2,
    .CSHoldTime           = 3,
    .CSSetupTime          = 3,
    .dataValidTime        = 0,
    .columnspace          = 0,
    .enableWordAddress    = 0,
    .AWRSeqIndex          = 0,
    .AWRSeqNumber         = 0,
    .ARDSeqIndex          = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber         = 1,
    .AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval = 0,
};
#endif

const uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
    /* Normal read mode -SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Fast read mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Fast read quad mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    /* Read extend parameters */
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x81, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Write Enable */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Erase Sector  */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),

    /* Page Program - single mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Page Program - quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Read ID */
    [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Enable Quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    /* Enter QPI mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_ENTERQPI] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x35, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Exit QPI mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_EXITQPI] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xF5, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Read status register */
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Erase whole chip */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
};

static status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base)
{
    /* Wait status ready. */
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }
        if (FLASH_BUSY_STATUS_POL)
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }

    } while (isBusy);

    return status;
}

static status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t address)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write neable */
    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base)
{
    flexspi_transfer_t flashXfer;
    status_t status;
    uint32_t writeValue = 0x40;

    /* Write enable */
    status = flexspi_nor_write_enable(base, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG;
    flashXfer.data          = &writeValue;
    flashXfer.dataSize      = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

static status_t flexspi_nor_flash_sector_erase(FLEXSPI_Type *base, uint32_t address)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

static status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Prepare page program command */
    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data          = (uint32_t *)src;
    flashXfer.dataSize      = MFLASH_PAGE_SIZE;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

static status_t flexspi_nor_read_data(FLEXSPI_Type *base, uint32_t startAddress, uint32_t *buffer, uint32_t length)
{
    status_t status;
    flexspi_transfer_t flashXfer;
    uint32_t readAddress = startAddress;

    /* Read page. */
    flashXfer.deviceAddress = readAddress;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = length;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

/* Initialize flash peripheral,
 * cannot be invoked directly, requires calling wrapper in non XIP memory */
static int32_t mflash_drv_init_internal(void)
{
    /* NOTE: Multithread access is not supported for SRAM target.
     *       XIP target MUST be protected by disabling global interrupts
     *       since all ISR (and API that is used inside) is placed at XIP.
     *       It is necessary to place at least "mflash_drv.o", "fsl_flexspi.o" to SRAM */
    /* disable interrupts when running from XIP */
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    status_t status = kStatus_Success;

#ifndef XIP_EXTERNAL_FLASH
    flexspi_config_t config;

    /* Get FLEXSPI default settings and configure the flexspi. */
    FLEXSPI_GetDefaultConfig(&config);

    /* Set AHB buffer size for reading data through AHB bus. */
    config.ahbConfig.enableAHBPrefetch   = true;
    config.ahbConfig.enableAHBBufferable = true;
    config.ahbConfig.enableAHBCachable   = true;
    config.rxSampleClock                 = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
    FLEXSPI_Init(MFLASH_FLEXSPI, &config);

    /* AHB Read Address option bit. This option bit is intend to remove AHB burst start address alignment limitation */
    MFLASH_FLEXSPI->AHBCR |= FLEXSPI_AHBCR_READADDROPT_MASK;

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig(MFLASH_FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);
#endif
    uint32_t tmpLUT[CUSTOM_LUT_LENGTH] = {0x00U};

    memcpy(tmpLUT, customLUT, sizeof(tmpLUT));
    /* Update LUT table. */
    FLEXSPI_UpdateLUT(MFLASH_FLEXSPI, 0, tmpLUT, CUSTOM_LUT_LENGTH);

#ifndef XIP_EXTERNAL_FLASH
    /* Enter quad mode. */
    status = flexspi_nor_enable_quad_mode(MFLASH_FLEXSPI);
#endif

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    return status;
}

/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    /* Necessary to have double wrapper call in non_xip memory */
    return mflash_drv_init_internal();
}

/* Internal - erase single sector */
static int32_t mflash_drv_sector_erase_internal(uint32_t sector_addr)
{
    status_t status;
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    status = flexspi_nor_flash_sector_erase(MFLASH_FLEXSPI, sector_addr);

    /* Do software reset. */
    FLEXSPI_SoftwareReset(MFLASH_FLEXSPI);

    DCACHE_InvalidateByRange(MFLASH_BASE_ADDRESS + sector_addr, MFLASH_SECTOR_SIZE);

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ISB();

    return status;
}

/* Calling wrapper for 'mflash_drv_sector_erase_internal'.
 * Erase one sector starting at 'sector_addr' - must be sector aligned.
 */
int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    if (0 == mflash_drv_is_sector_aligned(sector_addr))
        return kStatus_InvalidArgument;

    return mflash_drv_sector_erase_internal(sector_addr);
}

/* Internal - write single page */
static int32_t mflash_drv_page_program_internal(uint32_t page_addr, uint32_t *data)
{
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    status_t status;
    status = flexspi_nor_flash_page_program(MFLASH_FLEXSPI, page_addr, data);

    /* Do software reset. */
    FLEXSPI_SoftwareReset(MFLASH_FLEXSPI);

    DCACHE_InvalidateByRange(MFLASH_BASE_ADDRESS + page_addr, MFLASH_PAGE_SIZE);

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ISB();

    return status;
}

/* Calling wrapper for 'mflash_drv_page_program_internal'.
 * Write 'data' to 'page_addr' - must be page aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_page_program(uint32_t page_addr, uint32_t *data)
{
    if (0 == mflash_drv_is_page_aligned(page_addr))
        return kStatus_InvalidArgument;

    return mflash_drv_page_program_internal(page_addr, data);
}

/* Internal - read data */
static int32_t mflash_drv_read_internal(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    status_t status;
    status = flexspi_nor_read_data(MFLASH_FLEXSPI, addr, buffer, len);

    /* Do software reset. */
    FLEXSPI_SoftwareReset(MFLASH_FLEXSPI);

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ISB();

    return status;
}

/* Calling wrapper for 'mflash_drv_read_internal'. */
int32_t mflash_drv_read(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    /* Check alignment */
    if (((uint32_t)buffer % 4) || (len % 4))
        return kStatus_InvalidArgument;

    return mflash_drv_read_internal(addr, buffer, len);
}

/* Returns pointer (AHB address) to memory area where the specified region of FLASH is mapped, NULL on failure (could
 * not map continuous block) */
void *mflash_drv_phys2log(uint32_t addr, uint32_t len)
{
    /* take FLEXSPI remapping into account */
    uint32_t remap_offset = MFLASH_REMAP_OFFSET();
    uint32_t remap_start  = MFLASH_REMAP_START();
    uint32_t remap_end    = MFLASH_REMAP_END();
    uint32_t bus_addr;

    do {
        if (addr >= MFLASH_BSIZE)
        {
            bus_addr = 0UL;
            break;
        }
        /* calculate the bus address where the requested FLASH region is expected to be available */
        bus_addr = addr + MFLASH_BASE_ADDRESS;

        if((remap_offset == 0UL) || (remap_end <= remap_start))
        {
            /* remapping is not active */
            break;
        }

        if((remap_start >= bus_addr + len) || (remap_end <= bus_addr))
        {
            /* remapping window does not collide with bus addresses normally assigned for requested range of FLASH */
            break;
        }

        if((remap_start + remap_offset <= bus_addr) && (remap_end + remap_offset >= bus_addr + len))
        {
            /* remapping window covers the whole requested range of FLASH, return address adjusted by negative offset */
            bus_addr -= remap_offset;
            break;
        }
        bus_addr = 0UL;
        /* the bus address region normally assigned for requested range of FLASH is partially or completely shadowed by
        * remapping, fail */

    } while (false);

    return (void *) bus_addr;
}

/* Returns address of physical memory where the area accessible by given pointer is actually stored, UINT32_MAX on
 * failure (could not map as continuous block) */
uint32_t mflash_drv_log2phys(void *ptr, uint32_t len)
{
    /* take FLEXSPI remapping into account */
    uint32_t remap_offset = MFLASH_REMAP_OFFSET();
    uint32_t remap_start  = MFLASH_REMAP_START();
    uint32_t remap_end    = MFLASH_REMAP_END();
    uint32_t ret = UINT32_MAX;

    /* calculate the bus address where the requested FLASH region is expected to be available */
    do {
        uint32_t bus_addr = (uint32_t)ptr;

        if(bus_addr < MFLASH_BASE_ADDRESS)
        {
            /* the pointer points outside of the flash memory area */
            break;
        }

        ret = (bus_addr - MFLASH_BASE_ADDRESS);

        if (ret >= MFLASH_BSIZE)
        {
            /* the pointer points beyond the flash memory area */
            ret = UINT32_MAX;
            break;
        }

        if((remap_offset == 0UL) || (remap_end <= remap_start))
        {
            /* remapping is not active */
            break;
        }

        if((remap_start >= bus_addr + len) || (remap_end <= bus_addr))
        {
            /* remapping window does not affect the requested memory area */
            break;
        }

        if((remap_start <= bus_addr) && (remap_end >= bus_addr + len))
        {
            /* remapping window covers the whole address range, return address adjusted by offset */
            ret += remap_offset;
            break;
        }

        ret = UINT32_MAX;
        /* the bus address region partially collides with the remapping window, hence the range is not mapped to continuous
         * block in the FLASH, fail */
    } while (false);

    return ret;
}
