//main.cpp - defines the entry point of the application

#include "main.h"



bool Init()
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_Init failed", NULL);
		return false;
	}

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_CreateWindow failed", NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return false;
	}

	//request OpenGL core 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "context failed", NULL);
		SDL_DestroyWindow(window);
		SDL_QUIT;
		return false;
	}
	//Init GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (char*)glewGetErrorString(err), "GLEW INIT failed", NULL);
		return false;
	}
	SDL_SetRelativeMouseMode(SDL_bool(SDL_ENABLE));
	return true;

}

void Close() 
{
	//DeleteContext
	SDL_GL_DeleteContext(glContext);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();
}

int main(int argc, char* args[])
{

	//initialise SDL and OPENGL
	if (Init() != true)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "INIT FAILED CLOSING PROGRAM", NULL);
		Close();
	}

	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is close
	bool running = true;

	//create new camera
	camera = new Camera(aspectRatio);

	//Create physics simulation
	physSim = new PhysicsSimulation;
	btRigidBody* ground = physSim->creatRigidBodyCube(btVector3(100, 1, 100), 0, btVector3(0, -1, 0));
	btRigidBody* celing = physSim->creatRigidBodyCube(btVector3(100, 1, 100), 0.0, btVector3(0, 56, 0));

	// Load Shaders
	GLuint defaultShader = LoadShaders("Shaders/vertexShader.txt", "Shaders/fragmentShader.txt");
	GLuint TextureShader = LoadShaders("Shaders/TexVert.txt", "Shaders/TexFrag.txt");
	GLuint TexLightShader = LoadShaders("Shaders/TexLightVert.txt", "Shaders/TexLightFrag.txt");
	GLuint LightShader = LoadShaders("Shaders/LightVert.txt", "Shaders/LightFrag.txt");
	GLuint vertOutliner = LoadShaders("Shaders/cellVertShader.txt", "Shaders/cellFragShader.txt");

	//Create grid
	Grid* grid =  new Grid(*camera);
	grid->createGridVec(101, 101, defaultShader);

	// load sphere Mesh
	Mesh sphere(*camera);
	sphere.init("Meshes/only_quad_sphere.txt", LightShader, true);

	//load and create the static mesh for the tank
	Mesh tank(*camera);
	tank.init("Meshes/Tank1.FBX", TexLightShader, true, true, "Textures/Tank1DF.png");

	//load and create the static mesh drum mag
	Mesh drumMag(*camera);
	drumMag.init("Meshes/drumMag.obj", TexLightShader, true, true, "Textures/DrumMag_Low_blinn6_BaseColor.png");

	Mesh cube(*camera);
	cube.init("Meshes/SkyBox.obj", TextureShader, false, true, "Textures/SkyBox2.jpg");

	SkyBox skyBoxMesh(*camera);
	skyBoxMesh.init("Meshes/SkyBox.obj", TextureShader, false, true, "Textures/SkyBox2.jpg");
	skyBoxMesh.m_Transform.setWorldScale( vec3(400.0, 400.0, 400.0));
	skyBoxMesh.m_Transform.setWorldLocation(vec3(250, 0.0, 150));

	// init light
	Light light;
	light.init(sphere);
	light.worldTransform.setWorldLocation(vec3(-10.0, 30.0, 10.0));
	light.worldTransform.setWorldScale(vec3(1.0f, 1.0f, 1.0f));
	light.setNoTextureColour(vec4(1.0, 1.0, 1.0, 1.0));
	light.setIsLitt(false);
	light.setProgamToUse(defaultShader);

	vector <WorldObject*> worldObjects;

	// init sphere and set up attributes
	WorldObject* sphereObj = new WorldObject;
	sphereObj->init(sphere);
	sphereObj->worldTransform.setWorldLocation(vec3(4.0f, 10.0f, 1.0f));
	sphereObj->worldTransform.setWorldScale(vec3(3.0f, 3.0f, 3.0f));
	worldObjects.push_back(sphereObj);
	sphereObj->setProgamToUse(LightShader);

	//Unique tank that uses the same mesh but different shader
	WorldObject* newTank = new WorldObject;
	newTank->init(tank);
	newTank->worldTransform.setWorldLocation(vec3(((rand() % 30) - 20), 20, ((rand() % 30) - 20)));
	newTank->addCompoundBody(*physSim);
	newTank->setProgamToUse(defaultShader);
	newTank->setNoTextureColour(vec4(1.0, 0.0, 1.0f, 1.0));
	worldObjects.push_back(newTank);

	for (int i = 0; i < 10; i++)
	{
		WorldObject* newTank = new WorldObject;
		newTank->init(tank);
		newTank->worldTransform.setWorldLocation(vec3(((rand()% 30)-20), 20, ((rand() % 30) -20)));
		newTank->addCompoundBody(*physSim);
		newTank->setProgamToUse(TexLightShader);
		worldObjects.push_back(newTank);
	}

	for (int i = 0; i < 10; i++)
	{
		WorldObject* newDrumMag = new WorldObject;
		newDrumMag->init(drumMag);
		newDrumMag->worldTransform.setWorldLocation(vec3((rand() % 30) - 20, 20, (rand() % 30) - 20));
		newDrumMag->worldTransform.setWorldScale(vec3(5.0, 5.0, 5.0));
		newDrumMag->addCompoundBody(*physSim);
		newDrumMag->setProgamToUse(TexLightShader);
		worldObjects.push_back(newDrumMag);
	}

	GLint textureLocation = glGetUniformLocation(TextureShader, "baseTexture");

	vector <RayCast*> rayCastVec;

	// PostProcessoring SHTUFF
	PostProcessor postProcGrey;
	postProcGrey.init("Shaders/PostProcVert.txt","Shaders/PostProcGreyScaleFrag.txt", SCREEN_WIDTH, SCREEN_HEIGHT);
	
	PostProcessor postProcBlur;
	postProcBlur.init("Shaders/PostProcVert.txt", "Shaders/PostProcBlurFrag.txt", SCREEN_WIDTH, SCREEN_HEIGHT);

	PostProcessor postProcOutline;
	postProcOutline.init("Shaders/PostProcVert.txt", "Shaders/PostProcOutlineFrag.txt", SCREEN_WIDTH, SCREEN_HEIGHT, true);

	PostProcBloom postProcBloom;
	postProcBloom.init("Shaders/PostProcVert.txt", "Shaders/PostProcBloomFragPass1.txt", SCREEN_WIDTH, SCREEN_HEIGHT, true);
	postProcBloom.PostProcBloomInit("Shaders/PostProcVert.txt", "Shaders/PostProcBloomFragPass2.txt", SCREEN_WIDTH, SCREEN_HEIGHT);
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;

	RayCast* newRayCast;

	bool bloom = true;

	int yGrav = -10;

	//set up variables to handle mouse movement
	float itterator = 0;
	//main loop
	while (running)
	{
		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{

			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEMOTION:
				camera->rotate(ev.motion.xrel, ev.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch(ev.button.button)
				{
					case SDL_BUTTON_LEFT:
						newRayCast = new RayCast(*camera, camera->getWorldPos(), camera->forward, 500, defaultShader, vec4(0.7,0.3 , 0.7f,1.0f));
						rayCastVec.push_back(newRayCast);
						break;
					case SDL_BUTTON_RIGHT:
						break;
				}
				break;
				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					//Escape key
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_w:
					camera->move(0.5f);
					break;
				case SDLK_s:
					camera->move(-0.5f);
					break;
				case SDLK_d:
					camera->strafe(-0.5f);
					break;
				case SDLK_a:
					camera->strafe(0.5f);
					break;
				case SDLK_q:
					camera->lift(-0.5);
					break;
				case SDLK_e:
					camera->lift(0.5);
						break;
				case SDLK_b:
					if (bloom)bloom = false;
					else bloom = true;
				case SDLK_0:
					yGrav *= -1;
					physSim->dynamicsWorld->setGravity(btVector3(0.0, yGrav, 0.0));
					break;
				case SDLK_9:
					physSim->dynamicsWorld->clearForces();
					for (int i = 0; i < worldObjects.size(); i++)
					{
						btVector3 foo(0.0f, 10000.0f, 0.0f); 
						btVector3 bar(0.0f, 0.0f, 0.0f);
						if (worldObjects[i]->getRigidBody() != NULL)
						{
							worldObjects[i]->getRigidBody()->applyForce(foo, bar);
						}
						}
					break;
				}
				break;
			}
		}
		physSim->dynamicsWorld->stepSimulation(1 / 60.0f);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		//bind post processor buffer
		if (bloom) postProcBloom.bind1stBuff();

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the mesh into the stencil buffer.
		light.moveCircle();
		light.draw();

		// draw world objects
		for (int i = 0; i < worldObjects.size(); i++)
		{
			worldObjects[i]->draw(light.worldTransform.getWorldLocation());
		}

		// draw any raycasts
		if (rayCastVec.size() > 0)
		{
			for (int i = 0; i < rayCastVec.size(); i++)
			{
				rayCastVec[i]->draw();
			}
		}

		grid->draw();

		glDisable(GL_CULL_FACE);
		skyBoxMesh.render();

		// post processor draw
		if (bloom)postProcBloom.applyBloom();

		SDL_GL_SwapWindow(window);
		
	}
	delete grid;
	delete camera;
	delete physSim;
	destroyWorldObjects(worldObjects);
	destroyRaycast(rayCastVec);
	glDeleteProgram(defaultShader);
	glDeleteProgram(TexLightShader);
	glDeleteProgram(TextureShader);
	glDeleteProgram(LightShader);
	Close();
	return 0;
}

