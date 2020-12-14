#pragma once

#include "FlangerSettings.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

template<class A, class B, class C>
auto interpolate(A a, B b, C t) {
	return a + t * (b - a);
}


// Uwaga: Funkcja zostala napisana z mysla
// o plikach .raw, o czestotliwosci probkowania 44100 Hz,
// o jednym kanale (mono).
//
template<class Sample>
std::vector<Sample> applyFlanger(
	const std::vector<Sample>& inputSamples,
	const FlangerSettings& settings)
{

	// Przygotowanie kontenera na probki wynikowe - 
	// na poczatku bedzie on zawieral probki wejsciowe, 
	// odpowiednio przeskalowane.
	//
	auto outputSamples = std::vector<Sample>(inputSamples.size());
	std::transform(
		inputSamples.begin(),
		inputSamples.end(),
		outputSamples.begin(),
		//
		// Kazda probka jest przemnozona przez wspolczynnik 
		// podany przez uzytkownika. Mozna sie tym posilkowac,
		// aby uniknac ewentualnych przesterow (tj. clippingu).
		//
		[settings](auto& sample) { return sample * settings.originalTrackAttenuation; }
	);

	auto samplingFreqHz = 44100;	// Czestotliwosc z jaka probkowany 
									// byl plik wejsciowy.

	auto twoPi = 2 * M_PI;
	auto f = settings.delayOscillationFrequencyHz;

	auto A =										// Przesuniecie probek o wartosc czestotliwosci
		settings.delayAmplitudeMilliseconds *		// probkowania daje (dla pliku mono) sekundowe opoznienie.
		samplingFreqHz *							// Tak wiec, aby umozliwic uzytkownikowi podanie wartosci w ms,
		0.001 *										// nalezy ja odpowiednio przeskalowac (*0.001).
		0.5;										// Nastepnie wartosc ta jest dzielona przez 2, poniewaz 
													// zostanie potem raz jeszcze dodana do sinusa opoznienia (by 
													// przesunac go w calosci ponad os Y).

	for (std::size_t i = 0; i < inputSamples.size(); i++) {

		// Funkcja sinusoidalna generujaca wartosc opoznienia dla i-tej probki.
		// Po podzieleniu "i" przez czestotliwosc probkowania, otrzymuje sie 
		// sinus oscylujacy z czestotliwoscia f Hz, gdzie f ustala uzytkownik.
		//
		auto t = static_cast<double>(i) / samplingFreqHz;
		auto delayValue = A + A * std::sin(twoPi * f * t);

		auto shiftedIndex = i - delayValue;

		if (shiftedIndex >= 0 && shiftedIndex < inputSamples.size()) {

			// Aby zmniejszyc negatywny efekt dyskretyzacji probek
			// i kwantyzacji wartosci, zamiast wykorzystac bezposrednio wartosc
			// "outputSamples[shiftedIndex]", dokonywana jest interpolacja
			// wartosci funkcji pomiedzy probkami o indeksach floor(shiftedIndex)
			// i ceil(shiftedIndex), chociaz dla testowanych przypadkow operacja ta
			// wydaje sie zbedna.
			//
			auto indexFloor = static_cast<int>(std::floor(shiftedIndex));
			auto indexCeil = static_cast<int>(std::ceil(shiftedIndex));
			if (indexCeil == inputSamples.size()) {
				indexCeil = indexFloor;
			}
			auto indexFrac = shiftedIndex - static_cast<int>(shiftedIndex);
			

			double interpolatedValue;
			if (settings.loopDirection == LoopDirection::FEED_FORWARD) {
				//
				// Jesli wybrano feed-forward, opoznione probki pobierane
				// sa z wektora wejsciowego.
				//
				interpolatedValue = interpolate(
					inputSamples[indexFloor], 
					inputSamples[indexCeil], 
					indexFrac
				);
			}
			else {
				//
				// Jesli wybrano feed-back, opoznione probki pobierane
				// sa z wektora wynikowego.
				//
				interpolatedValue = interpolate(
					outputSamples[indexFloor], 
					outputSamples[indexCeil], 
					indexFrac
				);
			}

			// Do sygnalu wejsciowego (na poczatku procedury zostal skopiowany do 
			// "outputSamples" i przeskalowany) dodawana jest interpolowana wartosc.
			// Warto zauwazyc, ze obecna jest tu petla sprzezenia zwrotnego - 
			// "interpolatedValue" obliczane jest na podstawie wartosci 
			// "outputSamples".
			//
			outputSamples[i] += settings.depth * interpolatedValue;
		}
	}

	return outputSamples;
}
