#include "Car.h"
#include "Logger.h"
#include "Engine.h"

static u32 clampListPos(i32 pos, u32 max) {
	if (pos < 0) {
		pos = max - 1;
	}
	if (pos > max) {
		pos = 1;
	} else if (pos > max - 1) {
		pos = 0;
	}
	return u32(pos);
}

static void displaySpline(i32 pos, const Vec<Spline>& splines) {
	Spline spn = splines[pos];

	glm::vec2 last = spn.p1;

	const glm::vec4 col = glm::vec4(1.0f);

	const u32 loops = 4;
	for (u32 i = 1; i <= loops; i++) {
		float t = float(i) / float(loops);
		glm::vec2 pos = spn.get(t);
		DebugDraw::get().line(glm::vec3(last, 0.0f), glm::vec3(pos, 0.0f), col);
		last = pos;
	}
}

void CarBehavior::onCreate() {
	Car* own = dynamic_cast<Car*>(owner());

	own->body()->SetType(b2_dynamicBody);
	own->setBoxShape(0.5f, 0.25f);

	acceleration = 0.0f;
	currentWaypoint = 0;

	offset = (Utils::random() * 2.0f - 1.0f) * 1.5f;
}

void CarBehavior::onDestroy() {}

void CarBehavior::onUpdate(float delta) {
	Car* own = dynamic_cast<Car*>(owner());

	glm::vec2 vel = own->linearVelocity();
	float velLen = glm::length(vel);
	glm::vec2 fwd = own->forward();

	glm::vec2 speed = fwd * acceleration * delta;
	own->applyForce(speed);

	if (velLen > 0.0005f) {
		own->applyForce(-glm::normalize(vel) * braking * delta);
	}

	float dir = glm::dot(vel, own->relativeVector(glm::vec2(1.0f, 0.0f)));
	float st = steering * (velLen / 5.0f) * delta;
	float rot = own->rotation();
	if (dir >= 0.0f) {
		rot += st;
		//own->applyTorque(st);
	} else {
		rot -= st;
		//own->applyTorque(-st);
	}
	own->rotation(rot);

	glm::vec2 forward{ 0.5f, 0.0f };
	float steerRightAngl = own->angularVelocity() > 0.0f ? -90.0f : 90.0f;

	glm::vec2 raFwd = glm::rotate(forward, glm::radians(steerRightAngl));

	float driftForce = glm::dot(vel, own->relativeVector(glm::normalize(raFwd)));
	glm::vec2 relativeForce = own->relativeVector((glm::normalize(raFwd) * -1.0f) * (driftForce * 10.0f));

	own->applyForce(relativeForce);

	if (!waypoints.empty()) {
		// Draw waypoints
		const glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		const glm::vec4 yel(1.0f, 1.0f, 0.0f, 1.0f);
		const glm::vec4 blu(0.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);

		for (u32 i = 0; i < waypoints.size(); i++) {
			displaySpline(i, waypoints);
			DebugDraw::get().dot(glm::vec3(waypoints[i].p0, 0.0f), yel, 0.2f);
			DebugDraw::get().dot(glm::vec3(waypoints[i].p1, 0.0f), yel, 0.2f);
			DebugDraw::get().dot(glm::vec3(waypoints[i].p2, 0.0f), yel, 0.2f);
			DebugDraw::get().dot(glm::vec3(waypoints[i].p3, 0.0f), yel, 0.2f);
		}

		u32 splineID = u32(trackProgress);
		Spline spn = waypoints[splineID];
		float splineT = trackProgress - splineID;
		currentGuidePosition = spn.get(splineT);
		glm::vec2 prevGuidePos = spn.get(splineT - delta);

		glm::vec2 inoutOffset = glm::normalize(prevGuidePos - currentGuidePosition);
		inoutOffset = glm::vec2(-inoutOffset.y, inoutOffset.x) * offset;

		currentGuidePosition += inoutOffset;

		DebugDraw::get().dot(glm::vec3(currentGuidePosition, 0.0f), red, 0.4f);

		glm::vec2 vec = currentGuidePosition - glm::vec2(own->position());
		float dist = glm::length(vec);
		if (dist <= GUIDE_MAX_DIST) {
			trackProgress += velLen * delta;
			if (trackProgress >= waypoints.size()) {
				trackProgress = 0.0f;
			}
		}
		//

		waypointSide = glm::dot(own->right(), glm::normalize(vec));

		DebugDraw::get().line(own->position(), own->position() + glm::vec3(glm::normalize(vec), 0.0f), blu);
	}

	acceleration = 0.0f;
	steering *= 0.2f;
	braking *= 0.5f;

}

