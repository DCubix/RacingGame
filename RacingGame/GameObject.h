#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "RenderContext.h"
#include "Window.h"
#include "Logger.h"

#include "Box2D/Box2D.h"

class GameObject;
struct Collision {
	glm::vec2 normal, point;
	GameObject *objectA, *objectB;
};

class Behavior {
	friend class GameObject;
public:
	virtual void onCreate() = 0;
	virtual void onDestroy() = 0;
	virtual void onUpdate(float delta) = 0;

	virtual void onCollide(Collision collision) { }
	
	GameObject* owner() { return m_owner; }

private:
	GameObject *m_owner;
};

class Scene;
class GameObject {
	friend class Scene;
public:
	struct RayCastResult {
		GameObject *object;
		glm::vec2 normal, point;
	};

	GameObject() 
		: m_firstTime(true), m_scale(glm::vec2(1.0f)),
		m_rotation(0.0f), m_dead(false), m_life(-1),
		m_body(nullptr)
	{}

	virtual void render(RenderContext *context) {}
	virtual void update(float delta);

	void kill(float life = 0.0f) { m_life = life; }

	void addBehavior(Behavior *behavior);

	void position(const glm::vec3& v);
	glm::vec3 position() const { return m_position; }

	void scale(const glm::vec2& v) { m_scale = v; }
	glm::vec2 scale() const { return m_scale; }

	void rotation(float v);
	float rotation() const { return m_rotation; }

	void parent(GameObject *parent) { m_parent = parent; }
	GameObject* parent() { return m_parent; }

	glm::vec3 worldPosition() const;
	float worldRotation() const;

	glm::vec2 forward() const;
	glm::vec2 right() const;

	Scene* scene() { return m_scene; }

	void tag(const String& t) { m_tag = t; }
	String tag() const { return m_tag; }

	b2Body* body() { return m_body; }
	void setBoxShape(float width, float height);

	void applyForce(const glm::vec2& force);
	void applyTorque(float torque);

	glm::vec2 linearVelocity() const;
	glm::vec2 relativeVector(const glm::vec2& v) const;
	glm::vec2 localPoint(const glm::vec2& wp) const;
	float angularVelocity() const { return m_body != nullptr ? m_body->GetAngularVelocity() : 0.0f; }

	void sensor(float enable);

	RayCastResult rayCast(const glm::vec2& to, float dist = 1.0f);

	template <class T>
	T* getBehavior() {
		for (auto&& b : m_behaviors) {
			T* bc = dynamic_cast<T*>(b.get());
			if (bc != nullptr) {
				return bc;
			}
		}
		return nullptr;
	}

protected:
	glm::vec3 m_position;
	glm::vec2 m_scale;
	float m_rotation, m_life;

	Vec<UPtr<Behavior>> m_behaviors;
	bool m_firstTime, m_dead;

	GameObject *m_parent;
	Scene *m_scene;

	String m_tag;

	// Physics
	b2Body *m_body;

	void createFixture(b2Shape *shape);
};

#endif // GAME_OBJECT_H