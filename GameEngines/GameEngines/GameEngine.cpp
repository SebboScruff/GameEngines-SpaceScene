#include "GameEngine.h"
#include <iostream>

namespace GE {
	GameEngine::GameEngine() {

	}

	GameEngine::~GameEngine() {

	}

	bool GameEngine::init(bool vsync) {
		// initialise SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Unable to initialise SDL. SDL error: " << SDL_GetError() << std::endl;

			return false;
		}

		// because we're using GLEW 2.1.0, set major to 2 and minor to 1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// create the window
		window = SDL_CreateWindow("SDL OpenGL", 50, 50, 960, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			std::cerr << "Unable to create window. SDL error: " << SDL_GetError() << std::endl;

			return false;
		}

		// create the GL context within the window
		glContext = SDL_GL_CreateContext(window);
		if (glContext == nullptr) {
			std::cerr << "SDL could not create a GL context. SDL error: " << SDL_GetError() << std::endl;

			return false;
		}

		// initialise GLEW
		GLenum status = glewInit();
		if (status != GLEW_OK) {
			std::cerr << "Error initialising GLEW. Error: " << glewGetErrorString(status) << std::endl;

			return false;
		}

		//enable vsync
		if (vsync) {
			if (SDL_GL_SetSwapInterval(1) != 0) {
				std::cerr << "Warning: unable to set Vsync. Error: " << SDL_GetError() << std::endl;

				return false;
			}
		}
		
		
		// create the camera
		cam = new Camera(glm::vec3(0.0f, 7.0f, 35.0f),	//Position (NB Positive Z-axis is out of the screen)
			glm::vec3(0.0f, 0.0f, 0.0f),				//Look at (origin)
			glm::vec3(0.0f, 1.0f, 0.0f),				//Up Direction (positive Y-axis)
			45.0f, 640.0f/480.0f, 0.1f, 100.0f);		//FoV, Aspect Ratio, NCP, FCP

		// DECLARE AND LOAD MODELS
		m1 = new Model();
		bool result1 = m1->loadFromFile("FinalModelsAndTextures/alienRock1.obj");
		if (!result1) { std::cerr << "failed to load model. check your spelling probably" << std::endl; }
		mat1 = new Texture("FinalModelsAndTextures/rockTexture1.jpg");

		m2 = new Model();
		bool result2 = m2->loadFromFile("FinalModelsAndTextures/alienRock2.obj");
		if (!result2) { std::cerr << "failed to load model. check your spelling probably" << std::endl; }
		mat2 = new Texture("FinalModelsAndTextures/rockTexture2.jpg");

		m3 = new Model();
		bool result3 = m3->loadFromFile("FinalModelsAndTextures/alienRock3.obj");
		if (!result3) { std::cerr << "failed to load model. check your spelling probably" << std::endl; }
		mat3 = new Texture("FinalModelsAndTextures/rockTexture3.jpg");

		m4 = new Model();
		bool result4 = m4->loadFromFile("FinalModelsAndTextures/alienRock1.obj");
		if (!result4) { std::cerr << "failed to load model. check your spelling probably" << std::endl; }
		mat4 = new Texture("FinalModelsAndTextures/rockTexture2.jpg");

		m5 = new Model();
		bool result5 = m5->loadFromFile("space_frigate_6.obj");
		if (!result5) { std::cerr << "failed to load model. check your spelling probably" << std::endl; }
		mat5 = new Texture("space_frigate_6_color.jpg");

		// create the modelrenderer objects
		mr1 = new ModelRenderer(m1);
		mr1->init();
		mr1->setMaterial(mat1);

		mr2 = new ModelRenderer(m2);
		mr2->init();
		mr2->setPos(25.0f, 7.0f, 35.0f);
		mr2->setMaterial(mat2);

		mr3 = new ModelRenderer(m3);
		mr3->init();
		mr3->setPos(9.0f, 7.0f, 43.0f);
		mr3->setMaterial(mat3);

		mr4 = new ModelRenderer(m4);
		mr4->init();
		mr4->setPos(40.0f, 6.0f, 24.0f);
		mr4->setRot(180.0f, 0.0f, 0.0f);
		mr4->setMaterial(mat4);

		mr5 = new ModelRenderer(m5);
		mr5->init();
		mr5->setPos(45.0, 9.0f, 20.0f);
		mr5->setRot(0.0f, 45.0f, 0.0f);
		mr5->setScale(0.1f, 0.1f, 0.1f);
		mr5->setMaterial(mat5);

		// the skybox elements are entered in a weird way because the images aren't named correctly - whoops
		// skybox images from https://www.babylonjs-playground.com/#UU7RQ#245
		skybox = new SkyboxRenderer("skyboxImages/space_front.jpg", "skyboxImages/space_back.jpg", 
									"skyboxImages/space_left.jpg", "skyboxImages/space_right.jpg",
									"skyboxImages/space_up.jpg", "skyboxImages/space_down.jpg");

