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
float PerlinNoise(float x, float y);
float Lerp(float t, float var1, float var2);
float FractalBrownianMotion(float x, float y, int numOctaves);
Primitives playerPrim;
Model* playerMesh;
GameObject* player;

class Scene {
public:
    int id;
    string name;
    map<string, GameObject*> sceneGOs;
    map<string, Model*> models;
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
    virtual void PassDataToScene(int shift = 0) { cout << "No Function Implemented for this scene" << endl; };

    void Unload() {
        if (!models.empty()) {
            for (map<string, Model*>::iterator it = models.begin(); it != models.end(); it++) {
                delete it->second;
            }
        }
        if (!sceneGOs.empty()) {
            for (map<string, GameObject*>::iterator it = sceneGOs.begin(); it != sceneGOs.end(); it++) {
                //cout << it->first << endl;
                delete it->second;
            }
            sceneGOs.clear();
        }
        models.clear();
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
    playerMesh = new Model(playerPrim.CreateModel(Primitives::Cube));
    player = new GameObject(playerMesh, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "player");
    cam->playerChar = player;
    player->position = scenes.at(sceneToLoad)->playerStartPos;
    Scene* temp = scenes.at(sceneToLoad)->InitScene();
    temp->sceneGOs["player"] = player;
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
        Model* cubeMesh = new Model(shape.CreateModel(shape.Cube));
        Model* quadMesh = new Model(shape.CreateModel(shape.Quad));
        Model* model = new Model("models/maxwell/maxwell.fbx");
        Model* car = new Model("models/car/Porsche_911_GT2.obj");
        models["cubeMesh"] = cubeMesh;
        models["quadMesh"] = quadMesh;
        models["maxwell"] = model;
        models["car"] = car;       
        testScene->playerStartPos = glm::vec3(1.f, 2.f, 1.f);
        GameObject* goal = new GameObject(models["cubeMesh"], glm::vec3(0, 0.f, 10), glm::vec3(0), glm::vec3(1), glm::vec3(0), "Test");
        GameObject* floor = new GameObject(models["cubeMesh"], glm::vec3(0.f, -10.f, 0.f), glm::vec3(0.f), glm::vec3(30.f, 2.f, 30.f), glm::vec3(0), "floor");
        //GameObject* maxwell = new GameObject(models["maxwell"], glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(.33f), glm::vec3(0), "maxwell");
        GameObject* carGO = new GameObject(models["car"], glm::vec3(10.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f), glm::vec3(0), "car");
        /*GameObject* window1 = new GameObject(models["quadMesh"], glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window1");
        GameObject* window2 = new GameObject(models["quadMesh"], glm::vec3(2.f, 0.f, -4.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window2");
        GameObject* window3 = new GameObject(models["quadMesh"], glm::vec3(1.f, 0.f, -6.f), glm::vec3(0.f, 90.f, 0.f), glm::vec3(1.f), glm::vec3(0), "window3");
        GameObject* window4 = new GameObject(models["quadMesh"], glm::vec3(0.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "window4");

        window1->transparent = true;
        window2->transparent = true;
        window3->transparent = true;
        window4->transparent = true;*/
        //maxwell->transparent = true;

        floor->staticObj = true;
        //maxwell->staticObj = true;

        /*testScene->sceneGOs["window1"] = window1;
        testScene->sceneGOs["window2"] = window2;
        testScene->sceneGOs["window3"] = window3;
        testScene->sceneGOs["window4"] = window4;*/

        testScene->sceneGOs["goal"] = goal;
        //testScene->sceneGOs["maxwell"] = maxwell;
        testScene->sceneGOs["car"] = carGO;
        testScene->sceneGOs["floor"] = floor;

        return testScene;
    }
};
//Code based on Sebastion Lague's Procedural Terrain Generation series https://www.youtube.com/watch?v=wbpMiKiSKm8&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3
class MeshData {
public:
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
    }

    void AddTriangle(int a, int b, int c) {
        if (triangleIndex >= triangles.size()) {
            cout << "Index greater than vector" << endl;
            //triangleIndex = 0;
        }
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
    static const int chunkWidth = 10;
    static const int chunkHeight = 10;
    static const int numberOfChunks = 10;
    static const int mapWidth = chunkWidth * numberOfChunks;
    static const int mapHeight = chunkHeight * numberOfChunks;
    float frequency = 20;
    int octaves = 8;
    float scale = 0.01f;
    float heightMap[chunkWidth][chunkHeight];
    float textureMap[chunkWidth][chunkHeight];
    int vertexIndex = 0;
    float moveLeftX = (mapWidth - 1) / -2.f;
    float moveLeftZ = (mapHeight - 1) / 2.f;

    float viewDist = 300;
    float offsetX=0, offsetY=0;
    MeshData meshData = MeshData(chunkWidth, chunkHeight);
    ProceduralScene(string name) : Scene(name) {
        //GenerateNoiseData();
    }



    void GenerateNoiseData() {
        int increment = 0;
        vertexIndex = 0;
        meshData.triangleIndex = 0;
        for (int x = 0; x < chunkWidth; x++) {
            for (int y = 0; y < chunkHeight; y++) {

                float sampleX = x + offsetX;
                float sampleY = y + offsetY;

                heightMap[x][y] = FractalBrownianMotion(sampleX, sampleY, octaves);//Noise
                glm::vec3 currentPoint = glm::vec3(moveLeftX + sampleX, heightMap[x][y] * frequency, moveLeftZ - sampleY);

                textureMap[x][y] = heightMap[x][y];
                meshData.vertices[vertexIndex].Position = currentPoint;
                meshData.vertices[vertexIndex].TexCoords = glm::vec2(x / (float)chunkWidth, y / (float)chunkHeight);

                //cout << heightMap[x][y] << ", ";

                float colourMapTranslate = (heightMap[x][y] + 1) / 2;

                int meshDataIndex = (y * chunkWidth + x) * 4;
                float colourShifter = 1;//Lerp(heightMap[x][y], 2, 1);

                if (heightMap[x][y] >= 0.5f) {
                    meshData.textureColourPlane[meshDataIndex] = (unsigned char)0;
                    meshData.textureColourPlane[meshDataIndex + 1] = (unsigned char)(200 / colourShifter);
                    meshData.textureColourPlane[meshDataIndex + 2] = (unsigned char)0;
                    meshData.textureColourPlane[meshDataIndex + 3] = (unsigned char)255;
                }
                else if (heightMap[x][y] >= 0.f) {
                    meshData.textureColourPlane[meshDataIndex] = (unsigned char)(150 / colourShifter);
                    meshData.textureColourPlane[meshDataIndex + 1] = (unsigned char)(floorf(75 / colourShifter));
                    meshData.textureColourPlane[meshDataIndex + 2] = (unsigned char)0;
                    meshData.textureColourPlane[meshDataIndex + 3] = (unsigned char)255;
                }
                else {
                    meshData.vertices[vertexIndex].Position.y = 0;
                    meshData.textureColourPlane[meshDataIndex] = (unsigned char)0;
                    meshData.textureColourPlane[meshDataIndex + 1] = (unsigned char)0;
                    meshData.textureColourPlane[meshDataIndex + 2] = (unsigned char)(floorf(Lerp(heightMap[x][y], 128, 255)) / colourShifter);
                    meshData.textureColourPlane[meshDataIndex + 3] = (unsigned char)255;
                }


                if (x < chunkWidth - 1 && y < chunkHeight - 1) {
                    meshData.AddTriangle(vertexIndex, vertexIndex + chunkWidth + 1, vertexIndex + chunkWidth);
                    meshData.AddTriangle(vertexIndex + chunkWidth + 1, vertexIndex, vertexIndex + 1);
                }
                vertexIndex++;
            }
        }
        cout << endl;
    }

    void GenerateMapGOs(Scene* scene) {
        GenerateNoiseData();
        models["generatedMeshChunk"] = new Model(Mesh(meshData.GenerateMesh()));
        int chunkIndex = 0;
        for (int x = 1; x < numberOfChunks; x++) {
            for (int y = 0; y < numberOfChunks; y++) {
                GenerateNoiseData();
                models["generatedMeshChunk"]->meshes.push_back(Mesh(meshData.GenerateMesh()));
                offsetY = y * (chunkHeight - 1);
            }
            offsetX = x * (chunkWidth - 1);
        }
        cout << models["generatedMeshChunk"]->meshes.size() << endl;
        scene->sceneGOs["mapChunk"] = new GameObject(models["generatedMeshChunk"], glm::vec3(moveLeftX, -15.f, moveLeftZ), glm::vec3(0), glm::vec3(1), glm::vec3(0), "floor");
        scene->sceneGOs["mapChunk"]->staticObj = true;

    }

    virtual Scene* InitScene() override {
        Scene* scene = scenes.at("Procedural");
        //models["generatedMesh"] = new Model(Mesh(meshData.GenerateMesh()));
        
        GenerateMapGOs(scene);

        //GameObject* floor = new GameObject(models["generatedMeshChunk" ], glm::vec3(0.f, -20.f, 0.f), glm::vec3(0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0), "floor");
        //floor->staticObj = true;

        //scene->sceneGOs["floor"] = floor;

        return scene;
    }

    void ScrollNoise() {

    }

    virtual void PassDataToScene(int shift = 0) override{
        offsetX++;
        offsetY++;
        if (shift == 1) {
            //GenerateNoiseData();
            //delete sceneGOs["mapChunk " + to_string(0)]->model;
            //sceneGOs["mapChunk " + to_string(0)]->model = new Model(Mesh(meshData.GenerateMesh()));
        }
    }

};

