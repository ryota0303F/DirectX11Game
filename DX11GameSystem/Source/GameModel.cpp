#include "GameModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


GameModel::GameModel()
{
}

GameModel::~GameModel()
{
}

void GameModel::LoadModel(const std::string path)
{
    // loadèàóù
    Assimp::Importer importer;
    u_int flag = aiProcess_Triangulate;
    auto pScene = importer.ReadFile(path, flag);
}
