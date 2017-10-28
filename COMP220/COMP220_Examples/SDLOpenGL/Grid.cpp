#pragma once
#include "Grid.h"

void Grid::createGridVec(int numberX, int numberY)
{
	glGenBuffers(1, &lineBuff);
	int tempy = 0;
	int tempx = 0;
	for (int i = 0; i < numberX; i++)
	{
		for (int j = 0; j < numberY; j++)
		{
			tempy++;
			vec3 lineVert1 = vec3(i, -1, j);
			vec3 lineVert2 = vec3(i, -1, 0);
			vec3 lineVert3 = vec3(i, -1, j);
			vec3 lineVert4 = vec3(0, -1, j);
			vec4 col = vec4(0.5, 0.5, 0.5, 1.0);
			vec4 col1 = vec4(0.5, 0.5, 0.5, 1.0);
			if (tempy == 10)
			{
				col = vec4(0.1, 1.0, 0.1, 1.0);
				tempy = 0;
			}
			if (tempx == 10)
			{
				col1 = vec4(0.1, 1.0, 0.1, 1.0);
				tempx = 0;
			}
			LineVertex lineVertex = { lineVert1, col1 };
			LineVertex lineVertex2 = { lineVert2, col1 };
			LineVertex lineVertex3 = { lineVert3, col };
			LineVertex lineVertex4 = { lineVert4, col };
			lineVerts.push_back(lineVertex);
			lineVerts.push_back(lineVertex2);
			lineVerts.push_back(lineVertex3);
			lineVerts.push_back(lineVertex4);
		}
		tempx++;
	}
}

void Grid::draw(MVP &MVPLocation, Camera &camera, float aspectRatio)
{
	MVPMatrix = calculateTransform(camera, aspectRatio);
	glUniformMatrix4fv(MVPLocation.modelMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.modelMatrix));
	glUniformMatrix4fv(MVPLocation.viewMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.viewMatrix));
	glUniformMatrix4fv(MVPLocation.projectionMatrixLocation, 1, GL_FALSE, value_ptr(MVPMatrix.projectionMatrix));
	glBindBuffer(GL_ARRAY_BUFFER, lineBuff);
	glBufferData(GL_ARRAY_BUFFER, lineVerts.size() * sizeof(LineVertex), &lineVerts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), ((void*)offsetof(LineVertex, vertexCol)));
	glDrawArrays(GL_LINES, 0, lineVerts.size());

}

Grid::~Grid()
{
	glDeleteBuffers(1, &lineBuff);
}