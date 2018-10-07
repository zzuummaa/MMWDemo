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
#ifdef __cplusplus
extern "C" {
#endif

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

#define MMWDEMO_MAX_FILE_NAME_SIZE 128
/**
 * @brief
 *  Message for reporting DSS assertion information
 *
 * @details
 *  The structure defines the message body for the information
 *  on a DSS exception that should be forwarded to the MSS.
 */
typedef struct MmwDemo_dssAssertInfoMsg_t
{
    /*! @brief file name */
    char     file[MMWDEMO_MAX_FILE_NAME_SIZE];

    /*! @brief line number */
    uint32_t line;
} MmwDemo_dssAssertInfoMsg;

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
    /*! @brief   Detection Information message */
    MmwDemo_detInfoMsg     detObj;
} MmwDemo_message_body;

/*! @brief For advanced frame config, below define means the configuration given is
 * global at frame level and therefore it is broadcast to all sub-frames.
 */
#define MMWDEMO_SUBFRAME_NUM_FRAME_LEVEL_CONFIG (-1)

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
    /*! @brief   Subframe number for which this message is applicable. 
                 Valid only when advanced frame config is used.
                 When advanced frame config is not used, this field should
                 be set to MMWDEMO_SUBFRAME_NUM_FRAME_LEVEL_CONFIG.
    */
    int8_t                   subFrameNum;

    /*! @brief  message body */
    MmwDemo_message_body      body;

} MmwDemo_message;

#ifdef __cplusplus
}
#endif

#endif /* MMW_MESSAGES_H */