void CarBehavior::setWaypoints(const Vec<glm::vec2>& points) {
	LogAssert(points.size() >= 4, "Invalid point count for spline.");

	waypoints.clear();
	for (u32 i = 0; i < points.size(); i++) {
		Spline spn{};
		spn.p0 = points[clampListPos(i - 1, points.size())];
		spn.p1 = points[i];
		spn.p2 = points[clampListPos(i + 1, points.size())];
		spn.p3 = points[clampListPos(i + 2, points.size())];
		waypoints.push_back(spn);
	}
}

Car::Car() : GameObject() {
	m_tint = glm::vec4(
		std::min(Utils::random() + 0.2f, 1.0f),
		std::min(Utils::random() + 0.2f, 1.0f),
		std::min(Utils::random() + 0.2f, 1.0f),
		1.0f
	);

	loadSkin();

	auto&& am = Engine::get()->assetManager();
	floor = am->getTexture("textures/floor.tga"); 
}

void Car::render(RenderContext *ctx) {
	ctx->cursor()
		.region(glm::vec4(0, 0, 32, 32))
		.position(glm::vec3(0.0f, 0.0f, -0.01f))
		.rotation(0)
		.scale(glm::vec2(32.0f));
	ctx->submitSprite(floor, Texture2D(), Texture2D());

	ctx->cursor()
		.region(glm::vec4(0, 0, 1, 1))
		.scale(glm::vec2(1.0f))
		.position(worldPosition())
		.rotation(worldRotation() + glm::radians(90.0f));
	ctx->submitSprite(m_color, m_normal, m_specular, m_tint);

	const float spotFac = 0.1f;
	glm::vec2 spotPos = forward() * spotFac + glm::vec2(worldPosition());

	ctx->submitSpotLight(
		glm::vec3(spotPos, 0.02f),
		glm::vec3(forward(), -0.35f),
		glm::vec3(1.0f, 0.9f, 0.9f),
		1.0f,
		6.0f,
		0.6f
	);

	const u32 lights = 10;
	for (u32 i = 0; i < lights; i++) {
		float fac = float(i) / float(lights);
		ctx->submitPointLight(
			glm::vec3(i * 2.0f - float(lights), std::sin(fac * 10.0f) * float(lights), 1.2f),
			glm::vec3(fac, 0.6f, 1.0f - fac),
			1.3f,
			6.0f
		);
	}
}

void Car::loadSkin(const String& name) {
	auto&& am = Engine::get()->assetManager();
	m_color = am->getTexture("textures/cars/" + name + "/color.tga");
	m_normal = am->getTexture("textures/cars/" + name + "/normal.tga");
	m_specular = am->getTexture("textures/cars/" + name + "/specular.tga");
}

void CarController::onUpdate(float delta) {
	auto&& input = Engine::get()->input();

	if (input.isKeyDown(Key::KeyW)) {
		acceleration = 100.0f;
	} else if (input.isKeyDown(Key::KeyS)) {
		acceleration = -60.0f;
	}

	if (input.isKeyDown(Key::KeyA)) {
		steering = 2.0f;
	} else if (input.isKeyDown(Key::KeyD)) {
		steering = -2.0f;
	}

	if (input.isKeyDown(Key::Space)) {
		braking = 140.0f;
	}
	
	CarBehavior::onUpdate(delta);
}

void CarAI::onUpdate(float delta) {
	Car* own = dynamic_cast<Car*>(owner());

	if (waypointSide < 0.0f) { // LEFT
		steering = -2.0f;
	} else if (waypointSide > 0.0f) { // RIGHT
		steering = 2.0f;
	}

	if (!waypoints.empty()) {
		acceleration = 40.0f;

		glm::vec2 vec = (currentGuidePosition - glm::vec2(own->position()));
		if (glm::length(vec) <= 5.0f) {
			braking = 30.0f;
		}
	}

	CarBehavior::onUpdate(delta);
}