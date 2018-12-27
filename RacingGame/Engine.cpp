#include "Engine.h"

#include "DebugDraw.h"

UPtr<Engine> Engine::s_instance;

Engine::Engine() {}

Engine* Engine::get() {
	if (s_instance == nullptr) {
		s_instance = UPtr<Engine>(new Engine());
	}
	return s_instance.get();
}

void Engine::start(Application *app, const String& title, u32 width, u32 height) {
	if (app == nullptr) return;

	srand(time(nullptr));

	ContextAttribs attrs = DEFAULT_ATTRIBS;
	attrs.multisample.buffers = 1;
	attrs.multisample.samples = 8;
	m_window = UPtr<Window>(new Window(title, width, height, attrs));
	gladLoadGL();

	m_renderContext = UPtr<RenderContext>(new RenderContext());
	m_sceneManager = UPtr<SceneManager>(new SceneManager());
	m_assetManager = UPtr<AssetManager>(new AssetManager());

	const double timeStep = 1.0 / 60;
	double lastTime = Utils::currentTime();
	double accum = 0.0;
	u32 frames = 0;
	double frameTime = 0.0;

	Event evt;

#ifdef _DEBUG
	String originalTitle = m_window->title();
#endif

	DebugDraw::get().init();

	app->onPreLoad();
	m_assetManager->load();
	app->onInit();
	while (!m_window->shouldClose()) {
		bool canRender = false;
		double currentTime = Utils::currentTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		accum += delta;
		frameTime += delta;

		if (frameTime >= 1.0) {
#ifdef _DEBUG
			m_window->title(
				Utils::concat(
					originalTitle, " | ",
					std::to_string(frames), "fps | ",
					(delta * 1000.0), "ms"
				)
			);
#endif
			frameTime = 0.0;
			frames = 0;
		}

		m_input.update(m_window.get());

		while (accum >= timeStep) {
			float dt = float(timeStep);

			app->onUpdate(dt);
			m_sceneManager->update(dt);

			accum -= timeStep;
			canRender = true;
		}

		if (canRender) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_renderContext->begin();
			m_sceneManager->render(m_renderContext.get());
			m_renderContext->end();

			m_renderContext->begin();
			app->onRender(m_renderContext.get());
			m_renderContext->end();

			DebugDraw::get().flush(m_renderContext->projection() * m_renderContext->view());

			m_window->swapBuffers();
			frames++;
		}
	}
	app->onExit();
	delete app;
}

void Input::update(Window *win) {
	for (u32 i = 0; i < 0xFF; i++) {
		m_keyboard[i].press = m_keyboard[i].release = false;
	}

	for (u32 i = 0; i < 3; i++) {
		m_mouse[i].press = m_mouse[i].release = false;
	}

	Event evt{};
	while (win->popEvent(evt)) {
		switch (evt.type) {
			case EventType::KeyDowm:
			{
				m_keyboard[evt.key].press = true;
				m_keyboard[evt.key].down = true;
			} break;
			case EventType::KeyUp:
			{
				m_keyboard[evt.key].release = true;
				m_keyboard[evt.key].down = false;
			} break;
			case EventType::MouseButtonDown:
			{
				m_mouse[evt.mouseButton].press = true;
				m_mouse[evt.mouseButton].down = true;
				m_mousePosition.x = evt.mouseX;
				m_mousePosition.y = evt.mouseY;
			} break;
			case EventType::MouseButtonUp:
			{
				m_mouse[evt.mouseButton].release = true;
				m_mouse[evt.mouseButton].down = false;
				m_mousePosition.x = evt.mouseX;
				m_mousePosition.y = evt.mouseY;
			} break;
			case EventType::MouseMove:
			{
				m_mousePosition.x = evt.mouseX;
				m_mousePosition.y = evt.mouseY;
			} break;
		}
	}
}
