#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include "LookUpTable.h"
class Filters
{
public:

	static AkReal32 SampleRate;
	AkReal32 CutoffFrequence;
	AkReal32 Q;

private:
	AkReal32 InputSamples[2] = { 0.0f, };
	AkReal32 OutputSamples[2] = { 0.0f, };
	SinCosTanLookupTable LookupTable;

public:
	AkSampleType LowpassFilter(AkSampleType Sample)
	{
		AkReal32 Cutoff_Frequence = CutoffFrequence / SampleRate;
		AkReal32 Beta = 0.5f * (1.0f - LookupTable.sin(Cutoff_Frequence) / 2.0f * Q) / (1.0f + LookupTable.sin(Cutoff_Frequence) / 2.0f * Q);
		AkReal32 Gamma = (0.5f + Beta) * LookupTable.cos(Cutoff_Frequence);

		AkReal32 Common_Coeff = 0.5f + Beta - Gamma;

		AkReal32 a0 = Common_Coeff / 2.0f;
		AkReal32 a1 = Common_Coeff;
		AkReal32 a2 = a0;

		AkReal32 b1 = -2.0f * Gamma;
		AkReal32 b2 = 2.0f * Beta;

		AkSampleType outputSample = a0 * Sample + a1 * InputSamples[0] + a2 * InputSamples[1] -
			b1 * OutputSamples[0] - b2 * OutputSamples[1];

		InputSamples[1] = InputSamples[0];
		InputSamples[0] = Sample;

		OutputSamples[1] = OutputSamples[0];
		OutputSamples[0] = outputSample;

		return outputSample;
	}

	AkSampleType HighpassFilter(AkSampleType Sample)
	{
		AkReal32 Cutoff_Frequence = CutoffFrequence / SampleRate;
		AkReal32 Beta = 0.5f * (1.0f - LookupTable.sin(Cutoff_Frequence) / 2.0f * Q) / (1.0f + LookupTable.sin(Cutoff_Frequence) / 2.0f * Q);
		AkReal32 Gamma = (0.5f + Beta) * LookupTable.cos(Cutoff_Frequence);

		AkReal32 Common_Coeff = 0.5f + Beta + Gamma;

		AkReal32 a0 = Common_Coeff / 2.0f;
		AkReal32 a1 = -Common_Coeff;
		AkReal32 a2 = a0;

		AkReal32 b1 = -2.0f * Gamma;
		AkReal32 b2 = 2.0f * Beta;

		AkSampleType outputSample = a0 * Sample + a1 * InputSamples[0] + a2 * InputSamples[1] -
			b1 * OutputSamples[0] - b2 * OutputSamples[1];

		InputSamples[1] = InputSamples[0];
		InputSamples[0] = Sample;

		OutputSamples[1] = OutputSamples[0];
		OutputSamples[0] = outputSample;

		return outputSample;
	}

	AkSampleType BandPassFilter(AkSampleType Sample)
	{
		AkReal32 K = LookupTable.tan(CutoffFrequence / (2.0f * SampleRate));
		AkReal32 Beta = K * K * Q + K + Q;


		AkReal32 a0 = K / Beta;
		AkReal32 a1 = 0.0f;
		AkReal32 a2 = -a0;

		AkReal32 b1 = 2.0f * Q * (K * K - 1.0f) / Beta;
		AkReal32 b2 = (K * K * Q - K + Q) / Beta;

		AkSampleType outputSample = a0 * Sample + a1 * InputSamples[0] + a2 * InputSamples[1] -
			b1 * OutputSamples[0] - b2 * OutputSamples[1];

		InputSamples[1] = InputSamples[0];
		InputSamples[0] = Sample;

		OutputSamples[1] = OutputSamples[0];
		OutputSamples[0] = outputSample;

		return outputSample;
	}
};

AkReal32 Filters::SampleRate = 0.0f;

