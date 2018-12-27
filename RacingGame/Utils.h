#ifndef UTILS_H
#define UTILS_H

#include "Collections.h"
#include <iterator>
#include <sstream>

class Utils {
public:

	template<typename Out>
	static void split(const String &s, char delim, Out result) {
		std::stringstream ss(s);
		String item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	static Vec<String> split(const String &s, char delim);

	template<typename... Args>
	static String concat(Args&&... args) {
		std::stringstream ss;
		(ss << ... << args);
		return ss.str();
	}

	static String currentDateTime(const String& fmt = "%m/%d/%Y %X");

	static float random();
	static double currentTime();

};

#define FPS_COUNTER_SAMPLES 32
class FPSCounter {
public:
	FPSCounter() : m_sample(0), m_time(0) {}
	void update(float deltaTime);

	float get() const;

private:
	float m_fps, m_time;
	float m_samples[FPS_COUNTER_SAMPLES];
	u32 m_sample;
};

#endif // UTILS_H