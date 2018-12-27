#include "Utils.h"
#include "Logger.h"

#include <ctime>

typedef int64_t msec_t;
#if defined(_WIN32)
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

msec_t time_ms(void) {
	return timeGetTime();
}

#else
#include <sys/time.h>

msec_t time_ms(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (msec_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif


Vec<String> Utils::split(const String& s, char delim) {
	Vec<String> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

String Utils::currentDateTime(const String& fmt) {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), fmt.c_str(), &tstruct);

	return String(buf);
}

float Utils::random() {
	return float(rand() % RAND_MAX) / RAND_MAX;
}

double Utils::currentTime() {
	return double(time_ms()) / 1000.0;
}

void FPSCounter::update(float deltaTime) {
	m_samples[m_sample++ % FPS_COUNTER_SAMPLES] = 1.0f / deltaTime;

	m_fps = 0.0f;
	for (u32 i = 0; i < FPS_COUNTER_SAMPLES; i++) m_fps += m_samples[i];
	m_fps /= FPS_COUNTER_SAMPLES;
}

float FPSCounter::get() const {
	return m_fps;
}
