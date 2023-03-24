#include "gameobject.h"

GameObject::GameObject(Mesh *mesh, glm::vec3 position = glm::vec3(0.f),
	glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f), glm::vec3 bbDimensions = glm::vec3(0.f), string name = "") {

	if (mesh) {
		this->mesh = mesh;
	}
	Initialise(position, rotation, scale, bbDimensions, name);
	LoopVertices();
}

GameObject::GameObject(Model* model, glm::vec3 position = glm::vec3(0.f),
	glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f), glm::vec3 bbDimensions = glm::vec3(0.f), string name = "") {
	this->model = model;
	Initialise(position, rotation, scale, bbDimensions, name);
	LoopVertices();
}
GameObject::GameObject(glm::vec3 bbDimensions = glm::vec3(0.f), string name = "") {
	noMesh = true;
	this->name = name;
	bbWidth = bbDimensions.x;
	bbHeight = bbDimensions.y;
	bbDepth = bbDimensions.z;
	LoopVertices();
}

/*GameObject::~GameObject() {
	if (model != nullptr)	delete model;
	if (mesh != nullptr && !noMesh) {
		if (mesh->vertices.size() > 0) {
			delete mesh;
		}
	}
}*/

void GameObject::Initialise(glm::vec3 position = glm::vec3(0.f),
	glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f), glm::vec3 bbDimensions = glm::vec3(0.f), string name = "") {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->name = name;
	bbWidth = bbDimensions.x;
	bbHeight = bbDimensions.y;
	bbDepth = bbDimensions.z;
}

