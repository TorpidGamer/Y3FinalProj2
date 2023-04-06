#pragma once
#include <iostream>
#include <cstdlib>
#include <glm/glm.hpp>
#include "primitives.h"
#include "gameobjectchildren.h"
#include "camera.h"

int numOfScenes = 0;
class Scene;
class MeshData;
map <string, Scene*> scenes;
bool sceneLoaded = false;
class Scene {
public:
    int id;
    string name;
    map<string, GameObject*> sceneGOs;
    map<string, Model*> models;
    map<string, Mesh*> meshes;
    glm::vec3 playerStartPos = glm::vec3(0);
    Scene(string name) {
        this->name = name;
        id = numOfScenes++;
        scenes[name] = this;
    }
    ~Scene() {
        Unload();
    }
    virtual Scene* InitScene() { cout << "Attempted to call initilization on base class, mistake?" << endl; return nullptr; };

    void Unload() {
        if (!meshes.empty()) {
            for (auto it = meshes.begin(); it != meshes.end(); it++) {
                delete it->second;
            }
        }
        if (!models.empty()) {
            for (map<string, Model*>::iterator it = models.begin(); it != models.end(); it++) {
                delete it->second;
            }
        }
        if (!sceneGOs.empty()) {
            for (map<string, GameObject*>::iterator it = sceneGOs.begin(); it != sceneGOs.end(); it++) {
                //cout << it->first << endl;
                if (it->first == "player") {
                    continue;
                }
                delete it->second;
            }
            sceneGOs.clear();
        }
        models.clear();
        meshes.clear();
    }
};

Scene* LoadScene(Scene* currentScene, string sceneToLoad, Camera* cam) {
    //Get Name to use, trigger function specific to name that loads in all objects
    sceneLoaded = false;
    currentScene->Unload();
    map <string, Scene*>::iterator it = scenes.find(sceneToLoad);
    if (it == scenes.end()) {
        cout << "Not a valid Scene" << endl;
        return LoadScene(currentScene, "Test", cam);
    }
    scenes.at(sceneToLoad)->sceneGOs["player"] = cam->playerChar;
    cam->playerChar->position = scenes.at(sceneToLoad)->playerStartPos;
    cam->playerChar->gravity = true;
    Scene* temp = scenes.at(sceneToLoad)->InitScene();
    sceneLoaded = true;
    for (map<string, GameObject*>::iterator it = currentScene->sceneGOs.begin(); it != currentScene->sceneGOs.end(); it++) {
        cout << it->first << endl;
    }
    return temp;
};