// Followed along from https://rtouti.github.io/graphics/perlin-noise-algorithm

glm::vec2 GetConstantVector(int permResult) {
    int vecCase = permResult & 3;
    if (vecCase == 0) return glm::vec2(1, 1);
    else if (vecCase == 1) return glm::vec2(-1, 1);
    else if (vecCase == 2) return glm::vec2(-1, -1);
    else return glm::vec2(1, -1);
    //cout << vecCase << endl;
}

float Fade(float t) {
    return ((6 * t - 15) * t + 10) * t * t * t;
}

float Lerp(float t, float var1, float var2) {
    return var1 + t * (var2 - var1);
}

float PerlinNoise(float x, float y) {
    int permTable[512];

    int permutation[] = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    for (int i = 0; i < 512; i++) {
        permTable[i] = permutation[i % 256];
    }

    //cout << "floorf x casted: " << (int)floorf(x) << " floorf x not casted: " << floorf(x) << endl;

    int cappedX = (int)floor(x) % 256;
    int cappedY = (int)floor(y) % 256;
    float fX = x - floorf(x);
    float fY = y - floorf(y);

    //cout << "intX: " << cappedX << ", intY: " << cappedY << ", floatX: " << fX << ", floatY: " << fY << endl;

    glm::vec2 topRight = glm::vec2(fX - 1, fY - 1);
    glm::vec2 topLeft = glm::vec2(fX, fY - 1);
    glm::vec2 bottomRight = glm::vec2(fX - 1, fY);
    glm::vec2 bottomLeft = glm::vec2(fX, fY);

    int topRightPerm = permTable[permTable[cappedX + 1] + cappedY + 1];
    int topLeftPerm = permTable[permTable[cappedX] + cappedY + 1];
    int bottomRightPerm = permTable[permTable[cappedX + 1] + cappedY];
    int bottomLeftPerm = permTable[permTable[cappedX] + cappedY];

    //cout << "TLP: " << topLeftPerm << ", TRP: " << topRightPerm << ", BLP: " << bottomLeftPerm << ", BRP: " << bottomRightPerm << endl;

    float topRightDot = glm::dot(topRight, GetConstantVector(topRightPerm));
    float topLeftDot = glm::dot(topLeft, GetConstantVector(topLeftPerm));
    float bottomLeftDot = glm::dot(bottomLeft, GetConstantVector(bottomLeftPerm));
    float bottomRightDot = glm::dot(bottomRight, GetConstantVector(bottomRightPerm));

    //cout << "TLD: " << topLeftDot << ", TRD: " << topRightDot << ", BLD: " << bottomLeftDot << ", BRD: " << bottomRightDot << endl;

    float xFade = Fade(fX);
    float yFade = Fade(fY);

    float leftLerp = Lerp(yFade, bottomLeftDot, topLeftDot);
    float rightLerp = Lerp(yFade, bottomRightDot, topRightDot);

    return Lerp(xFade, leftLerp, rightLerp);
}

float FractalBrownianMotion(float x, float y, int numOctaves) {
    float result = 0.f;
    float amplitude = 1.f;
    float frequency = 0.005f;

    for (int octave = 0; octave < numOctaves; octave++) {
        float currentNoise = amplitude * PerlinNoise(x * frequency, y * frequency);
        result += currentNoise;

        amplitude *= 0.5f;
        frequency *= 2.f;
    }
    return result;
}

class MeshGenerator {
    
};