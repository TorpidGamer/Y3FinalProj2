#pragma once


#include "gameobject.h"

glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis);
struct CollisionDetails {
	bool overlapped;
	glm::vec3 normal = glm::vec3(0);
	float depth = -1;
};

bool PointInCircle(glm::vec2 point, glm::vec2 circle, float circleRad) {
	float distX = point.x - circle.x;
	float distY = point.y - circle.y;
	if (hypot(distX, distY) >= circleRad) return true;
	return false;
}

CollisionDetails IsOverlapped(GameObject* obj1, GameObject* obj2) {
	//second point - first point = edge
	
	CollisionDetails collisionDeets;

	collisionDeets.depth = 999.f;
	collisionDeets.normal = glm::vec3(0);
	collisionDeets.overlapped = true;

	vector<glm::vec3> normalsToTest1, normalsToTest2;
	vector<glm::vec3> obj1Edges, obj2Edges;

	glm::vec3 obj1NextPos = obj1->position, obj2NextPos = obj2->position;
	glm::vec3 obj1Scale = obj1->scale, obj2Scale = obj2->scale;

	glm::vec3 obj1Pos = obj1->position, obj2Pos = obj2->position;
	// create circle that can then compare to point we're testing to see if within range
	glm::vec2 circlePos1 = glm::vec2(obj1->position.x, obj1->position.z), circlePos2 = glm::vec2(obj2->position.x, obj2->position.z);
	float circleRadius = 5;
	// use hypot(point

		//create normals and edges
		for (unsigned int i = 0; i < obj1->model->meshes[0].vertices.size(); i += 3) {
			unsigned int index = obj1->model->meshes[0].indices[i];
			glm::vec3 testVec3 = obj1->model->meshes[0].vertices[index].Position + obj1Pos;
			glm::vec2 testVec2(testVec3.x, testVec3.z);
			if (PointInCircle(testVec2, circlePos2, circleRadius)) {
				//cout << "Outside of radius, discarding" << endl;
				continue;
			}
			glm::vec3 p1 = obj1->model->meshes[0].vertices[index].Position;
			glm::vec3 p2 = obj1->model->meshes[0].vertices[index+1].Position;
			glm::vec3 p3 = obj1->model->meshes[0].vertices[index+2].Position;

			glm::vec3 u = p2 - p1;
			glm::vec3 v = p3 - p1;

			glm::vec3 normal = glm::vec3((u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x));

			normalsToTest1.push_back((normal * obj1Scale) + obj1NextPos);
			obj1Edges.push_back((u * obj1Scale) + obj1NextPos);
			obj1Edges.push_back((v * obj1Scale) + obj1NextPos);
		}
		for (unsigned int i = 0; i < obj2->model->meshes[0].vertices.size() - 3; i += 3) {
			unsigned int index = obj2->model->meshes[0].indices[i];
			glm::vec3 testVec3 = obj2->model->meshes[0].vertices[index].Position + obj2Pos;
			glm::vec2 testVec2(testVec3.x, testVec3.z);
			if (PointInCircle(testVec2, circlePos1, circleRadius)) {
				//cout << "Outside of radius, discarding" << endl;
				continue;
			}
			glm::vec3 p1 = obj2->model->meshes[0].vertices[index].Position;
			glm::vec3 p2 = obj2->model->meshes[0].vertices[(index + 1) % obj2->model->meshes[0].indices.size()].Position;
			glm::vec3 p3 = obj2->model->meshes[0].vertices[(index + 2) % obj2->model->meshes[0].indices.size()].Position;
			if (isnan(p1.x) || isnan(p1.y) || isnan(p1.z)) {
				cout << "index: " << index << " is nan" << endl;
				continue;
			}
			glm::vec3 u = p2 - p1;
			glm::vec3 v = p3 - p1;

			glm::vec3 normal = glm::vec3((u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x));
			normalsToTest2.push_back((normal * obj2Scale) + obj2NextPos);
			obj2Edges.push_back((u * obj2Scale) + obj2NextPos);
			obj2Edges.push_back((v * obj2Scale) + obj2NextPos);
		}

		//Test normals first
		for (int i = 0; i < normalsToTest1.size(); i++) {
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, normalsToTest1[i]);
			project2 = ProjectVertices(obj2, normalsToTest1[i]);

			float longSpan = max(project1.y, project2.y) - min(project1.x, project2.x);
			float sumSpan = project1.y - project1.x + project2.y - project2.x;

			if (longSpan > sumSpan) {
				collisionDeets.overlapped = false;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);
			
			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = normalsToTest1[i];
			}
		}

		for (int i = 0; i < normalsToTest2.size(); i++) {
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, normalsToTest2[i]);
			project2 = ProjectVertices(obj2, normalsToTest2[i]);

			float longSpan = max(project1.y, project2.y) - min(project1.x, project2.x);
			float sumSpan = project1.y - project1.x + project2.y - project2.x;

			if (longSpan > sumSpan) {
				collisionDeets.overlapped = false;
				cout << "Normal Test 2 Pass" << endl;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);

			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = normalsToTest2[i];
			}
		}

		for (int i = 0; i < obj1Edges.size(); i++) {
			if (obj2Edges.size() == 0) {
				cout << "No edges on Obj2" << endl;
				return collisionDeets;
			}
			glm::vec3 axis = glm::cross(obj1Edges[i], obj2Edges[i % obj2Edges.size()]);

			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			float longSpan = max(project1.y, project2.y) - min(project1.x, project2.x);
			float sumSpan = project1.y - project1.x + project2.y - project2.x;

			if (longSpan > sumSpan) {
				collisionDeets.overlapped = false;
				cout << "Edge Test 1 Pass" << endl;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);

			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = axis;
			}
		}

		for (int i = 0; i < obj2Edges.size(); i++) {
			if (obj1Edges.size() == 0) {
				cout << "No edges on Obj1" << endl;
				return collisionDeets;
			}
			glm::vec3 axis = glm::cross(obj2Edges[i], obj1Edges[i % obj1Edges.size()]);

			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			float longSpan = max(project1.y, project2.y) - min(project1.x, project2.x);
			float sumSpan = project1.y - project1.x + project2.y - project2.x;

			if (longSpan > sumSpan) {
				collisionDeets.overlapped = false;
				cout << "Edge Test 2 Pass" << endl;
				return collisionDeets;
			}

			float axisDepth = min(project2.y - project1.x, project1.y - project2.x);

			if (axisDepth < collisionDeets.depth) {
				collisionDeets.depth = axisDepth;
				collisionDeets.normal = axis;
			}
		}

		collisionDeets.depth /= collisionDeets.normal.length();
		collisionDeets.normal = glm::normalize(collisionDeets.normal);
		cout << collisionDeets.depth << endl;
		cout << collisionDeets.normal.x << ", " << collisionDeets.normal.y << ", " << collisionDeets.normal.z << endl;
		if (isnan(collisionDeets.normal.x) || isnan(collisionDeets.normal.y) || isnan(collisionDeets.normal.z)) {
			cout << "error isNan on: " << obj1->name << " and " << obj2->name << endl;
			collisionDeets.normal = glm::vec3(0);
		}
		glm::vec3 intendedDir = obj2->position - obj1->position;
		if (glm::dot(intendedDir, collisionDeets.normal) < 0.f) {
			collisionDeets.normal = -collisionDeets.normal;
		}
		//cout << "No Gap between: " << obj1->name << ", " << obj2->name << endl;
		collisionDeets.overlapped = true;
		return collisionDeets;
}

//glm::vec3 GetAxis(glm::vec3 pointA, glm::vec3 pointB) {}

//x = min, y = max
glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis) {
	float min = 999;
	float max = -999;
	for (int i = 0; i < verticesToProject->model->meshes[0].vertices.size(); i++) {
		glm::vec3 currentVert = (verticesToProject->model->meshes[0].vertices[i].Position * verticesToProject->scale) + verticesToProject->position + verticesToProject->velocity;
		if (isnan(currentVert.x) || isnan(currentVert.y) || isnan(currentVert.z)) {
			cout << "error isNan on: " << verticesToProject->name << endl;
			currentVert = glm::vec3(0);
		}
		float projection = glm::dot(currentVert, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
	return glm::vec2(min, max);
}