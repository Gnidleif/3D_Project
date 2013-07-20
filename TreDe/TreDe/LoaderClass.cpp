#include "LoaderClass.h"
#include <fstream>
#include <iostream>
using namespace std;

// Initialization functions of the loaderclass
LoaderClass* LoaderClass::mInstance = nullptr;

LoaderClass* LoaderClass::GetInstance()
{
	if(!mInstance)
		mInstance = new LoaderClass();
	return mInstance;
}

void LoaderClass::Shutdown()
{
	SafeDelete(mInstance);
}

void LoaderClass::Initialize(ID3D11Device* device)
{
	mDevice = device; // Might be useful later
}

// The heightmap part of it all!
void LoaderClass::CreateHeightMap(
	TerrainMesh* &mesh,
	string filename,
	float normFactor, 
	int texRepeat)
{
	this->LoadHeightMapFile(filename); // Opens the heightmap file and puts the positions into the mHeightMap
	this->NormalizeY(normFactor); // Normalizes y-coords by a certain factor
	this->CalcNormals(); // Calculates the normals and inputs them into the mHeightMap
	this->CalcTexCoords(texRepeat); // Calculates the texture coordinates and inputs them into the mHeightMap

	// Put the heightMap information in the terrainmesh
	int index(0), bLeft(0), bRight(0), uLeft(0), uRight(0);
	std::vector<VertexDef::PosNorTex> finalVertices((mRows*mCols) * 6);
	std::vector<UINT> finalIndices(finalVertices.size());
	for(int i(0); i != (mCols-1); ++i)
	{
		for(int j(0); j != (mRows-1); ++j)
		{
			bLeft = (mCols*i) + j;
			bRight = (mCols*i) + (j+1);
			uLeft = (mCols*(i+1)) + j;
			uRight = (mCols*(i+1)) + (j+1);

			finalVertices[index] = mHeightMap[uLeft];
			finalIndices[index] = index;
			index++;

			finalVertices[index] = mHeightMap[uRight];
			finalIndices[index] = index;
			index++;

			finalVertices[index] = mHeightMap[bLeft];
			finalIndices[index] = index;
			index++;

			finalVertices[index] = mHeightMap[bLeft];
			finalIndices[index] = index;
			index++;

			finalVertices[index] = mHeightMap[uRight];
			finalIndices[index] = index;
			index++;

			finalVertices[index] = mHeightMap[bRight];
			finalIndices[index] = index;
			index++;
		}
	}
	mesh->SetVerticeSize(finalVertices.size());
	mesh->SetIndiceSize(finalIndices.size());
	mesh->SetVertices(finalVertices);
	mesh->SetIndices(finalIndices);

	mesh->SetVertexBuffer(mDevice, &mesh->GetVertices()[0], mesh->GetVertices().size());
	mesh->SetIndexBuffer(mDevice, &mesh->GetIndices()[0], mesh->GetIndices().size());

	finalVertices.clear();
	finalIndices.clear();
}

// Private terrain functions
void LoaderClass::LoadHeightMapFile(std::string filename)
{
	// Open/load file
	FILE* ptr(nullptr);
	if( FAILED(fopen_s(&ptr, filename.c_str(), "rb")))
		std::cout << "Failed to open file." << std::endl;

	BITMAPFILEHEADER BMFileHeader;
	int count = fread(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, ptr);
	if(count != 1)
		std::cout << "BMFileHeader returned the wrong value." << std:: endl;

	BITMAPINFOHEADER BMInfoHeader;
	count = fread(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, ptr);
	if(count != 1)
		std::cout << "BMInfoHeader returned the wrong value." << std::endl;

	mCols = BMInfoHeader.biWidth;
	mRows = BMInfoHeader.biHeight;
	mHeightMap.resize(mCols * mRows);

	int imageSize = (mCols * mRows) * 3;
	unsigned char* BMImage = new unsigned char[imageSize];
	
	fseek(ptr, BMFileHeader.bfOffBits, SEEK_SET);
	
	count = fread(BMImage, 1, imageSize, ptr);
	if(count != imageSize)
		std::cout << "BMImage went retarded!" << std::endl;

	if( FAILED(fclose(ptr)))
		std::cout << "Failed to close file." << std::endl;

	unsigned char height('0');
	int index(0);
	int heightIndex(0);

	for(int i(0); i != mRows; ++i)
	{
		for(int j(0); j != mCols; ++j)
		{
			height = BMImage[heightIndex];
			index = (mRows * i) + j;
			mHeightMap[index].mPos = XMFLOAT3((float) j, (float) height, (float) i);
			heightIndex +=3;
		}
	}
	SafeDelete(BMImage);
}

