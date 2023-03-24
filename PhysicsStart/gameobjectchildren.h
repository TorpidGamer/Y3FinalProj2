#pragma once
#include "common.h"
#include "gameobject.h"

class Goal : public GameObject {
public :
	Goal(Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 bbDimensions, string nextScene) : GameObject(mesh, position, rotation, scale, bbDimensions, name) { this->isTrigger = true; this->staticObj = true; this->nextScene = nextScene; }
	Goal(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 bbDimensions, string nextScene) : GameObject(model, position, rotation, scale, bbDimensions, name) { this->isTrigger = true; this->staticObj = true; this->nextScene = nextScene; }
	Goal(glm::vec3 bbDimensions, string nextScene) : GameObject(bbDimensions, name){ this->isTrigger = true; this->staticObj = true; this->nextScene = nextScene; }

	string nextScene;
	virtual void OnOverlap(GameObject* other) override{
		cout << nextScene << endl;
	}
};