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
	if (sqrtf(distX * distX + distY * distY) >= circleRad) return true;
	return false;
}

CollisionDetails CalculateDetails(glm::vec3 axis, glm::vec2 projection1, glm::vec2 projection2, CollisionDetails* currentDetails, string name = "null") {
	float longSpan = max(projection1.y, projection2.y) - min(projection1.x, projection2.x);
	float sumSpan = projection1.y - projection1.x + projection2.y - projection2.x;

	if (longSpan > sumSpan) {
		currentDetails->overlapped = false;
		//cout << name << " Pass" << endl;
		return *currentDetails;
	}

	float axisDepth = min(projection2.y - projection1.x, projection1.y - projection2.x);

	if (axisDepth < currentDetails->depth) {
		float previousDepth = currentDetails->depth;
		glm::vec3 previousAxis = currentDetails->normal;
		currentDetails->depth = axisDepth;
		currentDetails->normal = axis;
		if (currentDetails->normal.x == 0 && currentDetails->normal.y == 0 && currentDetails->normal.z == 0) {
			//cout << name << " was 0, 0, 0" << endl;
			currentDetails->depth = previousDepth;
			currentDetails->normal = previousAxis;
		}
		if (isnan(currentDetails->normal.x)) cout << name << " is nan" << endl;
	}
	return *currentDetails;
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
	int meshToTest1 = 0, meshToTest2 = 0;
	float obj1ClosestMesh = glm::distance(obj1->model->meshes[0].meshSpaceCenter + obj1Pos, obj2->position), obj2ClosestMesh = glm::distance(obj2->model->meshes[0].meshSpaceCenter + obj2Pos, obj1->position);
	for (int i = 1; i < obj1->model->meshes.size(); i++) {
		float dist = glm::distance(obj1->model->meshes[i].meshSpaceCenter + obj1Pos, obj2->position);
		if (dist < obj1ClosestMesh) {
			obj1ClosestMesh = dist;
			meshToTest1 = i;
		}
	}
	for (int i = 1; i < obj2->model->meshes.size(); i++) {
		float dist = glm::distance(obj2->model->meshes[i].meshSpaceCenter + obj2Pos, obj1->position);
		if (dist < obj2ClosestMesh) {
			obj2ClosestMesh = dist;
			meshToTest2 = i;
		}
	}
	glm::vec3 obj1MeshPos = obj1->model->meshes[meshToTest1].meshSpaceCenter;
	glm::vec3 obj2MeshPos = obj2->model->meshes[meshToTest2].meshSpaceCenter;
	//cout << "Testing obj1: " << obj1->name << ", mesh no. " << meshToTest1 << endl;
	//cout << "Testing obj2: " << obj2->name << ", mesh no. " << meshToTest2 << endl;

		//create normals and edges

		for (unsigned int i = 0; i < obj1->model->meshes[meshToTest1].vertices.size(); i += 3) {
			unsigned int index = obj1->model->meshes[meshToTest1].indices[i];
			glm::vec3 p1 = obj1->model->meshes[meshToTest1].vertices[index].Position;
			glm::vec3 p2 = obj1->model->meshes[meshToTest1].vertices[index+1].Position;
			glm::vec3 p3 = obj1->model->meshes[meshToTest1].vertices[index+2].Position;
			//Transform these to worldspace
			cout << "Obj1p1Start: " << p1.x << ", " << p1.y << ", " << p1.z << endl;
			p1 *= obj1Scale;
			cout << "Obj1p1Scale: " << p1.x << ", " << p1.y << ", " << p1.z << endl;
			p1 += obj1MeshPos + obj1Pos;
			cout << "Obj1p1Pos: " << p1.x << ", " << p1.y << ", " << p1.z << endl << endl;

			p2 *= obj1Scale;
			p2 += obj1MeshPos + obj1Pos;

			p3 *= obj1Scale;
			p3 += obj1MeshPos + obj1Pos;


			glm::vec3 u = p2 - p1;
			glm::vec3 v = p3 - p1;

			glm::vec3 normal = glm::vec3((u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x));

			normalsToTest1.push_back(normal);
			obj1Edges.push_back(u);
			obj1Edges.push_back(v);
		}
		for (unsigned int i = 0; i < obj2->model->meshes[meshToTest2].vertices.size() - 3; i += 3) {
			unsigned int index = obj2->model->meshes[meshToTest2].indices[i];
			glm::vec3 p1 = obj2->model->meshes[meshToTest2].vertices[index].Position;
			glm::vec3 p2 = obj2->model->meshes[meshToTest2].vertices[(index + 1)].Position;
			glm::vec3 p3 = obj2->model->meshes[meshToTest2].vertices[(index + 2)].Position;
			//Transform these to worldspace
			cout << "Obj2p1Start: " << p1.x << ", " << p1.y << ", " << p1.z << endl;
			p1 *= obj2Scale;
			cout << "Obj2p1Scale: " << p1.x << ", " << p1.y << ", " << p1.z << endl;
			p1 += obj2MeshPos + obj2Pos;
			cout << "Obj2p1Pos: " << p1.x << ", " << p1.y << ", " << p1.z << endl << endl;

			p2 *= obj2Scale;
			p2 += obj2MeshPos + obj2Pos;

			p3 *= obj2Scale;
			p3 += obj2MeshPos + obj2Pos;

			glm::vec3 u = p2 - p1;
			glm::vec3 v = p3 - p1;

			glm::vec3 normal = glm::vec3((u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x));
			normalsToTest2.push_back(normal);
			obj2Edges.push_back(u);
			obj2Edges.push_back(v);
		}

		//Test normals
		for (int i = 0; i < normalsToTest1.size(); i++) {
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, normalsToTest1[i]);
			project2 = ProjectVertices(obj2, normalsToTest1[i]);

			collisionDeets = CalculateDetails(normalsToTest1[i], project1, project2, &collisionDeets, "normals1");
		}

		for (int i = 0; i < normalsToTest2.size(); i++) {
			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, normalsToTest2[i]);
			project2 = ProjectVertices(obj2, normalsToTest2[i]);

			collisionDeets = CalculateDetails(normalsToTest2[i], project1, project2, &collisionDeets, "normals2");
		}

		//Test edges
		for (int i = 0; i < obj1Edges.size(); i++) {
			if (obj2Edges.size() == 0) {
				cout << "No edges on Obj2" << endl;
				break;
			}
			glm::vec3 axis = glm::cross(obj1Edges[i], obj2Edges[i % obj2Edges.size()]);

			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "edges1");
		}

		for (int i = 0; i < obj2Edges.size(); i++) {
			if (obj1Edges.size() == 0) {
				cout << "No edges on Obj1" << endl;
				break;
			}
			glm::vec3 axis = glm::cross(obj2Edges[i], obj1Edges[i % obj1Edges.size()]);

			glm::vec2 project1, project2;
			project1 = ProjectVertices(obj1, axis);
			project2 = ProjectVertices(obj2, axis);

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "edges2");
		}

		//If we reached this point the objects overlap so return details

		float minDepth = 0.0001;
		collisionDeets.depth /= collisionDeets.normal.length();
		collisionDeets.depth /= 2;
		if (collisionDeets.depth < minDepth) collisionDeets.depth = minDepth;
		if (collisionDeets.normal.x == 0 && collisionDeets.normal.y == 0 && collisionDeets.normal.z == 0); //cout << "Normal not Valid" << endl;
		else collisionDeets.normal = glm::normalize(collisionDeets.normal);
		if (isnan(collisionDeets.normal.x)) cout << "Normalized Normal Nan" << endl;
		//cout << "Overlap" << endl;
		//cout << collisionDeets.depth << endl;
		glm::vec3 intendedDir = obj2->position - obj1->position;
		if (glm::dot(intendedDir, collisionDeets.normal) <= 0.f) {
			collisionDeets.normal = -collisionDeets.normal;
		}
		//cout << collisionDeets.normal.x << ", " << collisionDeets.normal.y << ", " << collisionDeets.normal.z << endl;
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