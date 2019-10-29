//#include <SDL/SDL.h>
//#include <GLEW/glew.h>
//#include <SDL/SDL_opengl.h>
//#include <iostream>
//
//const GLint SCREEN_WIDTH = 640;
//const GLint SCREEN_HEIGHT = 480;
//
//static int win(0);
//
//
//int main(int argc, char* args[]) {
//
//	SDL_Init(SDL_INIT_EVERYTHING);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//
//
//	SDL_Window* window = SDL_CreateWindow("OpenGL", 200, 300, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
//
//	SDL_GLContext context = SDL_GL_CreateContext(window);
//
//	glewExperimental = GL_TRUE;
//
//	if (GLEW_OK != glewInit()) {
//		std::cout << "glew failed to initalize\n";
//
//		return EXIT_FAILURE;
//	}
//
//	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//
//	SDL_Event windowEvent;
//
//	while (true) {
//		if (SDL_PollEvent(&windowEvent)) {
//			if (SDL_QUIT == windowEvent.type) {
//				break;
//			}
//		}
//
//		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//
//		glBegin(GL_POINTS);
//		for (int i = 0; i < 100; i++)
//		{
//			glColor3f(1.0, 1.0, 1.0);
//			glVertex2i(i, i);
//		}
//		glEnd();
//
//
//
//		SDL_GL_SwapWindow(window);
//	}
//
//	SDL_GL_DeleteContext(context);
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//
//	return EXIT_SUCCESS;
//}




//-----------------------------





//#include <stdio.h>
//#include <GL/glut.h>
//#include <math.h>
//#include <thread>
//
//static int win(0);
//
//const GLuint Width = 620;
//const GLuint Height = 480;
//
//void draw() {
//	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	//int i = 0;
//	glBegin(GL_POINTS);
//	glColor3f(1, 1, 1);
//	//glVertex2i(i, Height / 2 + sin(i * 50) * 100);
//	glVertex2i(100, 100);
//	glEnd();
//	//Sleep(50);
//	//i++;
//
//}
//
//int main(int argc, char* argv[]) {
//	glutInit(&argc, argv);
//
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowPosition(80, 80);
//	glutInitWindowSize(Width, Height);
//	glutCreateWindow("Test");
//
//
//	glutDisplayFunc(draw);
//
//	//std::thread first(draw, Height / 4);
//	//std::thread second(draw, Height - Height / 1);
//
//	//first.join();
//	//second.join();
//
//	system("Pause");
//}