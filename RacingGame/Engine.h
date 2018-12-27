#ifndef ENGINE_H
#define ENGINE_H

#include "Window.h"
#include "Memory.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Utils.h"
#include "AssetManager.h"

class Application {
public:
	virtual void onPreLoad() = 0;
	virtual void onInit() = 0;
	virtual void onRender(RenderContext *context) = 0;
	virtual void onUpdate(float deltaTime) = 0;
	virtual void onExit() = 0;
};

class Input {
	friend class Engine;
public:
	struct State {
		bool press, release, down;
	};

	bool isKeyDown(Key key) { return m_keyboard[u32(key)].down; }
	bool isKeyUp(Key key) { return m_keyboard[u32(key)].release; }
	bool isKeyPressed(Key key) { return m_keyboard[u32(key)].press; }

	bool isMouseDown(u32 btn) { return m_mouse[btn].down; }
	bool isMouseUp(u32 btn) { return m_mouse[btn].release; }
	bool isMousePressed(u32 btn) { return m_mouse[btn].press; }

	glm::vec2 mousePosition() const { return m_mousePosition; }


private:
	State m_keyboard[0xFF];
	State m_mouse[3];
	glm::vec2 m_mousePosition;

	void update(Window *win);
};

class Engine {
public:
	~Engine() = default;

	static Engine* get();

	void start(Application *app, const String& title, u32 width, u32 height);

	Window* window() { return m_window.get(); }
	SceneManager* sceneManager() { return m_sceneManager.get(); }
	AssetManager* assetManager() { return m_assetManager.get(); }
	RenderContext* renderContext() { return m_renderContext.get(); }
	Input& input() { return m_input; }

private:
	Engine();

	UPtr<SceneManager> m_sceneManager;
	UPtr<AssetManager> m_assetManager;
	UPtr<RenderContext> m_renderContext;
	UPtr<Window> m_window;
	Input m_input;

	static UPtr<Engine> s_instance;
};

#endif // ENGINE_H