#include "PostProcessBloom.h"

void PostProcessBloom::renderLuminance()
{
	bind2ndBuff();

	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind our Postprocessing Program
	glUseProgram(bloomShader1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTextureID);
	glUniform1i(firstTextureLoc0, 0);

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PostProcessBloom::PostProcBloomInit(const char* vertShader, int numberOfBlurs, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	bloomShader1 = LoadShaders(vertShader, "Shaders/PostProcBloomFragPass1.txt");
	bloomShader2 = LoadShaders(vertShader, "Shaders/PostProcBloomFragPass2.txt");
	bloomShader3 = LoadShaders(vertShader, "Shaders/PostProcBloomFragPass3.txt");

	resolution =1024;
	radius = 4;
	timesToBlur = numberOfBlurs;


	glGenRenderbuffers(1, &depthBufferID);
	//Bind the depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
	//Set the format of the depth buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Create textures
	sceneTextureID =     createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	luminanceTextureID = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	verticalTextureID =  createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	combinedTextureID =  createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Generate buffers using the same depth buffer but different texureIDs
	generateBuffers(firstFrameBufferID, sceneTextureID);
	generateBuffers(secondFrameBufferID, luminanceTextureID);
	generateBuffers(thirdFrameBufferID, verticalTextureID);
	generateBuffers(fourthFrameBufferID, combinedTextureID);
	
	//create fullscreen quad
	GLfloat vertices[8] = { -1.0f, -1.0f, 1.0f, -1.0f,-1.0f, 1.0f, 1.0f, 1.0f };

	screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	screenVAO;
	glGenVertexArrays(1, &screenVAO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// first shader uniforms
	firstTextureLoc0 = glGetUniformLocation(bloomShader1, "texture0");

	//second shader uniforms
	secondTextureLoc0 = glGetUniformLocation(bloomShader2, "texture0");
	secondResolutionLoc = glGetUniformLocation(bloomShader2, "resolution");
	secondRadiusLoc = glGetUniformLocation(bloomShader2, "radius");
	directionLoc = glGetUniformLocation(bloomShader2, "direction");

	//third shader uniforms
	thirdTextureLoc0 = glGetUniformLocation(bloomShader3, "texture0");
	thirdTextureLoc1 = glGetUniformLocation(bloomShader3, "texture1");
	thirdResolutionLoc = glGetUniformLocation(bloomShader3, "resolution");
	thirdRadiusLoc = glGetUniformLocation(bloomShader3, "radius");
	unBindBuffer();
}

void PostProcessBloom::applyBloom()
{
	renderLuminance();
	firstBlur();
	for(int i = 0; i < timesToBlur; i++)	increaseBlur();
	finalDraw();
}

void PostProcessBloom::firstBlur()
{
	bind3rdBuff();
	direction = { 1.0,0.0 };
	blurTexture(luminanceTextureID);

	bind4thBuff();
	direction = { 0.0,1.0 };
	blurTexture(verticalTextureID);
}

PostProcessBloom::~PostProcessBloom()
{

	glDeleteBuffers(1, &screenQuadVBOID);
	glDeleteVertexArrays(1, &screenVAO);

	glDeleteProgram(bloomShader1);
	glDeleteProgram(bloomShader2);
	glDeleteProgram(bloomShader3);

	glDeleteTextures(1, &luminanceTextureID);
	glDeleteTextures(1, &verticalTextureID);
	glDeleteTextures(1, &sceneTextureID);
	glDeleteTextures(1, &combinedTextureID);

	glDeleteRenderbuffers(1, &depthBufferID);

	glDeleteFramebuffers(1, &firstFrameBufferID);

	glDeleteFramebuffers(1, &secondFrameBufferID);

	glDeleteFramebuffers(1, &thirdFrameBufferID);

	glDeleteFramebuffers(1, &fourthFrameBufferID);
}

void PostProcessBloom::blurTexture(GLuint Texture)
{
	glUseProgram(bloomShader2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glUniform1i(secondTextureLoc0, 0);
	glUniform1f(secondRadiusLoc, radius);
	glUniform1f(secondResolutionLoc, resolution);

	glUniform2fv(directionLoc, 1, value_ptr(direction));

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void PostProcessBloom::increaseBlur()
{
		bind3rdBuff();
		direction = { 1.0,0.0 };
		blurTexture(combinedTextureID);

		bind4thBuff();
		direction = { 0.0, 1.0 };
		blurTexture(verticalTextureID);
}
void PostProcessBloom::finalDraw()
{

	unBindBuffer();
	//Bind our Postprocessing Program
	glUseProgram(bloomShader3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTextureID);
	glUniform1i(thirdTextureLoc0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, combinedTextureID);
	glUniform1i(thirdTextureLoc1, 1);

	glUniform1f(thirdRadiusLoc, radius);
	glUniform1f(thirdResolutionLoc, resolution);

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	unBindBuffer();
}
void PostProcessBloom::generateBuffers( GLuint  & frameBuffID, GLuint & texture)
{

	//The frambuffer
	frameBuffID;
	glGenFramebuffers(1, &frameBuffID);

	//Bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffID);

	//Bind the depth buffer as a depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
	//Bind the texture as a colour attachment 0 to the active framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "checkFrameBufferStatus fail:" << endl;
	}

}