		terrainTexture = new Texture("terrain-texture.png");
		terrain = new Terrain("terrain-heightmap.png", terrainTexture);

		return true;
	}

	bool GameEngine::keepRunning() {
		// keep the event queue up to date
		SDL_PumpEvents();

		// set up a variable to check if the window has been closed
		SDL_Event evt;

		if (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT)) {
			return false;
		}

		return true;
	}

	void GameEngine::processInput() {
		// CAMERA ROTATION
		// just static numbers in here for now, TODO make these variable
		const float camSpeed = 0.2;
		const float mouseSens = 0.1f;
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);

		float diffX = mouse_x - cam->getOldMouseX();
		float diffY = cam->getOldMouseY() - mouse_y;

		cam->setYaw((cam->getYaw() + diffX) * mouseSens);
		cam->setPitch((cam->getPitch() + diffY) * mouseSens);

		// now we can change the camera with these values that we've set:
		glm::vec3 direction;
		direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		direction.y = sin(glm::radians(cam->getPitch()));
		direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		cam->setTarget(glm::normalize(direction));

		// CAMERA TRANSLATION
		bool keyStates[4] = { 0, 0, 0, 0 };
		int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3;
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			// detect key down events, triggering the corresponding bools
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					keyStates[UP] = true;
					break;
				case SDL_SCANCODE_DOWN:
					keyStates[DOWN] = true;
					break;
				case SDL_SCANCODE_LEFT:
					keyStates[LEFT] = true;
					break;
				case SDL_SCANCODE_RIGHT:
					keyStates[RIGHT] = true;
					break;
				default:
					break;
				}
			}
			// likewise for key up events, turn off the bools
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					keyStates[UP] = false;
					break;
				case SDL_SCANCODE_DOWN:
					keyStates[DOWN] = false;
					break;
				case SDL_SCANCODE_LEFT:
					keyStates[LEFT] = false;
					break;
				case SDL_SCANCODE_RIGHT:
					keyStates[RIGHT] = false;
					break;
				default:
					break;
				}
			}
		}

		// resolve camera location based on the key events
		if (keyStates[UP]) { cam->SetPos(cam->getPos() + cam->getTarget() * camSpeed); }
		if (keyStates[DOWN]) { cam->SetPos(cam->getPos() - cam->getTarget() * camSpeed); }
		if (keyStates[LEFT]) { cam->SetPos(cam->getPos() - glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed); }
		if (keyStates[RIGHT]) { cam->SetPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed); }

		cam->updateCamMatrices();
		
		//set old mouse x/y to be half the width/height of the window
		cam->setOldMouseX(960 / 2);
		cam->setOldMouseY(720 / 2);

	}

	void VerticalHover(ModelRenderer* model, int minHeight, int maxHeight, float speed) {
		if (model->getPosY() >= maxHeight) { model->goUp = false; }
		if (model->getPosY() <= minHeight) { model->goUp = true; }

		if(model->goUp)
			model->setPos(model->getPosX(), model->getPosY() + speed, model->getPosZ()); // upward vertical movement
		else
			model->setPos(model->getPosX(), model->getPosY() - speed, model->getPosZ()); // downward vertical movement
	}

	void GameEngine::update() {
		mr5->setPos(mr5->getPosX() - 0.0005f, mr5->getPosY(), mr5->getPosZ() + 0.0005); // move the ship along the X axis for now as a decoration
		
		VerticalHover(mr2, 3, 11, 0.0005);
		VerticalHover(mr3, 3, 11, 0.00069);
		VerticalHover(mr4, 3, 11, 0.001);
	}

	void GameEngine::draw() {
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		// enable depth buffering so the textured models dont look all strange
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->draw(cam);

		mr1->draw(cam);
		mr1->drawTerrain(terrain);

		mr2->draw(cam);
		mr3->draw(cam);
		mr4->draw(cam);
		mr5->draw(cam);
		/*glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(-1.0f, 0.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(0.0f, 1.0f);
		glEnd();*/

		SDL_GL_SwapWindow(window);
	}

	void GameEngine::shutdown() {
		mr1->destroy();
		mr2->destroy();
		mr3->destroy();
		mr4->destroy();
		mr5->destroy();
		skybox->destroy();
		delete mr1;
		delete m1;
		delete mr2;
		delete m2;
		delete mr3;
		delete m3;
		delete mr4;
		delete m4;
		delete mr5;
		delete m5;
		delete cam;
		delete skybox;


		SDL_DestroyWindow(window);

		window = nullptr;

		SDL_Quit();
	}

	void GameEngine::setWindowTitle(const char* newTitle) {
		SDL_SetWindowTitle(window, newTitle);
	}

	void displayInfoMessage(const char* msg) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Basic Game Engine", msg, nullptr);
	}
}