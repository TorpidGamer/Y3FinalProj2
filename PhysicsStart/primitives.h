#pragma once
#include "mesh.h"



class Primitives {
public:
	enum Shape
	{
		Cube,
		Pyramid,
		Quad
	};
	vector<Vertex> primitiveVertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Shape currentShape = Cube;

	void DefineShapes(Shape shapeToDefine) {
		switch (shapeToDefine)
		{
		case Cube:
			primitiveVertices = {
				//Position												//Texcoords					//Normals
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f),					glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, 1.f)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f),					glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f),					glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f),						glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, 1.f)),

				Vertex(glm::vec3(0.5f, 0.5f, -0.5f),					glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, -1.f)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f),					glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, -1.f)),
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),					glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, -1.f)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f),					glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, -1.f)),

				//Extra for TexCoords
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f),					glm::vec2(1.f, 0.f),		glm::vec3(0.f, 1.f, 0.f)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f),					glm::vec2(1.f, 0.f),		glm::vec3(0.f, 1.f, 0.f)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f),					glm::vec2(0.f, 0.f),		glm::vec3(0.f, 1.f, 0.f)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f),						glm::vec2(0.f, 0.f),		glm::vec3(0.f, 1.f, 0.f)),

				Vertex(glm::vec3(0.5f, 0.5f, -0.5f),					glm::vec2(0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f),					glm::vec2(0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)),
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),					glm::vec2(1.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f),					glm::vec2(1.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)),
			};

			indices = {
				0, 1, 2,
				0, 2, 3,

				7, 6, 1,
				7, 1, 0,

				4, 5, 6,
				4, 6, 7,

				3, 2, 5,
				3, 5, 4,

				0 + 8, 7 + 8, 4 + 8,
				0 + 8, 4 + 8, 3 + 8,

				1 + 8, 6 + 8, 5 + 8,
				1 + 8, 5 + 8, 2 + 8
			};

			textures = {
				Texture(0, string("texture_diffuse"), string("../Common/resources/container.jpg"), true),
				Texture(1, string("texture_diffuse"), string("../Common/resources/awesomeface.png"), true)
			};
			currentShape = Cube;
			break;

		case Pyramid:
			break;
		case Quad:
			primitiveVertices = {
				Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 0.f)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 0.f)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 0.f)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f))
			};
			indices = {
				0, 1, 2,
				3, 0, 2
			};
			textures = {
				Texture(0, string("texture_diffuse"), string("../Common/resources/window.png"), true),
			};
			currentShape = Quad;
			break;
		}
	}

	Mesh CreateMesh(Shape meshToMake) {
		DefineShapes(meshToMake);
		return Mesh(primitiveVertices, indices, textures, true);
	}
};