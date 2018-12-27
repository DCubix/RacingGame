#ifndef SCENE_H
#define SCENE_H

#include "DebugDraw.h"
#include "GameObject.h"

class Scene : public b2ContactListener {
	friend class GameObject;
	friend class SceneManager;
public:
	~Scene();
	Scene();

	virtual void create() {}
	void update(float dt);
	void render(RenderContext *context);
	void destroy();

	void add(GameObject *obj);

	b2World* physicsWorld() { return m_physicsWorld.get(); }

	// Box2D
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact) { }

private:
	Vec<UPtr<GameObject>> m_objects, m_addList;

	UPtr<PhysicsDebugDraw> m_debugDraw;

	// Physics
	UPtr<b2World> m_physicsWorld;
	void initPhysics();
};

class SceneManager {
public:
	SceneManager();
	~SceneManager() = default;

	void registerScene(const String& name, Scene* scene);
	void setScene(const String& name);

	void update(float dt);
	void render(RenderContext *context);

	Scene* current() { return !m_scenes.empty() ? m_scenes[m_currentScene].get() : nullptr; }

private:
	UMap<String, UPtr<Scene>> m_scenes;
	String m_nextScene, m_currentScene;
	bool m_changingScenes;
};

#endif // SCENE_H