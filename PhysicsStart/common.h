#pragma once
#ifndef COMMON_H
#define COMMON_H
#include "scene.h"
#include "camera.h"
#include "primitives.h"
#include "timer.h"


extern Scene* currentScene;
extern Camera camera;
//extern ISoundEngine* soundEngine;

extern Scene* LoadScene(Scene* currentScene, string sceneToLoad, Camera* cam);




#endif // !COMMON_H
