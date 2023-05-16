#pragma once

#include "gameobject.h"

glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis, int meshToTest);

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

CollisionDetails IsOverlapped(GameObject* objects[2]) {
	//second point - first point = edge
	
	CollisionDetails collisionDeets;

	collisionDeets.depth = 999.f;
	collisionDeets.normal = glm::vec3(0);
	collisionDeets.overlapped = true;

	glm::vec3 objectScale[2] = {objects[0]->scale, objects[1]->scale};

	glm::vec3 objectPos[2] = {objects[0]->position, objects[1]->position};
	int objectMeshToTest[2] = {0};
	float objectClosestMeshDist[2]{ glm::distance(objects[0]->model->meshes[0].meshSpaceCenter + objectPos[0], objectPos[1]),
		glm::distance(objects[1]->model->meshes[0].meshSpaceCenter + objectPos[1], objectPos[0]) };

	for (int o = 0; o < 1; o++) {
		for (int i = 1; i < objects[o]->model->meshes.size(); i++) {
			int otherObj = o == 1 ? 0 : 1;
			if (!objects[o]->model->meshes[i].useForCollision) {
				cout << "mesh not for collision" << endl;
				continue;
			}
			float dist = glm::distance(objects[o]->model->meshes[i].meshSpaceCenter + objectPos[o], objectPos[otherObj]);
			if (dist < objectClosestMeshDist[o]) {
				objectClosestMeshDist[o] = dist;
				objectMeshToTest[o] = i;
			}
		}
	}
	if (objects[0]->name == "player") objectMeshToTest[0] = 1;
	if (objects[1]->name == "player") objectMeshToTest[1] = 1;
	glm::vec3 objectMeshPosition[2] = { objects[0]->model->meshes[objectMeshToTest[0]].meshSpaceCenter, objects[1]->model->meshes[objectMeshToTest[1]].meshSpaceCenter };
	vector<glm::vec3> normalsToTest[2] = { objects[0]->model->meshes[objectMeshToTest[0]].normals, objects[1]->model->meshes[objectMeshToTest[1]].normals };
	vector<glm::vec3> objectEdges[2] = { objects[0]->model->meshes[objectMeshToTest[0]].edges, objects[1]->model->meshes[objectMeshToTest[1]].edges };
	//cout << "Testing obj1: " << obj1->name << ", mesh no. " << meshToTest1 << endl;
	//cout << "Testing obj2: " << obj2->name << ", mesh no. " << meshToTest2 << endl;

		//create normals and edges to test along
		/*for (int o = 0; o < 2; o++) {
			for (unsigned int i = 0; i < objects[o]->model->meshes[objectMeshToTest[o]].vertices.size(); i += 3) {
				unsigned int index = objects[o]->model->meshes[objectMeshToTest[o]].indices[i];
				glm::vec3 p1 = objects[o]->model->meshes[objectMeshToTest[o]].vertices[index].Position;
				glm::vec3 p2 = objects[o]->model->meshes[objectMeshToTest[o]].vertices[index + 1].Position;
				glm::vec3 p3 = objects[o]->model->meshes[objectMeshToTest[o]].vertices[index + 2].Position;
				//Transform these to worldspace
				//p3 += objects[o]->model->meshes[objectMeshToTest[o]].meshSpaceCenter;
				
				glm::vec3 u = p2 - p1;
				glm::vec3 v = p3 - p1;

				glm::vec3 normal = glm::vec3((u.y * v.z) - (u.z * v.y),
					(u.z * v.x) - (u.x * v.z),
					(u.x * v.y) - (u.y * v.x));

				normalsToTest[o].push_back(normal);
				objectEdges[o].push_back(u);
				objectEdges[o].push_back(v);
			}
		}*/
			
		for (int o = 0; o < 2; o++) {
			int otherObj = o == 1 ? 0 : 1;
			//Test normals
			for (int i = 0; i < normalsToTest[o].size(); i++) {
				glm::vec2 project1, project2;
				glm::vec3 axis = (normalsToTest[o][i]); //* -objects[o]->scale));// + objects[o]->position);
				project1 = ProjectVertices(objects[o], axis, objectMeshToTest[o]);
				project2 = ProjectVertices(objects[otherObj], axis, objectMeshToTest[otherObj]);

				collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "normals " + to_string(o));
			}

			//Test edges
			for (int i = 0; i < objectEdges[o].size(); i++) {
				if (objectEdges[otherObj].size() == 0) {
					cout << "No edges on Obj" << otherObj+1 << endl;
					break;
				}
				glm::vec3 axis1 = (objectEdges[o][i]); //* -objects[o]->scale); //+ objects[o]->position;
				glm::vec3 axis2 = (objectEdges[otherObj][i % objectEdges[otherObj].size()]); //* -objects[otherObj]->scale);// + objects[otherObj]->position;
				glm::vec3 axis = glm::cross(axis1, axis2);

				glm::vec2 project1, project2;
				project1 = ProjectVertices(objects[o], axis, objectMeshToTest[o]);
				project2 = ProjectVertices(objects[otherObj], axis, objectMeshToTest[otherObj]);

				collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "edges " + to_string(o));
			}
		}

		//If we reached this point the objects overlap so return details

		float minDepth = 0.0001;
		collisionDeets.depth /= collisionDeets.normal.length();

		collisionDeets.depth;
		if (collisionDeets.depth < minDepth) collisionDeets.depth = minDepth;
		if (collisionDeets.normal.x == 0 && collisionDeets.normal.y == 0 && collisionDeets.normal.z == 0); //cout << "Normal not Valid" << endl;
		else {
			collisionDeets.normal = glm::normalize(collisionDeets.normal);
			collisionDeets.normal.x = 0;
			collisionDeets.normal.z = 0;
		}
		if (isnan(collisionDeets.normal.x)) cout << "Normalized Normal Nan" << endl;
		//cout << "Overlap" << endl;
		//cout << collisionDeets.depth << endl;
		glm::vec3 intendedDir = objects[1]->position - objects[0]->position;
		if (glm::dot(intendedDir, collisionDeets.normal) <= 0.f) {
			collisionDeets.normal = -collisionDeets.normal;
		}
		//cout << collisionDeets.normal.x << ", " << collisionDeets.normal.y << ", " << collisionDeets.normal.z << endl;
		collisionDeets.overlapped = true;
		return collisionDeets;
}

//glm::vec3 GetAxis(glm::vec3 pointA, glm::vec3 pointB) {}

//x = min, y = max
glm::vec2 ProjectVertices(GameObject* verticesToProject, glm::vec3 axis, int meshToTest) {
	float min = 999;
	float max = -999;
	for (int i = 0; i < verticesToProject->model->meshes[meshToTest].vertices.size(); i++) {
		glm::vec3 currentVert = (verticesToProject->model->meshes[meshToTest].vertices[i].Position * verticesToProject->scale) + verticesToProject->position;
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