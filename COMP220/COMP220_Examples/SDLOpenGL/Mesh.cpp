#pragma once

#include "Mesh.h"




void Mesh::init(const std::string& filename, GLuint programID)
{
	programToUse = programID;
	loadMeshFromFile(filename,subMeshes);
	copyBufferData();
	MVPLoc = { glGetUniformLocation(programID, "modelMatrix"),
		glGetUniformLocation(programID, "viewMatrix"),
		glGetUniformLocation(programID, "projectionMatrix") };
	lightDirectionLoc = glGetUniformLocation(programID, "lightLocation");

}

void Mesh::render(Camera &camera) 
{
	glUseProgram(programToUse);
	tempLightDir = normalize(lightSource - worldPos);
	MVPMatrix = calculateTransform(camera, aspectRatio, worldPos, worldRot, worldScale);
	directionFromLightSource[0] = tempLightDir.x;
	directionFromLightSource[1] = tempLightDir.y;
	directionFromLightSource[2] = tempLightDir.z;
	glUniform3fv(lightDirectionLoc, 1, directionFromLightSource);
	glUniformMatrix4fv(MVPLoc.modelMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.modelMatrix));
	glUniformMatrix4fv(MVPLoc.viewMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.viewMatrix));
	glUniformMatrix4fv(MVPLoc.projectionMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.projectionMatrix));
	for (int i = 0; i < subMeshes.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, subMeshes[i]->m_VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i]->m_EBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((void*)offsetof(Vertex, vertexCol)));

		if (subMeshes[i]->hasTexture)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((void*)offsetof(Vertex, textureCoords)));
			
		}
		if (subMeshes[i]->lightMe)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((void*)offsetof(Vertex, vertexNormals)));
			printf("lighting layer being called\n");
		}
		glDrawElements(GL_TRIANGLES, subMeshes[i]->meshElementArray.size(), GL_UNSIGNED_INT, 0);
	}
}

void Mesh::movement(float move)
{
	worldPos += worldRot*move;
	worldRot = normalize(vec3(0.0f, 0.0f, 0.0f) - worldPos);
}

void Mesh::copyBufferData()
{
	for (int i = 0; i < subMeshes.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, subMeshes[i]->m_VBO);
		glBufferData(GL_ARRAY_BUFFER, subMeshes[i]->meshVertex.size() * sizeof(Vertex), &subMeshes[i]->meshVertex[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i]->m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i]->meshElementArray.size() * sizeof(int), &subMeshes[i]->meshElementArray[0], GL_STATIC_DRAW);
	}
}

bool loadMeshFromFile(const std::string& filename, std::vector<subMesh*> &meshes)
{

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (!scene)
	{
		printf("Model Loading Error - %s\n", importer.GetErrorString());
		return false;
	}


	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *currentMesh = scene->mMeshes[i];
		subMesh *pSubMesh = new subMesh();
		pSubMesh->init();
		Vertex currentVertex;
		for (int v = 0; v < currentMesh->mNumVertices; v++)
		{
			if (currentMesh->mVertices[v].x != NULL)
			{
				aiVector3D currentModelVertex = currentMesh->mVertices[v];
				currentVertex.vertexPos = { currentModelVertex.x, currentModelVertex.y, currentModelVertex.z };
				currentVertex.vertexCol = { 1.0f, 1.0f, 1.0f, 1.0f };
			}

			if (currentMesh->HasNormals())
			{
			aiVector3D currentNormals = currentMesh->mNormals[v];
			currentVertex.vertexNormals = { currentNormals.x, currentNormals.y,currentNormals.z };
			pSubMesh->lightMe = true;
			}

			if (currentMesh->HasTextureCoords(0))
			{
				aiVector3D currentTextureCoordinates = currentMesh->mTextureCoords[0][v];
				currentVertex.textureCoords = { currentTextureCoordinates.x, currentTextureCoordinates.y };
				pSubMesh->hasTexture = true;
			}
			else 
			{
				currentVertex.vertexCol = { 0.0f, 0.0f, sin(rand() % 100), 1.0f };
				pSubMesh->hasTexture = false;
				
			}
			pSubMesh->meshVertex.push_back(currentVertex);
		}
		for (int f = 0; f < currentMesh->mNumFaces; f++)
		{
			aiFace currentModelFace = currentMesh->mFaces[f];
			pSubMesh->meshElementArray.push_back(currentModelFace.mIndices[2]);
			pSubMesh->meshElementArray.push_back(currentModelFace.mIndices[0]);
			pSubMesh->meshElementArray.push_back(currentModelFace.mIndices[1]);
		}
		pSubMesh->copyBufferData();

		meshes.push_back(pSubMesh);
		vertices.clear();
		indices.clear();
	}

	return true;
}

