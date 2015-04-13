// Stenciltest.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include "../GLloadgen/gl_core_3_3.h"
#include "GL/freeglut_std.h"
#include "GL/freeglut_ext.h"

#include <iostream>

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

GLuint fbo;
GLuint textures[2];
unsigned char testtex[500 * 500 * 4] {};



extern "C"{
	void DebugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message, const void *userParam) {

		char debSource[16], debType[20], debSev[7];
		if (source == GL_DEBUG_SOURCE_API_ARB)
			strcpy_s(debSource, 16, "OpenGL");
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
			strcpy_s(debSource, 16, "Windows");
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
			strcpy_s(debSource, 16, "Shader Compiler");
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
			strcpy_s(debSource, 16, "Third Party");
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
			strcpy_s(debSource, 16, "Application");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
			strcpy_s(debSource, 16, "Other");

		if (type == GL_DEBUG_TYPE_ERROR_ARB)
			strcpy_s(debType, 20, "Error");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
			strcpy_s(debType, 20, "Deprecated behavior");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
			strcpy_s(debType, 20, "Undefined behavior");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
			strcpy_s(debType, 20, "Portability");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
			strcpy_s(debType, 20, "Performance");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)
			strcpy_s(debType, 20, "Other");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
			strcpy_s(debSev, 7, "High");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
			strcpy_s(debSev, 7, "Medium");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
			strcpy_s(debSev, 7, "Low");
		else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		{
			strcpy_s(debSev, 7, "Notice");
			return;
		}
		printf("Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n",
			debSource, debType, id, debSev, message);


	}
}

void init()
{
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(DebugCallbackARB,NULL);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glGenTextures(2, textures);
	
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 500, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 500, 500, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, testtex);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textures[1], 0);

	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer incomplete" << std::endl;
	}
}

void readback_buffer()
{
	printf("first few bytes before readback: 0x%.8X 0x%.8X 0x%.8X \n", *((unsigned int *) &(testtex[0])), *((unsigned int *) &(testtex[4])), *((unsigned int *) &(testtex[8])));
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, (void*) testtex);
	glBindTexture(GL_TEXTURE_2D, 0);
	printf("first few bytes after readback: 0x%.8X 0x%.8X 0x%.8X\n", *((unsigned int *) &(testtex[0])), *((unsigned int *) &(testtex[4])), *((unsigned int *) &(testtex[8])));
}

void writeback_buffer()
{
	printf("first few bytes before writeback: 0x%.8X 0x%.8X 0x%.8X\n", *((unsigned int *) &(testtex[0])), *((unsigned int *) &(testtex[4])), *((unsigned int *) &(testtex[8])));
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 500, 500, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, (void *)testtex);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw()
{
	static float r = 0.5f;
	static int counter = 0;
	r += 0.01f;
	if (r >= 1.0f) r -= 1.0f;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glViewport(0, 0, 500, 500);
	glClearColor(r, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glFlush();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	glBlitFramebuffer(0, 0, 500, 500, 0, 0, 500, 500, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	glutSwapBuffers();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	std::cout << "drew frame"  << counter++ << std::endl;
	readback_buffer();
	writeback_buffer();
	readback_buffer();

	//glutPostRedisplay();
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	/* add command line argument "classic" for a pre-3.x context */
	if ((argc != 2) || (strcmp(argv[1], "classic") != 0)) {
		glutInitContextVersion(3, 3);
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	}
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	int windowID = glutCreateWindow("Name");
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glutDestroyWindow(windowID);
		return 1;
	}

	init();
	readback_buffer();
	writeback_buffer();
	readback_buffer();
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