void LoaderClass::NormalizeY(float factor)
{
	for(int i(0); i != mRows; ++i)
	{
		for(int j(0); j != mCols; ++j)
		{
			mHeightMap[(mRows*i) + j].mPos.y /= factor;
		}
	}
}

void LoaderClass::CalcNormals()
{
	int index(0), index1(0), index2(0), index3(0);
	XMFLOAT3 vertex1(0.0f, 0.0f, 0.0f), vertex2(0.0f, 0.0f, 0.0f), vertex3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 vector1(0.0f, 0.0f, 0.0f), vector2(0.0f, 0.0f, 0.0f);

	std::vector<VertexDef::PosNorTex> normals((mRows-1) * (mCols-1));
	for(int i(0); i != (mRows-1); ++i)
	{
		for(int j(0); j != (mCols-1); ++j)
		{
			index1 = (i*mRows) + j;
			index2 = (i*mRows) + (j+1);
			index3 = ((i+1)*mRows) + j;

			vertex1 = mHeightMap[index1].mPos;
			vertex2 = mHeightMap[index2].mPos;
			vertex3 = mHeightMap[index3].mPos;

			vector1 = XMFLOAT3(vertex1.x - vertex3.x, vertex1.y - vertex3.y, vertex1.z - vertex3.z);
			vector2 = XMFLOAT3(vertex3.x - vertex2.x, vertex3.y - vertex2.y, vertex3.z - vertex2.z);

			index = (i*(mRows-1)) + j;
			normals[index].mPos.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
			normals[index].mPos.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
			normals[index].mPos.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
		}
	}

	int count(0);
	float length(0.0f);
	XMFLOAT3 sum(0.0f, 0.0f, 0.0f);
	for(int i(0); i != (mRows - 1); ++i)
	{
		for(int j(0); j != (mCols - 1); ++j)
		{
			sum = XMFLOAT3(0.0f, 0.0f, 0.0f);
			if(((j-1) >= 0) && ((i-1) >= 0))
			{
				index = ((i-1) * ((int)mRows-1)) + (j-1);
				sum.x += normals[index].mPos.x;
				sum.y += normals[index].mPos.y;
				sum.z += normals[index].mPos.z;
				count++;
			}

			if((j < ((int)mCols-1)) && ((i - 1) >= 0))
			{
				index = ((i-1) * ((int)mRows-1)) + j;
				sum.x += normals[index].mPos.x;
				sum.y += normals[index].mPos.y;
				sum.z += normals[index].mPos.z;
				count++;
			}

			if(((j-1) >= 0) && (i < ((int)mRows-1)))
			{
				index = (i * ((int)mRows-1)) + (j-1);
				sum.x += normals[index].mPos.x;
				sum.y += normals[index].mPos.y;
				sum.z += normals[index].mPos.z;
				count++;
			}

			if((i < ((int)mCols-1)) && (j < ((int)mRows - 1)))
			{
				index = (i * ((int)mRows-1)) + j;
				sum.x += normals[index].mPos.x;
				sum.y += normals[index].mPos.y;
				sum.z += normals[index].mPos.z;
				count++;
			}

			sum.x = (sum.x / (float)count);
			sum.y = (sum.y / (float)count);
			sum.z = (sum.z / (float)count);

			length = sqrt((sum.x * sum.x) + (sum.y * sum.y) + (sum.z * sum.z));

			index = (i*(int)mRows) + j;
			mHeightMap[index].mNormal.x = 1.0f / (sum.x / length);
			mHeightMap[index].mNormal.y = 1.0f / (sum.y / length);
			mHeightMap[index].mNormal.z = 1.0f / (sum.z / length);
		}
	}
	normals.clear();
}

void LoaderClass::CalcTexCoords(int repeat)
{
	// Calculate texture coordinates
	float incVal = (float) repeat / mCols;
	int incCount = mRows / repeat;

	float tuCoord(0.0f), tvCoord(1.0f);
	int tuCount(0), tvCount(0);

	for(int i(0); i != mRows; ++i)
	{
		for(int j(0); j != mCols; ++j)
		{
			mHeightMap[((mRows*i) + j)].mTexCoord = XMFLOAT2(tuCoord, tvCoord);
			tuCoord += incVal;
			tuCount++;

			if(tuCount == incCount)
			{
				tuCoord = 0.0f;
				tuCount = 0;
			}
		}
		tvCoord += incVal;
		tvCount++;

		if(tvCount == incCount)
		{
			tvCoord = 1.0f;
			tvCount = 0;
		}
	}
}

