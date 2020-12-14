#include <vector>
#include <string>
#include <stdexcept>

// Sample type should be chosen based on
// the size of actual samples, i.e. for
// 32-bit signed samples, Sample should be
// std::int32_t.
// 
template<class Sample>
std::vector<Sample> readFile(const std::string& path) {
	auto inFile = std::fopen(path.data(), "rb");
	if (!inFile) {
		throw std::runtime_error("Could not open input file.");
	}

	auto inputData = std::vector<Sample>();

	// The buffer's size is purely arbitrary.
	//
	auto buffer = std::vector<Sample>(4096 / sizeof(Sample));
	auto readCount = std::size_t(0);

	do {

		// Fill the buffer
		//
		readCount = std::fread(
			buffer.data(),
			sizeof(Sample),
			buffer.size(),
			inFile
		);
		
		// Append buffer's contents
		// to output container.
		//
		inputData.insert(
			inputData.end(),
			buffer.begin(),
			buffer.begin() + readCount
		);

	} while (readCount == buffer.size());

	std::fclose(inFile);
	return inputData;
}

// Sample type represents the format
// in which the data is stored on disk.
//
template<class Sample>
void writeFile(const std::string& path, const std::vector<Sample>& resultSamples) {
	auto outFile = std::fopen(path.data(), "wb");
	if (!outFile) {
		throw std::runtime_error("Could not open output file.");
	}

	// The buffer's size is purely arbitrary.
	//
	auto buffer = std::vector<Sample>(4096 / sizeof(Sample));
	auto writeCount = std::size_t(0);
	auto writeSum = std::size_t(0);
	auto samplesLeft = resultSamples.size();

	do {

		// Write buffer's contents to file. 
		//
		writeCount = std::fwrite(
			resultSamples.data() + writeSum,
			sizeof(Sample),
			std::min(samplesLeft, buffer.size()),
			outFile
		);

		samplesLeft -= writeCount;
		writeSum += writeCount;
	
		// If we were able to fill the buffer entirely, 
		// loop again, because potentially there's more
		// data waiting.
		//
	} while (samplesLeft);
	
	std::fclose(outFile);
}