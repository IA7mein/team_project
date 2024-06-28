#include "Graphics/Graphics.h"
#include "ResourceManager.h"


//モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	//モデルを検索
	auto it = models.find(filename);
	if (it != models.end())
	{
		std::shared_ptr<ModelResource> resource = it->second.lock();
		if (resource)
		{
			return resource;
		}
	}
	
	//新規モデルリソース作成＆読み込み
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	if (model != nullptr)
	{
		model->Load(Graphics::Instance().GetDevice(), filename);
	}

	//マップに登録
	models[filename] = model;

	return model;
}