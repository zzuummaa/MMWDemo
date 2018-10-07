/**
 *   @file  mmw_messages.h
 *
 *   @brief
 *      This is the main header file for the Millimeter Wave Demo
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
#ifndef MMW_MESSAGES_H
#define MMW_MESSAGES_H

#include "mmw_output.h"
#include "mmw_config.h"

#ifdef __cplusplus
extern "C" {
#endif
/* UART API */
#include <stdint.h>

/**
 * @brief
 *  Message types used in Millimeter Wave Demo for Mailbox communication 
 * between MSS and DSS.
 *
 * @details
 *  The enum is used to hold all the messages types used for Mailbox communication
 * between MSS and DSS in mmw Demo.
 */
typedef enum MmwDemo_message_type_e 
{
    /*! @brief   message types for MSS to DSS communication */
    MMWDEMO_MSS2DSS_GUIMON_CFG = 0xFEED0001,
    MMWDEMO_MSS2DSS_CFAR_RANGE_CFG,
    MMWDEMO_MSS2DSS_CFAR_DOPPLER_CFG,
    MMWDEMO_MSS2DSS_PEAK_GROUPING_CFG,
    MMWDEMO_MSS2DSS_MULTI_OBJ_BEAM_FORM,
    MMWDEMO_MSS2DSS_CALIB_DC_RANGE_SIG,
    MMWDEMO_MSS2DSS_DETOBJ_SHIPPED,
    MMWDEMO_MSS2DSS_SET_DATALOGGER,

    /*! @brief   message types for DSS to MSS communication */
    MMWDEMO_DSS2MSS_CONFIGDONE = 0xFEED0100,
    MMWDEMO_DSS2MSS_DETOBJ_READY,
    MMWDEMO_DSS2MSS_STOPDONE,

}MmwDemo_message_type;



/**
 * @brief
 *  TLV part of the message from DSS to MSS on data path detection information.
 *
 * @details
 *  The structure describes the message item 
 */
typedef struct MmwDemo_msgTlv_t
{
    /*! @brief   Payload type */
    uint32_t    type;

    /*! @brief   Length in bytes */
    uint32_t    length;

    /*! @brief Address of the payload */
    uint32_t   address;
} MmwDemo_msgTlv;

/**
 * @brief
 *  Message for reporting detection information from data path to MSS
 *
 * @details
 *  The structure defines the message body for detection information from from data path to MSS.
 */
typedef struct MmwDemo_detObjMsg_t
{
    /*! @brief Header of the detection information message */
    MmwDemo_output_message_header header;

    /*! @brief TLVs of the detection information */
    MmwDemo_msgTlv   tlv[MMWDEMO_OUTPUT_MSG_MAX];
} MmwDemo_detInfoMsg;

/**
 * @brief
 *  Message body used in Millimeter Wave Demo for passing configuration from MSS
 * to DSS.
 *
 * @details
 *  The union defines the message body for various configuration messages. 
 */
typedef union MmwDemo_message_body_u 
{
    /*! @brief   Gui Monitor Selection */
    MmwDemo_GuiMonSel     guiMonSel;

    /*! @brief   CFAR Range/Doppler configuraiton */
    MmwDemo_CfarCfg       cfarCfg;

    /*! @brief   Peak grouping configuration */
     MmwDemo_PeakGroupingCfg peakGroupingCfg;

     /*! @brief   Multi object beam forming configuration */
     MmwDemo_MultiObjBeamFormingCfg multiObjBeamFormingCfg;

     /*! @brief   Calibrate DC (zero) range signature */
     MmwDemo_CalibDcRangeSigCfg calibDcRangeSigCfg;

    /*! @brief   Detectection Information message */
    MmwDemo_detInfoMsg     detObj;

    /*! @brief   Datapath output logger setting */
    uint8_t               dataLogger;
} MmwDemo_message_body;

/**
 * @brief
 *  DSS/MSS communication messages
 *
 * @details
 *  The structure defines the message structure used to commuincate between MSS
 * and DSS.
 */
typedef struct MmwDemo_message_t
{
    /*! @brief   message type */
    MmwDemo_message_type      type;

    /*! @brief   message length : PROC_TBA does body need to be pointer and not a union structure?*/
    //uint32_t                  len;

    /*! @brief  message body */
    MmwDemo_message_body      body;

} MmwDemo_message;

#ifdef __cplusplus
}
#endif

#endif /* MMW_MESSAGES_H */
