
#pragma once
#include <GL\glew.h>
#include <SDL_opengl.h>
#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

/**
* CREDIT FOR THIS CLASS RICH https://github.com/Stompyy/bsc-live-coding/blob/2017-18/COMP220/COMP220_Examples/92_Refactor/Material.h
*/
class Material
{
public:
	/**
	*defualt constructor for material class
	*/
	Material();

	/**
	*default deconstructor for the material class
	*/
	~Material();

	/**
	*Sets the ambient colour of this instance of the Material class
	*/
	void setAmbientColour(const float r, const float g, const float b, const float a) { m_AmbientColour = vec4(r, g, b, a); }
	
	/**
	*Gets the current value of the ambient colour stored in this Material class
	*/
	vec4& getAmbientColour() { return m_AmbientColour; }

	/**
	*Sets the diffuse colour for this instance of the material class
	*/
	void setDiffuseColour(const float r, const float g, const float b, const float a) { m_DiffuseColour = vec4(r, g, b, a); }

	/**
	*Gets the diffuse colour for this instance of the material class
	*/
	vec4& getDiffuseColour() { return m_DiffuseColour; }

	/**
	*Sets the specular colour for this instance of the material class
	*/
	void setSpecularColour(const float r, const float g, const float b, const float a) { m_SpecularColour = vec4(r, g, b, a); }

	/**
	*Gets the specular colour for this instance of the material class
	*/
	vec4& getSpecularColour() { return m_SpecularColour; }

	/**
	*Sets the specular power for this instance of the material class
	*/
	void setSpecularPower(const float newSpecularPower) { m_SpecularPower = newSpecularPower; }

	/**
	*Gets the specular power for this instance of the material class
	*/
	float getSpecularPower() { return m_SpecularPower; }

	void setAmbientColour(const float r, const float g, const float b) { m_AmbientColour = vec4(r, g, b, 1.0f); }

	void setDiffuseColour(const float r, const float g, const float b) { m_DiffuseColour = vec4(r, g, b, 1.0f); }

	void setSpecularColour(const float r, const float g, const float b) { m_SpecularColour = vec4(r, g, b, 1.0f); }
	


private:
	vec4 m_AmbientColour;
	vec4 m_DiffuseColour;
	vec4 m_SpecularColour;
	float m_SpecularPower;
};

/**
*This class is just for the GLuinform variable locations
*/
class MaterialLocation
{
public:

	/**Gets the uniform locations of the material
	*The program to get the locations from
	*/
	void updateLocations(GLuint programToUse);

	/**Sends the uniforms accross to the shader
	*The material that is to be sent accross to the locations stored in this class
	*/
	void sendUniforms(Material & material);

	//location variables
	GLint m_SpecularColourLoc;
	GLint m_DiffuseColourLoc;
	GLint m_AmbientColourLoc;
	GLint m_SpecularPowerLoc;
};