// The static object part of the entirety of the thingymabob
bool LoaderClass::CreateStaticObject(
	std::vector<GenericMaterial>& materials,
	std::vector<StaticMesh*>& meshes,
	string filename)
{
	using namespace Assimp;
	Importer importer;
	string logName("../Data/StaticMeshLoader.log");
	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	const aiScene* scene = importer.ReadFile(
		filename,
		aiProcess_ConvertToLeftHanded |
		aiProcessPreset_TargetRealtime_Quality);

	if(!scene)
	{
		DefaultLogger::get()->info("Reading static object file failed!");
		DefaultLogger::kill();
		return false;
	}

	if(scene->HasMeshes())
	{		
		CreateMaterials(scene, materials);

		for(UINT curMesh(0); curMesh != scene->mNumMeshes; ++curMesh)
		{
			StaticMesh* tempMesh = new StaticMesh();
			aiMesh* mesh(scene->mMeshes[curMesh]);
			aiString meshName(mesh->mName);

			UINT numVertices(mesh->mNumVertices);
			UINT numFaces(mesh->mNumFaces);

			vector<VertexDef::PosNorTexTan> vertices;
			for(UINT i(0); i != numVertices; ++i)
			{
				VertexDef::PosNorTexTan vertex;
				vertex.mPos			= XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.mNormal		= XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.mTexCoord	= XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				vertex.mTangentU	= XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f);

				if(mesh->HasTangentsAndBitangents())
				{
					vertex.mTangentU = XMFLOAT4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, -1.0f);
				}
				vertices.push_back(vertex);
			}
			tempMesh->SetVerticeSize(vertices.size());
			tempMesh->SetVertices(vertices);

			vector<UINT> indices;
			for(UINT i(0); i != numFaces; ++i)
			{
				for(UINT j(0); j < 3; ++j)
				{
					indices.push_back(mesh->mFaces[i].mIndices[j]);
				}
			}
			// Might need to set some more shit here
			tempMesh->SetIndiceSize(indices.size());
			tempMesh->SetIndices(indices);

			tempMesh->SetMaterialIndex(mesh->mMaterialIndex);

			tempMesh->SetVertexBuffer(mDevice, &tempMesh->GetVertices()[0], tempMesh->GetVertices().size());
			tempMesh->SetIndexBuffer(mDevice, &tempMesh->GetIndices()[0], tempMesh->GetIndices().size());

			meshes.push_back(tempMesh);
		}
	}
	DefaultLogger::kill();
	return true;
}