class TestScene : public Scene {
public:
    TestScene(string name) : Scene(name) {
    }
    virtual Scene* InitScene() override {
        Scene* testScene = scenes.at("Test");
        Primitives shape;
        Mesh* cubeMesh = new Mesh(shape.CreateMesh(shape.Cube));
        Mesh* quadMesh = new Mesh(shape.CreateMesh(shape.Quad));
        Model* model = new Model("models/maxwell/maxwell.fbx");
        Model* car = new Model("models/car/Porsche_911_GT2.obj");
        meshes["cubeMesh"] = cubeMesh;
        meshes["quadMesh"] = quadMesh;
        models["maxwell"] = model;
        models["car"] = car;       
        testScene->playerStartPos = glm::vec3(1.f, 2.f, 1.f);
        Goal* goal = new Goal(meshes["cubeMesh"], glm::vec3(0, 0, 10), glm::vec3(0), glm::vec3(1), glm::vec3(0), "Level1");
        GameObject* floor = new GameObject(meshes["cubeMesh"], glm::vec3(0.f, -10.f, 0.f), glm::vec3(0.f), glm::vec3(30.f, 2.f, 30.f), glm::vec3(0), "floor");
        GameObject* maxwell = new GameObject(models["maxwell"], glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(.33f), glm::vec3(0), "maxwell");
        GameObject* carGO = new GameObject(models["car"], glm::vec3(10.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f), glm::vec3(0), "car");
        GameObject* window1 = new GameObject(meshes["quadMesh"], glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window1");
        GameObject* window2 = new GameObject(meshes["quadMesh"], glm::vec3(2.f, 0.f, -4.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window2");
        GameObject* window3 = new GameObject(meshes["quadMesh"], glm::vec3(1.f, 0.f, -6.f), glm::vec3(0.f, 90.f, 0.f), glm::vec3(1.f), glm::vec3(0), "window3");
        GameObject* window4 = new GameObject(meshes["quadMesh"], glm::vec3(0.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window4");

        window1->transparent = true;
        window2->transparent = true;
        window3->transparent = true;
        window4->transparent = true;
        maxwell->transparent = true;

        floor->staticObj = true;

        testScene->sceneGOs["window1"] = window1;
        testScene->sceneGOs["window2"] = window2;
        testScene->sceneGOs["window3"] = window3;
        testScene->sceneGOs["window4"] = window4;

        testScene->sceneGOs["goal"] = goal;
        testScene->sceneGOs["maxwell"] = maxwell;
        testScene->sceneGOs["car"] = carGO;
        testScene->sceneGOs["floor"] = floor;

        return testScene;
    }
};
//Code heavily based on Sebastion Lague's Procedural Terrain Generation series https://www.youtube.com/watch?v=wbpMiKiSKm8&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3
class MeshData {
public:
    //20 in this case is the mapwidth and mapheight, have to update this manually for now, figure out better way after testing
    vector<Vertex> vertices;
    vector<unsigned int> triangles;
    vector<unsigned char> textureColourPlane;
    int CPH = 0, CPW = 0;

    int triangleIndex = 0;

    MeshData(int mapWidth, int mapHeight) {
        vertices = vector<Vertex>(mapWidth * mapHeight);
        triangles = vector<unsigned int>((mapWidth - 1) * (mapHeight - 1) * 6);
        textureColourPlane = vector<unsigned char>(mapWidth*mapHeight*4);
        CPH = mapHeight;
        CPW = mapWidth;
        for (int iy = 0; iy < mapHeight; ++iy)
        {
            for (int ix = 0; ix < mapWidth; ++ix)
            {
                int   index = (iy * mapWidth + ix) * 4;
                float gradX = (float)ix / mapWidth;
                float gradY = (float)iy / mapHeight;
                textureColourPlane[index + 0] = (unsigned char)(255.0 * (1.0 - gradX));
                textureColourPlane[index + 1] = (unsigned char)(255.0 * (1.0 - gradY));
                textureColourPlane[index + 2] = (unsigned char)(255.0 * gradX * gradY);
                textureColourPlane[index + 3] = 255;
            }
        }
    }

    void AddTriangle(int a, int b, int c) {
        triangles[triangleIndex] = a;
        triangles[triangleIndex + 1] = b;
        triangles[triangleIndex + 2] = c;
        triangleIndex += 3;
    }

    Mesh GenerateMesh() {
        return Mesh(vertices, triangles, textureColourPlane, CPH, CPW);
    }
};

class ProceduralScene : public Scene {
public:
    static const int mapWidth = 200;
    static const int mapHeight = 200;
    float heightMap[mapWidth][mapHeight];
    int triangles[(mapWidth - 1)*(mapHeight - 1) * 6];
    int vertexIndex = 0;
    float moveLeftX = (mapWidth - 1) / -2.f;
    float moveLeftZ = (mapHeight - 1) / 2.f;
    MeshData meshData = MeshData(mapWidth, mapHeight);
    ProceduralScene(string name) : Scene(name) {
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++){
                heightMap[x][y] = /*RandomNoise*/ static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                meshData.vertices[vertexIndex].Position = glm::vec3(moveLeftX + x, heightMap[x][y], moveLeftZ - y);
                meshData.vertices[vertexIndex].TexCoords = glm::vec2(x / (float)mapWidth, y / (float)mapHeight);

                int meshDataIndex = (y * mapWidth + x) * 4;
                meshData.textureColourPlane[meshDataIndex] = (unsigned char)(0 * (1 - heightMap[x][y]) + (255 * heightMap[x][y]));
                meshData.textureColourPlane[meshDataIndex + 1] = (unsigned char)(0 * (1 - heightMap[x][y]) + (255 * heightMap[x][y]));
                meshData.textureColourPlane[meshDataIndex + 2] = (unsigned char)(0 * (1 - heightMap[x][y]) + (255 * heightMap[x][y]));
                meshData.textureColourPlane[meshDataIndex + 3] = (unsigned char)255;

                if (x < mapWidth - 1 && y < mapHeight - 1) {
                    meshData.AddTriangle(vertexIndex, vertexIndex + mapWidth + 1, vertexIndex + mapWidth);
                    meshData.AddTriangle(vertexIndex + mapWidth + 1, vertexIndex, vertexIndex + 1);
                }
                vertexIndex++;
            }
        }
    }
    virtual Scene* InitScene() override {
        Scene* scene = scenes.at("Procedural");
        Mesh* generatedMesh = new Mesh(meshData.GenerateMesh());
        meshes["generatedMesh"] = generatedMesh;
        GameObject* floor = new GameObject(meshes["generatedMesh"], glm::vec3(0.f, -5.f, 0.f), glm::vec3(0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0), "floor");
        floor->staticObj = true;

        scene->sceneGOs["floor"] = floor;

        return scene;
    }

};

class PerlinNoiseGenerator {

};

class MeshGenerator {

};