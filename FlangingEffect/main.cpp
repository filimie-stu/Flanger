#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include "Flanger.hpp"
#include "FileIO.hpp"

using sample_t = std::int32_t;


int main(int argc, char* argv[]) {

	auto inputRawFilePath = "";
	auto outputRawFilePath = "";

	auto inputSamples = readFile<sample_t>(inputRawFilePath);

	// Opisy poszczegolnych parametrow znajduja sie w pliku FlangerSettings.hpp,
	// przy definicji struktury.
	//
	auto settings = FlangerSettings();
	settings.delayAmplitudeMilliseconds = 6;
	settings.delayOscillationFrequencyHz = 0.45;
	settings.originalTrackAttenuation = 0.5;
	settings.depth = 0.7;
	settings.loopDirection = LoopDirection::FEED_FORWARD;

	auto resultSamples = applyFlanger(inputSamples, settings);
	
	writeFile<sample_t>(outputRawFilePath, resultSamples);

	return 0;
}