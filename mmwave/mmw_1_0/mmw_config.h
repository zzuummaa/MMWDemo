/**
 *   @file  mmw_config.h
 *
 *   @brief
 *      This is the header file that describes configurations for the Millimeter
 *   Wave Demo.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2016 Texas Instruments, Inc.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MMW_CONFIG_H
#define MMW_CONFIG_H

/* MMWAVE library Include Files */
#include <stdint-gcc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum number of 1D FFT bins in DC range antenna signature compensation */
#define DC_RANGE_SIGNATURE_COMP_MAX_BIN_SIZE 32

/**
 * @brief
 *  Millimeter Wave Demo CFAR Configuration
 *
 * @details
 *  The structure contains the cfar configuration used in data path
 */
typedef struct MmwDemo_CfarCfg_t
{
    /*! @brief    CFAR threshold scale */
    uint16_t       thresholdScale;

    /*! @brief    CFAR averagining mode 0-CFAR_CA, 1-CFAR_CAGO, 2-CFAR_CASO */
    uint8_t        averageMode;

    /*! @brief    CFAR noise avraging window length */
    uint8_t        winLen;

    /*! @brief    CFAR guard length*/
    uint8_t        guardLen;

    /*! @brief    CFAR cumulative noise sum divisor */
    uint8_t        noiseDivShift;

    /*! @brief    CFAR 0-cyclic mode disabled, 1-cyclic mode enabled */
    uint8_t        cyclicMode;

} MmwDemo_CfarCfg;

/**
 * @brief
 *  Millimeter Wave Demo Peak grouping Configuration
 *
 * @details
 *  The structure contains the Peak grouping configuration used in data path
 */
typedef struct MmwDemo_PeakGroupingCfg_t
{
    /*! @brief    minimum range index exported */
    uint16_t        minRangeIndex;

    /*! @brief    maximum range index exported */
    uint16_t        maxRangeIndex;

    /*! @brief    Peak grouping scheme 1-based on neighboring peaks from detection matrix
     *                                 2-based on on neighboring CFAR detected peaks */
    uint8_t        scheme;

    /*! @brief     Grouping in range direction, 0- disbled, 1-enabled */
    uint8_t        inRangeDirectionEn;

    /*! @brief     Grouping in Doppler direction,  0- disbled, 1-enabled */
    uint8_t        inDopplerDirectionEn;

} MmwDemo_PeakGroupingCfg;

/**
 * @brief
 *  Millimeter Wave Demo multi object beam formaing Configuration
 *
 * @details
 *  The structure contains the Peak grouping configuration used in data path
 */
typedef struct MmwDemo_MultiObjBeamFormingCfg_t
{
    /*! @brief    enabled flag:  1-enabled 0-disabled */
    uint16_t     enabled;

    /*! @brief    second peak detection threshold */
    float        multiPeakThrsScal;

} MmwDemo_MultiObjBeamFormingCfg;

/**
 * @brief
 *  Millimeter Wave Demo DC range signature compensation
 *
 * @details
 *  The structure contains the DC range antenna signature removeal configuration used in data path
 */
typedef struct MmwDemo_CalibDcRangeSigCfg_t
{
    /*! @brief    enabled flag:  1-enabled 0-disabled */
    uint16_t    enabled;

    /*! @brief    maximum negative range bin (1D FFT index) to be compensated */
    int16_t     negativeBinIdx;

    /*! @brief    maximum positive range bin (1D FFT index) to be compensated */
    int16_t     positiveBinIdx;

    /*! @brief    number of chirps in the averaging phase */
    uint16_t    numAvgChirps;

} MmwDemo_CalibDcRangeSigCfg;

/**
 * @brief
 *  Millimeter Wave Demo Gui Monitor Selection
 *
 * @details
 *  The structure contains the flags which select what information is sent to gui
 *
 */
typedef struct MmwDemo_GuiMonSel_t
{
    /*! @brief   Send list of detected objects (structure outObj) */
    uint8_t        detectedObjects;

    /*! @brief   Send log magnitude range array  */
    uint8_t        logMagRange;

    /*! @brief   Send noise floor profile */
    uint8_t        noiseProfile;

    /*! @brief   Send complex range bins at zero doppler, all antenna symbols for range-azimuth heat map */
    uint8_t        rangeAzimuthHeatMap;

    /*! @brief   Send complex range bins at zero doppler, (all antenna symbols), for range-azimuth heat map */
    uint8_t        rangeDopplerHeatMap;

    /*! @brief   Send stats */
    uint8_t        statsInfo;

} MmwDemo_GuiMonSel;

/**
 * @brief
 *  Millimeter Wave Demo configuration
 *
 * @details
 *  The structure is used to hold all the relevant configuration
 *  which is used to execute the Millimeter Wave Demo.
 */
typedef struct MmwDemo_Cfg_t
{
    /*! @brief   CPU Clock Frequency. */
    uint32_t            sysClockFrequency;

    /*! @brief   UART Logging Baud Rate. */
    uint32_t            loggingBaudRate;

    /*! @brief   UART Command Baud Rate. */
    uint32_t            commandBaudRate;

    /*! @brief   mmWave Control Configuration. */
    //MMWave_CtrlCfg      ctrlCfg;

    /*! @brief   Gui Monitor Selection */
    MmwDemo_GuiMonSel   guiMonSel;

    /*! @brief   CFAR Configuration, [0] for range direction, [1] for doppler direction*/
    MmwDemo_CfarCfg     cfarCfg[2];

    /*! @brief   Peak grouping configuration */
    MmwDemo_PeakGroupingCfg peakGroupingCfg;

    /*! @brief   Multi object beam forming configuration */
    MmwDemo_MultiObjBeamFormingCfg multiObjBeamFormingCfg;

    /*! @brief   Calibrate DC (zero) range signiture */
    MmwDemo_CalibDcRangeSigCfg calibDcRangeSigCfg;

    /*! @brief   Datapath output loggerSetting
                 0 (default): MSS UART logger
                 1: DSS UART logger
     */
    uint8_t              dataLogger;
} MmwDemo_Cfg;
#ifdef __cplusplus
}
#endif

#endif /* MMW_CONFIG_H */
