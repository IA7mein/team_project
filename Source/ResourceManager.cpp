#include "Graphics/Graphics.h"
#include "ResourceManager.h"


//���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	//���f��������
	auto it = models.find(filename);
	if (it != models.end())
	{
		std::shared_ptr<ModelResource> resource = it->second.lock();
		if (resource)
		{
			return resource;
		}
	}
	
	//�V�K���f�����\�[�X�쐬���ǂݍ���
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	if (model != nullptr)
	{
		model->Load(Graphics::Instance().GetDevice(), filename);
	}

	//�}�b�v�ɓo�^
	models[filename] = model;

	return model;
}