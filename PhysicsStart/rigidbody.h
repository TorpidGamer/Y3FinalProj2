#pragma once
#include <glm/glm.hpp>

class Rigidbody {
public:
	glm::vec3 position;
	glm::vec3 linearVelocity;
	glm::vec3 angle;
	glm::vec3 angularVelocity;
	glm::vec3 force;
	glm::vec3 torque;
	glm::vec3 dimensions;
	float mass;
	float momentOfIntertia;
private:
};