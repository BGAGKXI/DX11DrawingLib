#include "Scene.h"
#include "key_input.h"
//**********************************************
//
//		�V�[���̊Ǘ�
//
//**********************************************


void SceneManager::Update()
{
	std::unique_ptr<Scene> newScene = nullptr;
	newScene = scene->getScene();
	if (newScene) {				//�`�F�b�N
		ChangeScene(std::move(newScene));
	}

	KeyInput::Update();
	scene->Update();
}

void SceneManager::Render()
{

	scene->Render();
}

void SceneManager::ChangeScene(std::unique_ptr<Scene> newScene)
{
	scene = std::move(newScene);
	scene->Initialize();
}
