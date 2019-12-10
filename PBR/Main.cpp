#include <SDL/SDL.h>
#include <GLEW/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static int win(0);
std::vector<std::string > textures;
bool InitGL()
{
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: GLEW failed to initialise with message: " << glewGetErrorString(err) << std::endl;
		return false;
	}
	std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return true;
}

GLuint CreateTriangleVAO()//tell opengl what verties to draw
{
	GLuint VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	float vertices[] = {
		 -1.0f, -1.0f,
		  1.0f, -1.0f,
		 -1.0f,  1.0f,

		  1.0f, -1.0f,
		  1.0f, 1.0f,
		 -1.0f,  1.0f
	};
	GLuint buffer = 0;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);

	return VAO;
}

void DrawVAOTris(GLuint VAO, int numVertices, GLuint shaderProgram)//tell opengl to actually draw it to the screen
{
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glBindVertexArray(0);

	glUseProgram(0);
}

bool CheckShaderCompiled(GLint shader) //check the shader compiled correctly
{
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetShaderInfoLog(shader, len, &len, log);
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete[] log;

		return false;
	}
	return true;
}

std::string readFile(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}


GLuint LoadShaders()//colour / shade the object
{
	//load in shaders from file
	std::string vertShaderStr = readFile("../Shaders/vShader.glsl");
	std::string fragShaderStr = readFile("../Shaders/fShader.glsl");

	const GLchar* vShaderText = vertShaderStr.c_str();
	const GLchar* fShaderText = fragShaderStr.c_str();


	GLuint program = glCreateProgram();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vShader, 1, &vShaderText, NULL);

	glCompileShader(vShader);

	if (!CheckShaderCompiled(vShader))
	{
		return 0;
	}

	glAttachShader(program, vShader);

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderText, NULL);


	glCompileShader(fShader);
	if (!CheckShaderCompiled(fShader))
	{
		return 0;
	}
	glAttachShader(program, fShader);

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(program, len, &len, log);
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete[] log;

		return 0;
	}

	return program;
}

std::vector<SDL_Keycode> Keys;
glm::mat4 viewMatrix;
glm::vec3 light_pos;
float brightness = 1.0f;
glm::mat4 projectionMatrix;
GLuint shaderProgram;

void moveMouse(glm::vec2 pos)
{
	viewMatrix = glm::rotate(viewMatrix, glm::radians(pos.x * 0.05f), glm::vec3(0.0, -1.0, 0.0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(pos.y * 0.05f), glm::vec3(-1.0, 0.0, 0.0));
}

bool keyDown(SDL_Keycode key)
{
	for (auto it = Keys.begin(); it != Keys.end();)
	{
		if (*it == key)
		{
			return true;
		}
		else
		{
			it++;
		}
	}
	return false;
}

void inputHandeler(float timestep)
{
	glUseProgram(shaderProgram);

	if (keyDown(SDLK_w)) // move forward
	{
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, 0.0, -1.0 * timestep));
	}
	if (keyDown(SDLK_a))
	{ // move left
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-1.0 * timestep, 0.0, 0.0));
	}
	if (keyDown(SDLK_s))
	{ // move back
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, 0.0, 1.0 * timestep));
	}
	if (keyDown(SDLK_d))
	{ // move right
		viewMatrix = glm::translate(viewMatrix, glm::vec3(1.0 * timestep, 0.0, 0.0));
	}
	if (keyDown(SDLK_q))
	{ // rotate
		viewMatrix = glm::rotate(viewMatrix, glm::radians(100.0f * timestep), glm::vec3(0.0, 0.0, 1.0));
	}
	if (keyDown(SDLK_e))
	{ // rotate
		viewMatrix = glm::rotate(viewMatrix, glm::radians(-100.0f * timestep), glm::vec3(0.0, 0.0, 1.0));
	}
	if (keyDown(SDLK_r))
	{ // move up
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, 1.0 * timestep, 0.0));
	}
	if (keyDown(SDLK_f))
	{ // move down
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, -1.0 * timestep, 0.0));
	}

	if (keyDown(SDLK_UP))
	{ // move light up
		light_pos += glm::vec3(0.0, 0.0, -1.0 * timestep);
	}

	if (keyDown(SDLK_DOWN))
	{ // move light down
		light_pos += glm::vec3(0.0, 0.0, 1.0 * timestep);
	}

	if (keyDown(SDLK_LEFT))
	{ // move light left
		light_pos += glm::vec3(-1.0 * timestep, 0.0, 0.0);
	}

	if (keyDown(SDLK_RIGHT))
	{ // move light right
		light_pos += glm::vec3(1.0 * timestep, 0.0, 0.0);
	}
	if (keyDown(SDLK_KP_ENTER))
	{ // move light right
		brightness += 1.0 * timestep;
	}
	if (keyDown(SDLK_KP_PERIOD))
	{ // move light right
		brightness -= 1.0 * timestep;
	}


	GLint view_location = glGetUniformLocation(shaderProgram, "inverseViewMatrix");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint light_location = glGetUniformLocation(shaderProgram, "light_pos");
	glUniform3f(light_location, light_pos.x, light_pos.y, light_pos.z);

	if (brightness < 0) brightness = 0;

	GLint lightBrightness_location = glGetUniformLocation(shaderProgram, "light_brightness");
	glUniform1f(lightBrightness_location, brightness);
}

