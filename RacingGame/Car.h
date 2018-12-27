#ifndef CAR_H
#define CAR_H

#include "GameObject.h"
#include "Spline.h"

#define GUIDE_MAX_DIST 2.0f

class CarBehavior : public Behavior {
public:
	CarBehavior() : trackProgress(0.0f) {}

	void onCreate();
	void onDestroy();
	virtual void onUpdate(float delta);

	float acceleration, steering, braking;

	void setWaypoints(const Vec<glm::vec2>& points);

	Vec<Spline> waypoints;
	glm::vec2 currentGuidePosition;
	u32 currentWaypoint;
	float trackProgress, waypointSide, offset;
};

class CarController : public CarBehavior {
public:
	void onUpdate(float delta) override;
};

class CarAI : public CarBehavior {
public:
	void onUpdate(float delta) override;
};

class Car : public GameObject {
	friend class CarBehavior;
public:
	Car();
	~Car() = default;

	void render(RenderContext *context) override;

	void loadSkin(const String& name = "default");

	Texture2D colorTexture() const { return m_color; }
	void colorTexture(Texture2D tex) { m_color = tex; }

	Texture2D normalTexture() const { return m_normal; }
	void normalTexture(Texture2D tex) { m_normal = tex; }

	Texture2D specularTexture() const { return m_specular; }
	void specularTexture(Texture2D tex) { m_specular = tex; }

	glm::vec4 tint() const { return m_tint; }
	void tint(const glm::vec4& v) { m_tint = v; }

	Texture2D floor;
protected:
	Texture2D m_color, m_normal, m_specular;
	glm::vec4 m_tint;
};

#endif // CAR_H