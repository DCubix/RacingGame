#include "Scene.h"

#include "Logger.h"

Scene::~Scene() {
	
}

Scene::Scene() {
	
}

void Scene::update(float dt) {
	for (auto&& obj : m_addList) {
		m_objects.push_back(std::move(obj));
	}
	m_addList.clear();

	m_physicsWorld->Step(dt, 10, 5);

	Vec<u32> m_dead;
	u32 i = 0;
	for (auto&& obj : m_objects) {
		obj->update(dt);
		if (obj->m_dead) {
			m_dead.push_back(i);
		}
		i++;
	}

	for (u32 id : m_dead) {
		m_objects.erase(m_objects.begin() + id);
	}
}

void Scene::render(RenderContext *context) {
	for (auto&& obj : m_objects) {
		obj->render(context);
	}

#ifdef _DEBUG
	m_physicsWorld->DrawDebugData();
#endif
}

void Scene::destroy() {
	m_objects.clear();
	m_addList.clear();
	m_debugDraw.release();
	m_physicsWorld.release();
}

void Scene::add(GameObject *obj) {
	if (obj == nullptr) return;
	obj->m_scene = this;
	obj->m_firstTime = true;
	m_addList.push_back(UPtr<GameObject>(obj));
}

void Scene::BeginContact(b2Contact* contact) {
	Collision col;
	col.objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
	col.objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());

	b2Vec2 n = contact->GetManifold()->localNormal;
	b2Vec2 p = contact->GetManifold()->localPoint;

	col.normal = glm::vec2(n.x, n.y);
	col.point = glm::vec2(p.x, p.y);

	for (auto&& b : col.objectA->m_behaviors) {
		b->onCollide(col);
	}
	for (auto&& b : col.objectB->m_behaviors) {
		b->onCollide(col);
	}
}

void Scene::initPhysics() {
	m_physicsWorld = UPtr<b2World>(new b2World(b2Vec2(0.0f, 0.0f)));
	m_physicsWorld->SetAllowSleeping(true);
	m_physicsWorld->SetContinuousPhysics(true);
	m_physicsWorld->SetContactListener(this);

	m_debugDraw = UPtr<PhysicsDebugDraw>(new PhysicsDebugDraw());
	m_debugDraw->ppm = 1.0f;

	u32 flags = 0;
	flags |= b2Draw::e_shapeBit;
	flags |= b2Draw::e_jointBit;
	flags |= b2Draw::e_aabbBit;
	flags |= b2Draw::e_pairBit;
	flags |= b2Draw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);

	m_physicsWorld->SetDebugDraw(m_debugDraw.get());
}

SceneManager::SceneManager() {
	m_currentScene = "";
	m_nextScene = "";
	m_changingScenes = false;
}

void SceneManager::registerScene(const String& name, Scene *scene) {
	if (m_scenes.find(name) != m_scenes.end()) return;
	m_scenes.insert({ name, UPtr<Scene>(scene) });
	if (m_currentScene.empty()) {
		setScene(name);
	}
}

void SceneManager::setScene(const String& name) {
	if (m_scenes.find(name) == m_scenes.end()) return;
	m_nextScene = name;
	m_changingScenes = true;
}

void SceneManager::update(float dt) {
	if (m_scenes.empty()) return;
	if (m_changingScenes) {
		if (!m_currentScene.empty()) {
			current()->destroy();
		}
		m_currentScene = m_nextScene;
		if (!m_currentScene.empty()) {
			current()->initPhysics();
			current()->create();
		}
		m_changingScenes = false;
	} else {
		if (!m_currentScene.empty())
			m_scenes[m_currentScene]->update(dt);
	}
}

void SceneManager::render(RenderContext* context) {
	if (m_scenes.empty() || m_currentScene.empty()) return;
	if (!m_changingScenes) {
		m_scenes[m_currentScene]->render(context);
	}
}