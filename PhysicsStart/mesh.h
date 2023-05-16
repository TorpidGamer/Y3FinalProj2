#pragma once
#ifndef MESH_H
#define MESH_H

#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image/stb_image.h>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	int m_BoneIDs[MAX_BONE_INFLUENCE];

	float m_Weights[MAX_BONE_INFLUENCE];

	Vertex() {
	}

	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoord) {
		Position = pos;
		Normal = normal;
		TexCoords = texcoord;
	}
	Vertex(glm::vec3 pos, glm::vec2 texcoord, glm::vec3 normal) {
		Position = pos;
		Normal = normal;
		TexCoords = texcoord;
	}
};

struct Texture {
	unsigned int id;
	string type;
	string path;
	bool manual;

	Texture() {

	}

	Texture(unsigned int id, string type) {
		this->id = id;
		this->type = type;
	}

	Texture(unsigned int id, string type, string path) {
		this->id = id;
		this->type = type;
		this->path = path;
	}

	Texture(unsigned int id, string type, string path, bool manual) {
		this->id = id;
		this->type = type;
		this->path = path;
		this->manual = manual;
	}
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<glm::vec3>edges;
	vector<glm::vec3>normals;
	glm::vec3 meshSpaceCenter = glm::vec3(0);
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<unsigned char> colourPlane;
	int colourPlaneHeight = 0, colourPlaneWidth = 0;
	unsigned int VAO;
	bool empty = false;
	bool render = true;
	bool useForCollision = true;
	bool wireFrame = false;

	Mesh() {
		empty = true;
		EBO = 0;
		VAO = 0;
		VBO = 0;
		//std::cout << "Made without mesh stuffs" << endl;
	}

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<unsigned char> colourPlane /*  Width * Height * 4 (for RGBA)  */, int CPH, int CPW) {
		this->vertices = vertices;
		this->indices = indices;
		this->colourPlane = colourPlane;
		colourPlaneHeight = CPH;
		colourPlaneWidth = CPW;

		setupMesh();
		OverrideTextures();
	}
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices) {
		this->vertices = vertices;
		this->indices = indices;

		setupMesh();
	}
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, bool ownTextures) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
		SetupTextures();
	}

	void CalculateNormals() {
		int triangleCount = indices.size() / 3;
		for (int i = 0; i < triangleCount; i++) {
			int triangleIndex = i * 3;
			int vertexIndexA = indices[triangleIndex];
			int vertexIndexB = indices[triangleIndex+1];
			int vertexIndexC = indices[triangleIndex+2];

			glm::vec3 triangleNormal = SurfaceNormalsFromIndices(vertexIndexA, vertexIndexB, vertexIndexC);
			vertices[vertexIndexA].Normal = triangleNormal;
			vertices[vertexIndexB].Normal = triangleNormal;
			vertices[vertexIndexC].Normal = triangleNormal;
			normals.push_back(triangleNormal);
		}
		//for (int i = 0; i < vertices.size(); i++) {
		//	normalize(vertices[i].Normal);
		//}
	}

	void CreateEdges() {
		for (int i = 0; i < vertices.size(); i+=2) {
			edges.push_back(vertices[(i + 1) % vertices.size()].Position - vertices[i].Position);
		}
	}

	glm::vec3 SurfaceNormalsFromIndices(int indexA, int indexB, int indexC) {
		glm::vec3 pointA = vertices[indexA].Position;
		glm::vec3 pointB = vertices[indexB].Position;
		glm::vec3 pointC = vertices[indexC].Position;

		glm::vec3 sideAB = pointB - pointA;
		glm::vec3 sideAC = pointC - pointA;

		return glm::normalize(glm::cross(sideAB, sideAC));

	}

	void SetupTextures() {
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glGenTextures(1, &textures[i].id);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
			// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
			unsigned char* data = stbi_load(textures[i].path.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				if (nrChannels == 4) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}
				else if (nrChannels == 3)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				}
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
			textures[i].type = "texture_diffuse";
			//cout << "Mesh Setup w/Texture" << endl;
		}
	}

	void OverrideTextures() {
		textures.push_back(Texture(0, "texture_diffuse"));
		glGenTextures(1, &textures[0].id);
		glBindTexture(GL_TEXTURE_2D, textures[0].id);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, colourPlaneWidth, colourPlaneHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, colourPlane.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Draw(Shader& shader) {	
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		unsigned int textureNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			else if (name == "texture")
				number = std::to_string(textureNr++);

			shader.setInt(("material." + name + number).c_str(), i);

			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}
private:
	unsigned int VBO, EBO;
	void setupMesh() {
		for (int i = 0; i < vertices.size(); i++) {
			meshSpaceCenter += vertices[i].Position;
		}
		meshSpaceCenter /= vertices.size();

		CalculateNormals();
		CreateEdges();

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
		//Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		//Texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		//Tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		//Bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

		glBindVertexArray(0);
		//cout << "Mesh Setup" << endl;
	}
};





#endif