void loadTexture()
{
	const int size = 10;
	glUseProgram(shaderProgram);
	GLint albedoSamples[size];
	//load albedo textures
	for (int i = 0; i < textures.size(); i++)
	{
		albedoSamples[i] = i;
		unsigned int texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		std::string path = "../Materials/textures/" + textures[i] + "/" + textures[i] + "_albedo.jpg";
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "failed to load texture\n";
		}
		stbi_image_free(data);

	}


	GLuint albedo = glGetUniformLocation(shaderProgram, "u_albedo");

	glUniform1iv(albedo, textures.size(), albedoSamples);



	//load roughness textures
	GLint roughnessSamples[size];
	for (int i = 0; i < textures.size(); i++)
	{
		roughnessSamples[i] = i + textures.size();
		unsigned int texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0 + i + textures.size());
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		std::string path = "../Materials/textures/" + textures[i] + "/" + textures[i] + "_roughness.jpg";
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "failed to load texture\n";
		}
		stbi_image_free(data);
	}

	GLuint roughness = glGetUniformLocation(shaderProgram, "u_roughness");

	glUniform1iv(roughness, textures.size(), roughnessSamples);


	//load roughness textures
	GLint metalicSamples[size];
	for (int i = 0; i < textures.size(); i++)
	{
		metalicSamples[i] = i + (2 * textures.size());
		unsigned int texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0 + i + (2 * textures.size()));
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		std::string path = "../Materials/textures/" + textures[i] + "/" + textures[i] + "_metalic.jpg";
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "failed to load texture\n";
		}
		stbi_image_free(data);

	}

	GLuint metalic = glGetUniformLocation(shaderProgram, "u_metalic");

	glUniform1iv(metalic, textures.size(), metalicSamples);

	return;
}


int main(int argc, char* args[])
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Whoops! Something went very wrong, cannot initialise SDL :(" << std::endl;
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);//setting open gl version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	int windowPosX = 100;
	int windowPosY = 100;
	int windowWidth = 640;
	int windowHeight = 480;


	//using sdl to prep a window then launching opengl within it
	SDL_Window* window = SDL_CreateWindow("OpenGL", windowPosX, windowPosY, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	if (!InitGL())
	{
		return -1;
	}

	GLuint triangleVAO = CreateTriangleVAO();

	shaderProgram = LoadShaders();

	glUseProgram(shaderProgram);

	//create matrixes
	viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));//build view matrix
	projectionMatrix = glm::inverse(glm::perspective(glm::radians(30.0f), (float)windowWidth / (float)windowHeight, 0.001f, 100.0f));//build projection matrix


	//pass them into shader
	GLint view_location = glGetUniformLocation(shaderProgram, "inverseViewMatrix");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projection_location = glGetUniformLocation(shaderProgram, "inverseProjectionMatrix");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//load textures
	textures.push_back("iron");
	textures.push_back("brick");

	loadTexture();

	float lastTime, currentTime;
	int frames = 0;
	lastTime = 0;

	float startTime = SDL_GetTicks();
	float timeStep;

	SDL_RaiseWindow(window);

	bool go = true;
	while (go)
	{
		frames++;
		currentTime = SDL_GetTicks();

		if (currentTime >= lastTime + 1000)
		{
			printf("Fps: %d \n", frames);
			lastTime = currentTime;
			frames = 0;
		}

		//do movement and pass it into the shader again

		SDL_Event event;
		while (SDL_PollEvent(&event))//manages sdl events, such as key press' or just general sdl stuff like sdl quit
		{
			switch (event.type)
			{
			case SDL_QUIT:
				go = false;
				break;
			case SDL_KEYDOWN:
				for (auto it = Keys.begin(); it != Keys.end();)
				{
					if (*it == event.key.keysym.sym)
					{
						break;
					}
					else
					{
						it++;
					}
				}
				Keys.push_back(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				//Keys.erase(std::find(Keys.begin(), Keys.end(), event.key.keysym.sym));
				for (auto it = Keys.begin(); it != Keys.end();)
				{
					if (*it == event.key.keysym.sym)
					{
						it = Keys.erase(it);
					}
					else
					{
						it++;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				moveMouse(glm::vec2(event.motion.xrel, event.motion.yrel));
				break;
			}
			if (keyDown(SDLK_ESCAPE))
			{
				go = false;
			}
		}

		timeStep = ((float)SDL_GetTicks() - startTime) / 1000.0f;
		inputHandeler(timeStep);
		startTime = SDL_GetTicks();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		DrawVAOTris(triangleVAO, 6, shaderProgram);
		SDL_GL_SwapWindow(window);
	}

	//clean up
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	//store average fps

	return 0;
}