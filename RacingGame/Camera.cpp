#include "Camera.h"
#include "Engine.h"
#include "Logger.h"

Camera::Camera() {
	m_smoothing = 0.8f;
	m_zoom = 1.0f;
	m_tag = "CAM";
	m_target = nullptr;
}

void Camera::render(RenderContext *ctx) {
	auto&& win = Engine::get()->window();

	float asp = float(win->width()) / float(win->height());
	float z = glm::clamp(m_zoom, 1.0f, 2.0f) * 4.0f;

	ctx->projection(glm::perspective(glm::radians(50.0f), asp, 0.01f, 1000.0f));
	ctx->view(glm::translate(glm::mat4(1.0f), glm::vec3(-m_position.x, -m_position.y, -z)));
}

void Camera::update(float delta) {
	GameObject::update(delta);

	if (m_target != nullptr) {
		float vel = glm::length(m_target->linearVelocity());
		m_zoom = glm::mix(m_zoom, 1.0f + vel * 5.0f, 0.025f);

		glm::vec2 speed(m_target->position() - m_position);
		speed *= ((1.0f - m_smoothing) + 0.01f);

		position(position() + glm::vec3(speed, 0.0f));
	}
}
