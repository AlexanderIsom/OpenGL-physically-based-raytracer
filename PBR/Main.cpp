#include <SDL/SDL.h>
#include <GLEW/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

static int win(0);

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

std::string readFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
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

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	if (!InitGL())
	{
		return -1;
	}

	GLuint triangleVAO = CreateTriangleVAO();

	GLuint shaderProgram = LoadShaders();

	glUseProgram(shaderProgram);

	//create matrixes
	glm::mat4 viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));//build view matrix
	glm::mat4 projectionMatrix = glm::inverse(glm::perspective(30.0f * 3.14159265358979f / 180.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f));//build projection matrix


	//pass them into shader
	GLint view_location = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projection_location = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


	/*gl a = 1.0f;
	glUniform1f(test_posLocation, 1.0f);*/

	unsigned int lastTime = 0, currentTime;
	int frames = 0;

	bool go = true;
	while (go)
	{
		frames++;
		currentTime = SDL_GetTicks();

		if (currentTime >= lastTime + 1000) {
			printf("Fps: %d \n", frames);
			lastTime = currentTime;
			frames = 0;
		}

		SDL_Event incomingEvent;
		while (SDL_PollEvent(&incomingEvent))//manages sdl events, such as key press' or just general sdl stuff like sdl quit
		{

			switch (incomingEvent.type)
			{
			case SDL_QUIT:
				go = false;
				break;
			}
		}		

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);		


		DrawVAOTris(triangleVAO, 6, shaderProgram);
		SDL_GL_SwapWindow(window);
	}

	//clean up
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}