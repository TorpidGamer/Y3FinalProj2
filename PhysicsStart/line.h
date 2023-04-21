#pragma once

#include "mesh.h"

class LineRenderer {
public:
	vector<Vertex> vertices;
	unsigned int VAO;

	LineRenderer(glm::vec3 pointA, glm::vec3 pointB, float width) {
		unsigned int VBO, EBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//a Structs memory layout is sequential which means giving a pointer to a struct will lay all the bytes out meaning we dont need to calculate how many bytes we need or the layout
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			//vertex attribute pointers
			//Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	}
};