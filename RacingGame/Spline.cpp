#include "Spline.h"

glm::vec2 Spline::get(float t) {
	return glm::vec2(
		getSingle(p0.x, p1.x, p2.x, p3.x, t),
		getSingle(p0.y, p1.y, p2.y, p3.y, t)
	);
}

float Spline::getSingle(float p0, float p1, float p2, float p3, float t) {
	return 0.5f *
		((2 * p1) +
		(p2 - p0) * t +
		(2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
		(3 * p1 - p0 - 3 * p2 + p3) * t * t * t);
}
