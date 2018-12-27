#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"

class Camera : public GameObject {
public:
	Camera();
	~Camera() = default;

	void render(RenderContext *context) override;
	void update(float delta) override;

	float smoothing() const { return m_smoothing; }
	void smoothing(float s) { m_smoothing = s; }

	float zoom() const { return m_zoom; }
	void zoom(float z) { m_zoom = z; }

	void target(GameObject *t) { m_target = t; }
	GameObject* target() { return m_target; }

private:
	float m_zoom, m_smoothing;
	GameObject *m_target;
};

#endif