void GameObject::Render(Shader &complexShader) {	
	//Non static objects
	if (!staticObj) {
		if (!noMesh) {
			if (model == nullptr) {
				complexShader.setMat4("model", CalculateMatrix());
				mesh->Draw(complexShader);
			}
			else {
				complexShader.setMat4("model", CalculateMatrix());
				model->Draw(complexShader);
			}
		}
	}
	//Static objects shouldnt need recalculated matrices per frame
	else {
		if (staticModel == glm::mat4(0.f)) {
			staticModel = CalculateMatrix();
		}
		if (!noMesh) {
			if (model == nullptr) {
				complexShader.setMat4("model", staticModel);
				mesh->Draw(complexShader);
			}
			else {
				complexShader.setMat4("model", staticModel);
				model->Draw(complexShader);
			}
		}
	}
	//Bounding Box Section
	glm::mat4 bbModelMat;
	glm::vec3 bbPos;
	if (bounds.empty()) {
		CalculateBounds();
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (drawBBs == 2) {
			glm::mat4 bbModelMat;
			for (unsigned int i = 0; i < bounds.size(); i += 2) {
				bbPos = glm::vec3((bounds[i].x + bounds[i + 1].x) / 2, (bounds[i].y + bounds[i + 1].y) / 2, (bounds[i].z + bounds[i + 1].z) / 2);
				bbModelMat = glm::mat4(1.f);
				bbModelMat = glm::translate(bbModelMat, bbPos);
				complexShader.setMat4("model", bbModelMat);
				boundingBox.Draw(complexShader);
			}
		}
		else if (drawBBs == 1) {
			bbPos = glm::vec3((bounds[0].x + bounds[1].x) / 2, (bounds[0].y + bounds[1].y) / 2, (bounds[0].z + bounds[1].z) / 2);
			bbModelMat = glm::mat4(1.f);
			bbModelMat = glm::translate(bbModelMat, bbPos);
			complexShader.setMat4("model", bbModelMat);
			boundingBox.Draw(complexShader);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void GameObject::Update(float deltaTime = 0.f) {
	prevPos = position;
	if (gravity && !staticObj) {
		velocity.y += gravityForce * deltaTime;
	}
	position += velocity;
	velocity = glm::vec3(0, velocity.y, 0);
	//position.y -= .8f * deltaTime;
	//cout << position.x << endl;
}

//For loop to find the minimum and maximum of the vertices, use imbetween for sides
void GameObject::LoopVertices() {
	float top, bottom, left, right, front, back;
	right = (bbWidth / 2);
	left = -(bbWidth / 2);
	top = (bbHeight / 2);
	bottom = -(bbHeight / 2);
	front = (bbDepth / 2);
	back = -(bbDepth / 2);
	if (!noMesh) {
		if (!model) {
			for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
				glm::vec3 posToCheck = mesh->vertices[i].Position;
				if (posToCheck.x > right) {
					right = posToCheck.x;
				}
				if (posToCheck.x < left) {
					left = posToCheck.x;
				}
				if (posToCheck.y > top) {
					top = posToCheck.y;
				}
				if (posToCheck.y < bottom) {
					bottom = posToCheck.y;
				}
				if (posToCheck.z > front) {
					front = posToCheck.z;
				}
				if (posToCheck.z < back) {
					back = posToCheck.z;
				}
				//cout << posToCheck.x << ", " << posToCheck.y << ", " << posToCheck.z << endl;
			}
		}
		else {
			for (unsigned int j = 0; j < model->meshes.size(); j++) {
				for (unsigned int i = 0; i < model->meshes[j].vertices.size(); i++) {
					glm::vec3 posToCheck = model->meshes[j].vertices[i].Position;
					if (posToCheck.x > right) {
						right = posToCheck.x;
					}
					if (posToCheck.x < left) {
						left = posToCheck.x;
					}
					if (posToCheck.y > top) {
						top = posToCheck.y;
					}
					if (posToCheck.y < bottom) {
						bottom = posToCheck.y;
					}
					if (posToCheck.z > front) {
						front = posToCheck.z;
					}
					if (posToCheck.z < back) {
						back = posToCheck.z;
					}
					//cout << posToCheck.x << ", " << posToCheck.y << ", " << posToCheck.z << endl;
				}
			}
		}
	}
	top *= scale.y;
	bottom *= scale.y;
	left *= scale.x;
	right *= scale.x;
	front *= scale.z;
	back *= scale.z;
	RightTopFront = glm::vec3(right, top, front);
	LeftBottomBack = glm::vec3(left, bottom, back);
	vector<Vertex> primitiveVertices = {
	Vertex(glm::vec3(left, top, front),	glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f)),
	Vertex(glm::vec3(right, top, front), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
	Vertex(glm::vec3(right, top, back),	glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
	Vertex(glm::vec3(left, top, back), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f)),

	Vertex(glm::vec3(left, bottom, back),	glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)),
	Vertex(glm::vec3(right, bottom, back),	glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
	Vertex(glm::vec3(right, bottom, front),	glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
	Vertex(glm::vec3(left, bottom, front),	glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, -1.f)),
	};

	vector<unsigned int> indices = {
	0, 1, 2,//Top
	0, 2, 3,

	7, 6, 1,//Front
	7, 1, 0,

	4, 5, 6,//Bottom
	4, 6, 7,

	3, 2, 5,//Back
	3, 5, 4,

	0, 7, 4,//Left
	0, 4, 3,

	1, 6, 5,//Right
	1, 5, 2
	};

	boundingBox = Mesh(primitiveVertices, indices);
}
void GameObject::CalculateBounds() {
	glm::vec3 nextPos = position + velocity;
	if (!staticObj || bounds.empty()) {
		RTF = RightTopFront;
		LBB = LeftBottomBack;
		float width = glm::distance(RTF.x, LBB.x);
		float height = glm::distance(RTF.y, LBB.y);
		float depth = glm::distance(RTF.z, LBB.z);
		//Width of center box in positional space so RTF + LBB / 2 + position
		bounds = {
			RTF + nextPos,
			LBB + nextPos,

			RTF + nextPos + glm::vec3(width, 0, 0),//Right
			LBB + nextPos + glm::vec3(width, 0, 0),

			RTF + nextPos - glm::vec3(width, 0, 0),//Left
			LBB + nextPos - glm::vec3(width, 0, 0),

			RTF + nextPos + glm::vec3(0, height, 0),//Top
			LBB + nextPos + glm::vec3(0, height, 0),

			glm::vec3(RTF.x / 4, RTF.y / 4, RTF.z / 4) + nextPos - glm::vec3(0, height, 0),//Bottom
			glm::vec3(LBB.x / 4, LBB.y / 4, LBB.z / 4) + nextPos - glm::vec3(0, height, 0),

			RTF + nextPos + glm::vec3(0, 0, depth),//Front
			LBB + nextPos + glm::vec3(0, 0, depth),

			RTF + nextPos - glm::vec3(0, 0, depth),//Back
			LBB + nextPos - glm::vec3(0, 0, depth),
		};
	}
}
void GameObject::Collisions(GameObject* other, float deltaTime) {
	//Make pre-emptive instead (place before update? - cancel the update if true? - get the side and cancel the side?)
	if (name == other->name) return;
	CalculateBounds();
	other->CalculateBounds();
	float collisionSpeed = 5;

	bool x, y, z;
	bool isBottom = false;
	for (unsigned int i = 0; i < bounds.size(); i += 2) {
		if (min(bounds[i].x, other->bounds[i].x) > max(bounds[i + 1].x, other->bounds[i + 1].x)) /*X overlap*/ x = true;
		else x = false;
		if (min(bounds[i].y, other->bounds[i].y) > max(bounds[i + 1].y, other->bounds[i + 1].y)) /*Y overlap*/ y = true;
		else y = false;
		if (min(bounds[i].z, other->bounds[i].z) > max(bounds[i + 1].z, other->bounds[i + 1].z)) /*Z overlap*/ z = true;
		else z = false;

		if (x || y || z) {
			if (i == 6) {
				isBottom = true;
			}
		}

		if (x && y && z) {
			if (i == 6) {
				if (this->name == "player") {
					//cout << "bottom" << endl;
				}
				isBottom = true;
			}
			if (i == 0) {
				if (!isTrigger) {
					isBottom = true;
					resolveCollisions = true;
					if (std::find(collidingWith.begin(), collidingWith.end(), other->name) == collidingWith.end()) {
						if (this->name == "player") cout << name << ": Begin Overlap with: " << other->name << endl;
						collidingWith.push_back(other->name);
					}
				}
				else {
					OnOverlap(other);
				}
				//continue;
			}
			HandleCollision(i, other);
		}
		for (unsigned int c = 0; c < collidingWith.size(); c++) {
			if (other->name == collidingWith[c]) {
				if (resolveCollisions && (i == 0) && (!x || !y || !z)) {
					resolveCollisions = false;
					//if (wasGrav) gravity = true;
					//cout << name << ": No longer touching: " << other->name << endl;
					collidingWith.erase(collidingWith.begin() + c);
				}
			}
		}
	}
	if (!isBottom) {
		//cout << "NotBottom" << endl;
		//gravity = true;
	}
}
void GameObject::HandleCollision(int side, GameObject* other) {
	if (!staticObj) {
		float collisionSpeed = 6.f;
		if (resolveCollisions && other->resolveCollisions) {
			//cout << side << endl;
			position = prevPos;
			if (!other->staticObj) other->position = other->prevPos;
			if (side == 6) { //Bottom
				//cout << "Stopping Y" << endl;
				//cout << position.y << other->position.y << endl;
				gravity = false;
				velocity.y *= 0;
			}
			/*if (side == 2 || side == 4) { //Left
				cout << "Stopping X" << endl;
				velocity.x = -velocity.x;
				if (!other->staticObj) other->velocity.x = -other->velocity.x;
			}
			if (side == 6 || side == 8) { //Top?
				cout << "Stopping Y" << endl;
				velocity.y *= 0;
				if (!other->staticObj) other->velocity.y *= 0;
			}
			if (side == 10 || side == 12) { //Back
				cout << "Stopping Z" << endl;
				velocity.z *= 0;
				if (!other->staticObj) other->velocity.z *= 0;
			}*/
			
			/*if (side == 2) { //Left
				cout << "Pushing Right" << endl;
				if (velocity.x < 0)	velocity.x = 0;
				//if (!other->staticObj) other->velocity.x = 0;
			}
			if (side == 4) { //Right
				cout << "Pushing Left" << endl;
				if (velocity.x > 0)	velocity.x = 0;
				//if (!other->staticObj) other->velocity.x = 0;
			}
			if (side == 6) { //Top?
				cout << "Pushing Up" << endl;
				if (velocity.y < 0)	velocity.y = 0;
				//if (!other->staticObj) other->velocity.y = 0;
			}
			if (side == 8) { //Bottom?
				cout << "Pushing Down" << endl;
				if (velocity.y > 0)	velocity.y = 0;
				//if (!other->staticObj) other->velocity.y = 0;
			}
			if (side == 10) { //Back
				cout << "Pushing Front" << endl;
				if (velocity.z < 0)	velocity.z = 0;
				//if (!other->staticObj) other->velocity.z = 0;
			}
			if (side == 12) { //Front
				cout << "Pushing Back" << endl;
				if (velocity.z > 0)	velocity.z = 0;
				//if (!other->staticObj) other->velocity.z = 0;
			}*/
			//if (min(bounds[0].x, other->bounds[0].x) > max(bounds[1].x, other->bounds[1].x)) /*X overlap*/ x = true;
			//else x = false;
			//if (min(bounds[0].y, other->bounds[0].y) > max(bounds[1].y, other->bounds[1].y)) /*Y overlap*/ y = true;
			//else y = false;
			//if (min(bounds[0].z, other->bounds[0].z) > max(bounds[1].z, other->bounds[1].z)) /*Z overlap*/ z = true;
			//else z = false;


			/*if (!x || !y || !z) { resolveCollisions = false; return; }
			if (x && y && z) {HandleCollision(side, other, deltaTime);}*/
		}
	}
}

void GameObject::OnOverlap(GameObject* other) { }

void GameObject::Translate(glm::vec3 newPos) {
	velocity = newPos;
}
void GameObject::Translate(float x, float y, float z) {
	velocity = glm::vec3(x, y, z);
}
void GameObject::Rotate(glm::vec3 newRotation) {
	rotation = newRotation;
}
void GameObject::Rotate(float x, float y, float z) {
	rotation = glm::vec3(x, y, z);
}
void GameObject::Scale(glm::vec3 newScale) {
	scale = newScale;
}
void GameObject::Scale(float x, float y, float z) {
	scale = glm::vec3(x, y, z);
}

glm::mat4 GameObject::ApplyRotation(glm::mat4 matrix, glm::vec3 rotation) {
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	return matrix;
}

glm::mat4 GameObject::CalculateMatrix() {
	glm::mat4 modelMat = glm::mat4(1.f);
	modelMat = glm::translate(modelMat, position);
	modelMat = ApplyRotation(modelMat, rotation);
	modelMat = glm::scale(modelMat, scale);
	return modelMat;
}