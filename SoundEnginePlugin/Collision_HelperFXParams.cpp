/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "Collision_HelperFXParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

Collision_HelperFXParams::Collision_HelperFXParams()
{
}

Collision_HelperFXParams::~Collision_HelperFXParams()
{
}

Collision_HelperFXParams::Collision_HelperFXParams(const Collision_HelperFXParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* Collision_HelperFXParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Collision_HelperFXParams(*this));
}

AKRESULT Collision_HelperFXParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default parameters here
        RTPC.fPlaceholder = 0.0f;
        RTPC.Bandpass_Q = 0.707f;
        RTPC.Bandpass_Cutoff = 1000.0f;
        RTPC.Sat_Param = 0.01f;
        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT Collision_HelperFXParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT Collision_HelperFXParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    RTPC.fPlaceholder = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.Bandpass_Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.Bandpass_Cutoff = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);;
    RTPC.Sat_Param = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT Collision_HelperFXParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case PARAM_PLACEHOLDER_ID:
        RTPC.fPlaceholder = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_PLACEHOLDER_ID);
        break;
    case PARAM_BANDFREQ:
        RTPC.Bandpass_Cutoff = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_BANDFREQ);
        break;
    case PARAM_BANDQ:
        RTPC.Bandpass_Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_BANDQ);
        break;
    case PARAM_SATPARAM:
        RTPC.Sat_Param = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_SATPARAM);
        break;
    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
