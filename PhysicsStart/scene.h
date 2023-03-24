#pragma once
#include <iostream>
#include <cstdlib>
#include "primitives.h"
#include "gameobjectchildren.h"
#include "camera.h"

int numOfScenes = 0;
class Scene;
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

class ProceduralScene : public Scene {
public:
    ProceduralScene(string name) : Scene(name) { 
        

    }

};