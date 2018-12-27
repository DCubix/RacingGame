#ifndef SPLINE_H
#define SPLINE_H

#include "glm/vec2.hpp"

struct Spline {
	glm::vec2 p0, p1, p2, p3;

	glm::vec2 get(float t);

private:
	float getSingle(float p0, float p1, float p2, float p3, float t);
};

#endif // SPLINE_H