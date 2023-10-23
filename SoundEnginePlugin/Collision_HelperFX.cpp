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

#include "Collision_HelperFX.h"
#include "../Collision_HelperConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateCollision_HelperFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Collision_HelperFX());
}

AK::IAkPluginParam* CreateCollision_HelperFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Collision_HelperFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(Collision_HelperFX, AkPluginTypeEffect, Collision_HelperConfig::CompanyID, Collision_HelperConfig::PluginID)

Collision_HelperFX::Collision_HelperFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

Collision_HelperFX::~Collision_HelperFX()
{
}

AKRESULT Collision_HelperFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (Collision_HelperFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    Filters::SampleRate = static_cast<AkReal32>(in_rFormat.uSampleRate);

    BandpassFilter.CutoffFrequence  = m_pParams->RTPC.Bandpass_Cutoff;
    BandpassFilter.Q = m_pParams->RTPC.Bandpass_Q;
    p_Sat = m_pParams->RTPC.Sat_Param;

    return AK_Success;
}

AKRESULT Collision_HelperFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT Collision_HelperFX::Reset()
{
    return AK_Success;
}

AKRESULT Collision_HelperFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void Collision_HelperFX::Execute(AkAudioBuffer* io_pBuffer)
{
    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();

    BandpassFilter.CutoffFrequence = m_pParams->RTPC.Bandpass_Cutoff;
    BandpassFilter.Q = m_pParams->RTPC.Bandpass_Q;
    p_Sat = m_pParams->RTPC.Sat_Param;
    if (p_Sat == 0.0f)
    {
        p_Sat = APPROX_VALUE;
    }


    AkUInt16 uFramesProcessed;

    if (p_Sat < 0)
    {
        p_Sat = p_Sat * -1.0f + 2.0f;
        for (AkUInt32 i = 0; i < uNumChannels; ++i)
        {
            AkReal32* AK_RESTRICT pBuf = (AkReal32 * AK_RESTRICT)io_pBuffer->GetChannel(i);

            uFramesProcessed = 0;
            while (uFramesProcessed < io_pBuffer->uValidFrames)
            {
                // Execute DSP in-place here
                AkReal32 output = Util.SaturateFunc2(pBuf[uFramesProcessed], p_Sat); //saturation
                output = BandpassFilter.BandPassFilter(output);
                pBuf[uFramesProcessed] = output;
                ++uFramesProcessed;
            }
        }
    }
    else
    {
        for (AkUInt32 i = 0; i < uNumChannels; ++i)
        {
            AkReal32* AK_RESTRICT pBuf = (AkReal32 * AK_RESTRICT)io_pBuffer->GetChannel(i);

            uFramesProcessed = 0;
            while (uFramesProcessed < io_pBuffer->uValidFrames)
            {
                // Execute DSP in-place here
                AkReal32 output = Util.SaturateFunc(pBuf[uFramesProcessed], p_Sat); //saturation
                output = BandpassFilter.BandPassFilter(output);
                pBuf[uFramesProcessed] = output;
                ++uFramesProcessed;
            }
        }
    }

}

AKRESULT Collision_HelperFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