// Functions related to SkinnedObjects
bool LoaderClass::CreateSkinnedObject(
	std::vector<GenericMaterial>& materials,
	std::vector<SkinnedMesh*>& meshes,
	std::string filename,
	SkinData* &skinData)
{
	using namespace Assimp;
	Importer importer;
	string logName("../Data/StaticMeshLoader.log");
	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	const aiScene* scene = importer.ReadFile(
		filename,
		aiProcess_ConvertToLeftHanded |
		aiProcessPreset_TargetRealtime_Quality);

	if(!scene)
	{
		DefaultLogger::get()->info("Reading static object file failed!");
		DefaultLogger::kill();
		return false;
	}

	if(scene->HasMeshes())
	{
		SkinDef::Bone* skeleton = CreateBoneTree(scene->mRootNode, nullptr, skinData);
		skinData->SetSkeleton(skeleton);
		//std::map<std::string, SkinDef::Bone*> boneNames = skinData.GetBoneNames();
		CreateAnimations(scene, skinData);
		CreateMaterials(scene, materials);

		for(UINT curMesh(0); curMesh != scene->mNumMeshes; ++curMesh)
		{
			SkinnedMesh* tempMesh = new SkinnedMesh();
			aiMesh* mesh(scene->mMeshes[curMesh]);
			aiString meshName(mesh->mName);

			UINT numVertices(mesh->mNumVertices);
			UINT numFaces(mesh->mNumFaces);

			std::vector<VertexDef::PosNorTexTanSkin> vertices;
			for(UINT i(0); i != numVertices; ++i)
			{
				VertexDef::PosNorTexTanSkin vertex;
				vertex.mPos			= XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.mNormal		= XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.mTexCoord	= XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				vertex.mTangentU	= XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f);

				if(mesh->HasTangentsAndBitangents())
				{
					vertex.mTangentU = XMFLOAT4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, -1.0f);
				}

				vertex.mBoneIndices[0] = vertex.mBoneIndices[1] = vertex.mBoneIndices[2] = vertex.mBoneIndices[3] = 0;
				vertex.mWeights = XMFLOAT3(0.0f, 0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			vector<UINT> indices;
			for(UINT i(0); i != numFaces; ++i)
			{
				for(UINT j(0); j < 3; ++j)
				{
					indices.push_back(mesh->mFaces[i].mIndices[j]);
				}
			}

			std::map<std::string, SkinDef::Bone*> tempBoneNames = skinData->GetBoneNames();
			for(UINT i(0); i != mesh->mNumBones; ++i)
			{
				aiBone* bone = mesh->mBones[i];
				auto it = tempBoneNames.find(bone->mName.data);
				auto end = tempBoneNames.end();

				if(it != end)
				{
					bool skip = false;
					for(UINT j(0); j != skinData->GetBones().size(); ++j)
					{
						std::string boneName = bone->mName.data;
						if(skinData->GetBone(j)->mName == boneName)
						{
							skip = true;
							break;
						}
					}

					if(!skip)
					{
						std::string test = it->second->mName;
						it->second->mOffset = SkinDef::ReadAiMatrix(bone->mOffsetMatrix);
						XMMATRIX offsetMatrix = XMLoadFloat4x4(&it->second->mOffset);
						offsetMatrix = XMMatrixTranspose(offsetMatrix);
						XMStoreFloat4x4(&it->second->mOffset, offsetMatrix);

						skinData->InsertBone(it->second);
						skinData->InsertBoneIndice(it->first, skinData->GetBones().size()-1);
					}
				}
				// Check here later if anything messes up, this looks weird
				for(UINT j(0); j != bone->mNumWeights; ++j)
				{
					if(vertices[bone->mWeights[j].mVertexId].mWeights.x == 0.0f)
					{
						vertices[bone->mWeights[j].mVertexId].mBoneIndices[0] = skinData->GetBones().size()-1;
						vertices[bone->mWeights[j].mVertexId].mWeights.x = bone->mWeights[j].mWeight;
					}
					else if(vertices[bone->mWeights[j].mVertexId].mWeights.y == 0.0f)
					{
						vertices[bone->mWeights[j].mVertexId].mBoneIndices[1] = skinData->GetBones().size()-1;
						vertices[bone->mWeights[j].mVertexId].mWeights.y = bone->mWeights[j].mWeight;
					}
					else if(vertices[bone->mWeights[j].mVertexId].mWeights.z == 0.0f)
					{
						vertices[bone->mWeights[j].mVertexId].mBoneIndices[2] = skinData->GetBones().size()-1;
						vertices[bone->mWeights[j].mVertexId].mWeights.z = bone->mWeights[j].mWeight;
					}
					else 
					{
						vertices[bone->mWeights[j].mVertexId].mBoneIndices[3] = skinData->GetBones().size()-1;
					}
				}
			}

			tempMesh->SetIndiceSize(indices.size());
			tempMesh->SetIndices(indices);

			tempMesh->SetVerticeSize(vertices.size());
			tempMesh->SetVertices(vertices);

			tempMesh->SetMaterialIndex(mesh->mMaterialIndex);

			tempMesh->SetVertexBuffer(mDevice, &tempMesh->GetVertices()[0], tempMesh->GetVertices().size());
			tempMesh->SetIndexBuffer(mDevice, &tempMesh->GetIndices()[0], tempMesh->GetIndices().size());

			meshes.push_back(tempMesh);
		}

		skinData->SetTransVectorSize(skinData->GetBones().size());
		std::vector<std::vector<XMFLOAT4X4>> animVector;

		float timeStep = 1.0f/30.0f;

		for(UINT i(0); i != skinData->GetAnimations().size(); ++i)
		{
			skinData->SetAnimIndex(i);
			float dt = 0;
			for(float ticks(0.0f); ticks < skinData->GetAnimation(i).GetDuration(); ticks += skinData->GetAnimation(i).GetTicksPerSec()/30.0f)
			{
				dt += timeStep;
				skinData->CalcTransform(dt);
				animVector.push_back(std::vector<XMFLOAT4X4>());
				std::vector<XMFLOAT4X4>& trans = animVector.back();

				for(UINT j(0); j != skinData->GetTransVector().size(); ++j)
				{
					XMMATRIX offset = XMLoadFloat4x4(&skinData->GetBone(i)->mOffset);
					XMMATRIX global = XMLoadFloat4x4(&skinData->GetBone(i)->mGlobalTrans);
					XMMATRIX rotation = XMMatrixMultiply(offset, global);

					XMFLOAT4X4 rotation4x4;
					XMStoreFloat4x4(&rotation4x4, rotation);
					trans.push_back(rotation4x4);
				}
				skinData->SetAnimTransVector(i, animVector);
			}

		}
	}

	DefaultLogger::kill();
	return true;
}

