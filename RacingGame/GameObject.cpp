#include "GameObject.h"

#include "Scene.h"
#include "Logger.h"

void GameObject::update(float delta) {
	if (m_dead) { return; }

	if (m_life != -1) {
		m_life -= delta;
		if (m_life <= 0.0f) {
			for (auto&& b : m_behaviors) {
				b->onDestroy();
			}
			if (m_body != nullptr) {
				m_scene->physicsWorld()->DestroyBody(m_body);
				m_body = nullptr;
			}
			m_dead = true;
		}
	}

	if (m_firstTime) {
		glm::vec3 pos = m_position;
		b2BodyDef bdef;
		bdef.type = b2_staticBody;
		bdef.position.Set(pos.x, pos.y);
		bdef.angle = m_rotation;
		bdef.userData = this;
		m_body = m_scene->physicsWorld()->CreateBody(&bdef);
	}

	if (m_body != nullptr) {
		auto&& pos = m_body->GetPosition();
		m_position.x = pos.x;
		m_position.y = pos.y;
		m_rotation = m_body->GetAngle();
	}

	for (auto&& b : m_behaviors) {
		if (m_firstTime) {
			b->onCreate();
		}
		b->onUpdate(delta);
	}

	if (m_firstTime) {
		m_firstTime = false;
	}

}

void GameObject::addBehavior(Behavior *behavior) {
	behavior->m_owner = this;
	m_behaviors.push_back(UPtr<Behavior>(behavior));
}

void GameObject::position(const glm::vec3& v) { 
	m_position = v;
	if (m_body != nullptr) {
		m_body->SetTransform(b2Vec2(v.x, v.y), m_rotation);
	}
}

void GameObject::rotation(float v) {
	m_rotation = v;
	if (m_body != nullptr) {
		m_body->SetTransform(b2Vec2(m_position.x, m_position.y), v);
	}
}

glm::vec3 GameObject::worldPosition() const {
	if (m_parent == nullptr) {
		return m_position;
	}

	glm::vec3 ppos = m_parent->worldPosition();
	float prot = m_parent->worldRotation();
	return glm::rotateZ(m_position, prot) + ppos;
}

float GameObject::worldRotation() const {
	float rot = m_rotation;
	if (m_parent != nullptr) {
		rot += m_parent->worldRotation();
	}
	return rot;
}

glm::vec2 GameObject::forward() const {
	float rot = worldRotation();
	return glm::vec2(std::cos(rot), std::sin(rot));
}

glm::vec2 GameObject::right() const {
	glm::vec2 fwd = forward();
	return glm::vec2(-fwd.y, fwd.x);
}

void GameObject::setBoxShape(float width, float height) {
	b2PolygonShape shape;
	shape.SetAsBox(
		width * m_scale.x,
		height * m_scale.y
	);
	createFixture(&shape);
}

void GameObject::applyForce(const glm::vec2& force) {
	if (m_body != nullptr) {
		m_body->ApplyForce(b2Vec2(force.x, force.y), m_body->GetWorldCenter(), true);
	}
}

void GameObject::applyTorque(float torque) {
	if (m_body != nullptr) {
		m_body->ApplyTorque(torque, true);
	}
}

glm::vec2 GameObject::linearVelocity() const {
	glm::vec2 vel{ 0.0f };
	if (m_body != nullptr) {
		vel.x = m_body->GetLinearVelocity().x;
		vel.y = m_body->GetLinearVelocity().y;
	}
	return vel;
}

glm::vec2 GameObject::relativeVector(const glm::vec2& v) const {
	glm::vec2 ret{ 0.0f };
	if (m_body != nullptr) {
		auto&& vec = m_body->GetWorldVector(b2Vec2(v.x, v.y));
		ret.x = vec.x;
		ret.y = vec.y;
	}
	return ret;
}

glm::vec2 GameObject::localPoint(const glm::vec2& wp) const {
	glm::vec2 ret{ 0.0f };
	if (m_body != nullptr) {
		auto&& vec = m_body->GetLocalPoint(b2Vec2(wp.x, wp.y));
		ret.x = vec.x;
		ret.y = vec.y;
	}
	return ret;
}

void GameObject::sensor(float enable) {
	if (m_body == nullptr) return;
	for (b2Fixture* f = m_body->GetFixtureList(); f; f = f->GetNext()) {
		f->SetSensor(enable);
	}
}

class RayCastCallback : public b2RayCastCallback {
public:
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		this->point = glm::vec2(point.x, point.y);
		this->normal = glm::vec2(normal.x, normal.y);
		this->object = static_cast<GameObject*>(fixture->GetBody()->GetUserData());
		return fraction;
	}

	glm::vec2 point, normal;
	GameObject *object;
};

GameObject::RayCastResult GameObject::rayCast(const glm::vec2& to, float dist) {
	GameObject::RayCastResult res{};
	if (m_body == nullptr) {
		return res;
	}

	glm::vec3 pos = worldPosition();
	glm::vec2 _to = glm::normalize(to - glm::vec2(pos)) * dist;
	b2Vec2 p1 = b2Vec2(pos.x, pos.y);
	b2Vec2 p2 = b2Vec2(_to.x, _to.y);

	RayCastCallback cb{};
	m_scene->physicsWorld()->RayCast(&cb, p1, p2);

	res.normal = cb.normal;
	res.point = cb.point;
	res.object = cb.object;

	return res;
}

void GameObject::createFixture(b2Shape *shape) {
	b2FixtureDef fixtureDef{};
	fixtureDef.shape = shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.6f;
	fixtureDef.restitution = 1.0f;
	m_body->CreateFixture(&fixtureDef);
}
