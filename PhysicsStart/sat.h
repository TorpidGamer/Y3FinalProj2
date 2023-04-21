#pragma once


#include "gameobject.h"

glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis);
struct CollisionDetails {
	bool overlapped;
	glm::vec3 normal = glm::vec3(0);
	float depth = -1;
};

CollisionDetails IsOverlapped(GameObject* obj1, GameObject* obj2) {
	//second point - first point = edge
	
	CollisionDetails collisionDeets;

	collisionDeets.depth = 999.f;
	collisionDeets.normal = glm::vec3(0);
	collisionDeets.overlapped = true;

	//for (int m = 0; m < obj1->model->meshes.size(); m++) {
		for (int i = 0; i < obj1->model->meshes[0].vertices.size(); i += 2) {
			glm::vec3 pointA, pointB;
			pointB = (obj1->model->meshes[0].vertices[(i + 1) % obj1->model->meshes[0].vertices.size()].Position * obj1->scale) + obj1->position;
			pointA = (obj1->model->meshes[0].vertices[i].Position * obj1->scale) + obj1->position;
			glm::vec3 edge = pointB - pointA;
			edge.x = edge.y * 0 - edge.z * 1;
			edge.y = edge.z * 0 - edge.x * 0;
			edge.z = edge.x * 1 - edge.y * 0;
			cout << edge.x << ", " << edge.y << ", " << edge.z << endl;
			glm::vec3 axis = glm::normalize(edge);
			
			//X = min, Y == max
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			if (project1.x >= project2.y || project2.x >= project1.y) {
				collisionDeets.overlapped = false;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);
			
			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = axis;
			}
		} 
	//}
	//for (int m = 0; m < obj1->model->meshes.size(); m++) {
		for (int i = 0; i < obj2->model->meshes[0].vertices.size(); i += 2) {
			glm::vec3 pointA, pointB;
			pointB = (obj2->model->meshes[0].vertices[(i + 1) % obj2->model->meshes[0].vertices.size()].Position * obj2->scale) + obj2->position;
			pointA = (obj2->model->meshes[0].vertices[i].Position * obj2->scale) + obj2->position;
			glm::vec3 edge = pointB - pointA;
			glm::vec3 axis = glm::normalize(edge);
			//cout << axis.x << ", " << axis.y << ", " << axis.z << endl;
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			if (project1.x >= project2.y || project2.x >= project1.y) {
				collisionDeets.overlapped = false;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);
			
			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = axis;
			}
		}
	//}

		collisionDeets.depth /= collisionDeets.normal.length();
		collisionDeets.normal = glm::normalize(collisionDeets.normal);

		glm::vec3 intendedDir = obj2->position - obj1->position;
		if (glm::dot(intendedDir, collisionDeets.normal) < 0) collisionDeets.normal = -collisionDeets .normal;

		cout << "No Gap between: " << obj1->name << ", " << obj2->name << endl;
		collisionDeets.overlapped = true;
		return collisionDeets;
}

glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis) {
	float min;
	float max;
	min = max = glm::dot((verticesToProject->model->meshes[0].vertices[0].Position * verticesToProject->scale) + verticesToProject->position, axis);
	for (int i = 0; i < verticesToProject->model->meshes[0].vertices.size(); i++) {
		float projection = glm::dot((verticesToProject->model->meshes[0].vertices[0].Position * verticesToProject->scale) + verticesToProject->position, axis);

		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
	return glm::vec2(min, max);
}