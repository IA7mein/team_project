#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

//������
void SceneLoading::Initialize()
{
	//�X�v���C�g������
	//�X���b�h�J�n(��1�����ɓ��������������A��2�����Ɋ֐��̑�1����)
	thread = new std::thread(LoadingThread, this);
}

//�I����
void SceneLoading::Finalize()
{
	//�X���b�h�I����
	if (thread != nullptr)
	{
		//�X���b�h�I���܂őҋ@
		thread->join();
		//�X���b�h��j��
		delete thread;
		//�X���b�h��null������
		thread = nullptr;
	}
}

//�X�V����
void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//���̃V�[���̏���������������V�[����؂�ւ���
	//���̃V�[����ready��true���ǂ����`�F�b�N
	if (nextScene->IsReady() != false)
	{
		//�V�[���}�l�[�W���[��nextScene�ɐ؂�ւ���
		SceneManager::Instance().ChangeScene(nextScene);
		//nextScene��null������
		nextScene = nullptr;
	}
}

//�`�揈��
void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	//���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	//�X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	//���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}