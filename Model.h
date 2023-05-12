#pragma once
#include "TextureManager.h"
#include "Common.h"

// �萔�o�b�t�@�p �f�[�^�\���� (�}�e���A��)
struct ConstBufferDataMaterial
{
	XMFLOAT3 ambient;
	float pad1;

	XMFLOAT3 diffuse;
	float pad2;

	XMFLOAT3 specular;
	float alpha;

	XMFLOAT4 color; // �F(RGBA)
};

struct Node
{
	// ���O
	string name;

	// ���[�J���X�P�[��
	XMVECTOR scaling = { 1,1,1,0 };
	// ���[�J����]�p
	XMVECTOR rotation = { 0,0,0,0 };
	// ���[�J���ړ�
	XMVECTOR translation = { 0,0,0,1 };
	// ���[�J���ό`�s��
	XMMATRIX transform;
	// �O���[�o���ό`�s��
	XMMATRIX globalTransform;

	// �e�m�[�h
	Node* parent = nullptr;

};

class Model
{
public:
	// �t�����h�N���X
	friend class FbxLoader;

private:
	// ���f����
	string name;
	// �m�[�h�z��
	vector<Node> nodes;

public:
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos;	// xyz���W
		XMFLOAT3 normal;// �@���x�N�g�� 
		XMFLOAT2 uv;	// uv���W
	};

	// ���b�V�������m�[�h
	Node* meshNode = nullptr;
	// ���_�f�[�^�z��
	vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	vector<unsigned short> indices;

	// �}�e���A��
	struct Material
	{
		std::string name;
		XMFLOAT3 ambient;
		XMFLOAT3 diffuse;
		XMFLOAT3 specular;
		float alpha;

		TextureIndex index = -1;

		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};	

	Material material;

public:
	Model();

public:
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	// OBJ�t�@�C������RD���f���ǂݍ���
	static Model LoadFromOBJ(const std::string& modelName);

	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void CreateBuffers();

	void Draw();
		
private:
	void LoadFromOBJInternal(const std::string& modelName);

private:
	std::vector<unsigned short> indices;
	std::vector<VertexPosNormalUv> vertices;

	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
};