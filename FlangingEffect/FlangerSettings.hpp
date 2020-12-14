#pragma once

#include "LoopDirection.hpp"

// Parametry pozwalajace na dostosowanie efektu.
// -> depth: wspolczynnik, przez ktory mnozona jest
//			 przesunieta skladowa sygnalu: 
//			 zazwyczaj przymuje wartosci od 0 do 1.
// -> delayOscillationFrequencyHz: czestotliwosc 
//			 zmian wartosci opoznienia:
//			 zazwyczaj ponizej 0.5 Hz.
// -> delayAmplitudeMilliseconds: amplituda wartosci
//			 opoznienia:
//			 zazwyczaj w granicach pojedynczych milisekund.
// -> originalTrackAttentuation: wspolczynnik, przez ktory
//			 mnozona jest bazowa (tj. nieprzesunieta) 
//			 skladowa sygnalu - parametr dodany w celu
//			 eliminacji ewentualnego clippingu.
// -> loopDirection: wybor pomiedzy petla sprzezenia 
//			 w przod i w tyl.
//
struct FlangerSettings {
	double depth = 0.5;
	double delayOscillationFrequencyHz = 0.1;
	double delayAmplitudeMilliseconds = 5;
	double originalTrackAttenuation = 0.4;
	LoopDirection loopDirection = LoopDirection::FEED_BACK;
};
