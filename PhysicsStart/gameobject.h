#pragma once
#ifndef GAMEOBJ_H
#define GAMEOBJ_H

#include "model.h"
#include "primitives.h"

class GameObject {
public:
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 prevPos = position;
	glm::vec3 rotation = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);
	glm::vec3 velocity = glm::vec3(0.f);

	string name;
	GameObject* parent;
	vector<GameObject*> children;
	Model* model;


	int meshSize;
	bool transparent = false;
	bool resolveCollisions = false;
	bool staticObj = false;
	bool gravity = true;
	bool wasGrav = true;
	bool isBottom = false;
	bool isTrigger = false;
	bool render = true;
	bool isGrounded = false;
	float gravityForce = -2.f;
	unsigned int VAO;
	bool hasPrimitive;
	int drawBBs = 1;

	glm::mat4 modelMatrix = glm::mat4(0.f);

	float bbWidth = 0.25f, bbHeight = 0.25f, bbDepth = 0.25f;
	glm::vec3 RTF, LBB;
	glm::vec3 RightTopFront, LeftBottomBack;
	glm::vec3 collisionNormalLine = glm::vec3(0);
	vector<glm::vec3> bounds; //0 1
	vector<string> collidingWith;
	Mesh boundingBox, collisionLineMesh;
	
	GameObject(Model *model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 bbDimensions, string name);
	GameObject(string name);
	//~GameObject();

	virtual void Update(float deltaTime);
	virtual void OnOverlap(GameObject* other);
	void Render(Shader& complexShader);

	void Initialise(glm::vec3 position,	glm::vec3 rotation, glm::vec3 scale, glm::vec3 bbDimensions, string name);

	void LoopVertices();
	void Collisions(GameObject* other, float deltaTime);
	void HandleCollision(int side, GameObject* other);
	void CalculateBounds();
	void CountVertices();

	void Translate(glm::vec3 newPos);
	void Translate(float x, float y, float z);
	void Rotate(glm::vec3 newRotation);
	void Rotate(float x, float y, float z);
	void Scale(glm::vec3 newScale);
	void Scale(float x, float y, float z);
	glm::mat4 ApplyRotation(glm::mat4 matrix, glm::vec3 rotation);
	glm::mat4 CalculateMatrix();
private:
	const static int numOf = 0;

	bool noMesh;
};
#endif