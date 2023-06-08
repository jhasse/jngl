// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "audio/track.hpp"

#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Pass ogg file to encode.\n" << std::endl;
		return EXIT_FAILURE;
	}
#ifdef _WIN32
	FILE* const f = fopen(argv[1], "rb");
#else
	FILE* const f = fopen(argv[1], "rbe");
#endif
	if (f == nullptr) {
		std::cerr << "file not found: " << argv[1];
		return EXIT_FAILURE;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<char> buffer(fsize);
	int itemsRead = fread(buffer.data(), fsize, 1, f);
	assert(itemsRead == 1);
	fclose(f);

	std::cout << "Opened file (" << buffer.size() / 1024. / 1024. << " MB)" << std::endl;

	auto track = psemek::audio::load_ogg(buffer);
	auto stream = track->stream();

	const size_t sampleCount = 8000;
	float buf[sampleCount];
	std::chrono::steady_clock clock;
	std::vector<long long> durations;
	auto previous = std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch())
	               .count();
	while (true) {
		size_t read = stream->read(buf, sampleCount);
		if (read <= 0) {
			break;
		}
		auto now =
		    std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch())
		        .count();
		durations.emplace_back(now - previous);
		previous = now;
	}

	long long all = 0;
	for (auto d : durations) {
		all += d;
	}

	auto max = std::max_element(durations.begin(), durations.end());
	std::cout << "Max duration: " << (*max) << " us, #" << std::distance(durations.begin(), max)
	          << " / " << durations.size() << "\n"
	          << "Average: " << (all / durations.size()) << " us\n";
}