SkinDef::Bone* LoaderClass::CreateBoneTree(aiNode* node, SkinDef::Bone* parent, SkinData* &skinData)
{
	SkinDef::Bone* internalNode = new SkinDef::Bone();
	internalNode->mName = node->mName.data;
	internalNode->mParent = parent;
	skinData->InsertBoneName(internalNode->mName, internalNode);
	
	internalNode->mLocalTrans = SkinDef::ReadAiMatrix(node->mTransformation);

	XMMATRIX LocTransfTransp = XMLoadFloat4x4(&internalNode->mLocalTrans);
	LocTransfTransp = XMMatrixTranspose(LocTransfTransp);
	XMStoreFloat4x4(&internalNode->mLocalTrans, LocTransfTransp);
	internalNode->mOrigLocalTrans = internalNode->mLocalTrans;

	SkinDef::CalculateBoneToWorld(internalNode);

	for(UINT i(0); i != node->mNumChildren; ++i)
	{
		internalNode->mChildren.push_back(CreateBoneTree(node->mChildren[i], internalNode, skinData));
	}
	return internalNode;
}

void LoaderClass::CreateAnimations(const aiScene* scene, SkinData* &skinData)
{
	for(UINT i(0); i != scene->mNumAnimations; ++i)
	{
		skinData->InsertAnimation(AnimEvaluator(scene->mAnimations[i]));
	}

	for(UINT i(0); i != skinData->GetAnimations().size(); ++i)
	{
		skinData->InsertAnimNameID(skinData->GetAnimation(i).GetName(), i);
	}

	skinData->SetAnimation("Idle");
}

// Function used to create the various materials that a mesh might have
void LoaderClass::CreateMaterials(const aiScene* scene, vector<GenericMaterial>& materials)
{
	if(scene->HasMaterials())
	{
		for(UINT curMat(0); curMat != scene->mNumMaterials; ++curMat)
		{
			aiString name;
			aiColor3D 
				ambient(0.0f, 0.0f, 0.0f), 
				diffuse(0.0f, 0.0f, 0.0f), 
				specular(0.0f, 0.0f, 0.0f), 
				reflect(0.0f, 0.0f, 0.0f);

			scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_REFLECTIVE, reflect);

			GenericMaterial tempMat;
			tempMat.mName = name.C_Str();
			tempMat.mMaterial.Ambient = XMFLOAT4(ambient.r, ambient.g, ambient.b, 1.0f);
			tempMat.mMaterial.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, 1.0f);
			tempMat.mMaterial.Specular = XMFLOAT4(specular.r, specular.g, specular.b, 1.0f);
			tempMat.mMaterial.Reflect = XMFLOAT4(reflect.r, reflect.g, reflect.b, 1.0f);

			aiString path;
			if(scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
			{
				string texPathStr = path.C_Str();
				wstring tempString(texPathStr.begin(), texPathStr.end());

				wchar_t firstChar = tempString.front();
				while(firstChar == '.' || firstChar == '/' || firstChar == '\'')
				{
					tempString.erase(tempString.begin(), tempString.begin()+1);
					firstChar = tempString.front();
				}

				wchar_t curChar;
				int index(-1);
				for(UINT i(0); i != tempString.size(); ++i)
				{
					curChar = tempString[i];
					if(curChar == '\'' || curChar == '/')
						index = i;
				}
				if(index != -1)
					tempString.erase(0, index+1);

				for(UINT i(0); i != tempString.size(); ++i)
				{
					if(tempString[i] == '\'')
					{
						tempString.replace(i, 1, L"\\");
					}
				}
				string temp(tempString.begin(), tempString.end());
				tempMat.mDiffMapName = temp;
			}

			if(scene->mMaterials[curMat]->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
			{
				string normalMapPathStr = path.C_Str();
				wstring tempString(normalMapPathStr.begin(), normalMapPathStr.end());

				wchar_t firstChar = tempString.front();
				while(firstChar == '.' || firstChar == '/' || firstChar == '\'')
				{
					tempString.erase(tempString.begin(), tempString.begin()+1);
					firstChar = tempString.front();
				}

				for(UINT i(0); i != tempString.size(); ++i)
				{
					if(tempString[i] == L'/')
					{
						tempString.replace(i, 1, L"\\");
					}
				}
				string temp(tempString.begin(), tempString.end());
				tempMat.mNormalMapName = temp;
			}
			materials.push_back(tempMat);
		}
	}
}

// Constructor

LoaderClass::LoaderClass()
	:
	mRows(0), 
	mCols(0), 
	mHeightMap(std::vector<VertexDef::PosNorTex>())
{
}