#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	:
	mDevice(nullptr), mDevCon(nullptr),
	mMaxParticles(0), mFirstRun(true),
	mGameTime(0.0f), mTimeStep(0.0f), mAge(0.0f),
	mEmitPos(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	mEmitDir(XMFLOAT3(0.0f, 1.0f, 0.0f)),
	mTexArraySRV(nullptr),
	mRandomTexSRV(nullptr),
	mInitVB(nullptr),
	mDrawVB(nullptr),
	mStreamOutVB(nullptr),
	mEffect(nullptr)
{
}

ParticleSystem::~ParticleSystem(void)
{
}

void ParticleSystem::Initialize(
	ID3D11Device* device, 
	ID3D11DeviceContext* devCon,
	ParticleEffect* effect, 
	std::string texPath,
	UINT maxParticles)
{
	this->mDevice = device;
	this->mDevCon = devCon;
	this->mEffect = effect;

	this->mTexArraySRV = Texture->GetTexArray(texPath);
	this->mRandomTexSRV = Texture->GetRandomTex();

	this->mMaxParticles = maxParticles;
	this->BuildVB(device);
}

void ParticleSystem::Update(float dt, float gametime)
{
	this->mTimeStep = dt;
	this->mAge += dt;
	this->mGameTime = gametime;
}

void ParticleSystem::Draw(Camera* camera)
{
	this->mDevCon->IASetInputLayout(InputLayouts::mParticle);
	this->mDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	mEffect->SetViewProj(camera->GetViewProjMatrix());

	mEffect->SetEyePos(camera->GetPosition());
	mEffect->SetEmitPos(this->mEmitPos);
	mEffect->SetEmitDir(this->mEmitDir);

	mEffect->SetGameTime(this->mGameTime);
	mEffect->SetTimeStep(this->mTimeStep);

	mEffect->SetTexArray(this->mTexArraySRV);
	mEffect->SetRandomTex(this->mRandomTexSRV);

	UINT stride = sizeof(VertexDef::Particle);
	UINT offset = 0;

	if(mFirstRun)
		this->mDevCon->IASetVertexBuffers(0, 1, &this->mInitVB, &stride, &offset);
	else
		this->mDevCon->IASetVertexBuffers(0, 1, &this->mDrawVB, &stride, &offset);

	this->mDevCon->SOSetTargets(1, &mStreamOutVB, &offset);

	ID3DX11EffectTechnique* activeTech = mEffect->GetTechnique();

	activeTech->GetPassByIndex(0)->Apply(0, this->mDevCon);
	if(mFirstRun)
	{
		this->mDevCon->Draw(1, 0);
		mFirstRun = false;
	}
	else
		this->mDevCon->DrawAuto();

	ID3D11Buffer* bufferArray[1] = {0};
	this->mDevCon->SOSetTargets(1, bufferArray, &offset);

	std::swap(mDrawVB, mStreamOutVB);
	this->mDevCon->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	activeTech->GetPassByIndex(1)->Apply(0, this->mDevCon);
	this->mDevCon->DrawAuto();
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vbd = {};
	
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexDef::Particle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	VertexDef::Particle p;
	ZeroMemory(&p, sizeof(VertexDef::Particle));
	p.mAge = 0.0f;
	p.mType = 0;

	D3D11_SUBRESOURCE_DATA vid = {};

	vid.pSysMem = &p;
	device->CreateBuffer(&vbd, &vid, &this->mInitVB);

	vbd.ByteWidth = sizeof(VertexDef::Particle) * this->mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	device->CreateBuffer(&vbd, 0, &this->mDrawVB);
	device->CreateBuffer(&vbd, 0, &this->mStreamOutVB);
}
