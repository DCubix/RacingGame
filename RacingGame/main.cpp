#include "Engine.h"
#include "Car.h"
#include "Camera.h"

class MainScene : public Scene {
public:
	void create() {
		auto&& am = Engine::get()->assetManager();
		m_car = new Car();
		m_car->addBehavior(new CarAI());
		m_car->loadSkin("bmw850");
		add(m_car);

		CarBehavior* cb = m_car->getBehavior<CarBehavior>();
		Vec<glm::vec2> waypoints;
		const u32 step = (360 / 20);
		for (u32 i = 0; i < 360; i += step) {
			float a = glm::radians(float(i));
			float r = std::sin(a * 5.0f);
			float x = std::cos(a) * (10.0f + r);
			float y = std::sin(a) * (8.0f + r);
			waypoints.push_back(glm::vec2(x, y));
		}
		cb->setWaypoints(waypoints);

		m_camera = new Camera();
		m_camera->target(m_car);
		add(m_camera);
	}

	Car *m_car;
	Camera *m_camera;
};

class RacingGame : public Application {
public:
	void onPreLoad() {
		auto&& am = Engine::get()->assetManager();
		am->init("data.zip");
	}

	void onInit() {
		auto&& am = Engine::get()->assetManager();
		auto&& sm = Engine::get()->sceneManager();
		auto&& ctx = Engine::get()->renderContext();

		ctx->ambient(glm::vec3(0.06f));

		Texture2D reflTex = am->getTexture("textures/environment/default_reflection.tga");
		ctx->environment(reflTex);

		sm->registerScene("main", new MainScene());
	}

	void onRender(RenderContext *context) {
	}

	void onUpdate(float deltaTime) {
	}

	void onExit() {
	}
};

int main(int argc, char** argv) {
	Engine::get()->start(new RacingGame(), "Racing Game", 1024, 640);
	return 0;
}