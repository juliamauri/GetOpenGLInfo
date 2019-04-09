////////////////////////////////////////////////////////////////////////////////////////
//	MIT License
//	
//	Copyright(c) 2019 Julià Mauri Costa
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
////////////////////////////////////////////////////////////////////////////////////////
// Info how to create a window and get context for OpenGL with Windows.
//	window: https://www.youtube.com/watch?v=nQTiSLiNyk4
//	context: http://www.cplusplus.com/forum/beginner/112104/
////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>

#include "libs/glew/include/GL/glew.h"
#include <gl/GLU.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment(lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment(lib, "libs\\glew\\libx86\\glew32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define BUFFER_OUTPUT 1048576
char buffer[BUFFER_OUTPUT];
FILE* output = nullptr;
char* cursor = buffer;

void AddStr(const char* copy);
void GetShaderStorageBuffwerObjectInfo();

int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow)
{
	const auto pClassName = "JMC";
	// register window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = pClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClassEx(&wc);
	// create window instance
	HWND hWnd = CreateWindowEx(NULL,                            // extended style
		pClassName,                       // class name
		"GetOpenGLInfo",				   // app name
		WS_OVERLAPPEDWINDOW |            // window style
		WS_VISIBLE |
		WS_SYSMENU | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		100, 100,                        // x,y coordinate
		400, 400,                        // width, height
		NULL,                            // handle to parent
		NULL,                            // handle to menu
		hInstance,                       // application instance
		NULL);

	// pixel format setup
	int nPixelFormat;						  //your pixel format index
	PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),		  //size of the structure	
			1,									  //version, always set to 1,
			PFD_DRAW_TO_WINDOW |				  //support window
			PFD_SUPPORT_OPENGL |				  //support OpenGl
			PFD_DOUBLEBUFFER,					  //support double buffering  
			PFD_TYPE_RGBA,						  //RGBA color mode
			32,									  //go for 32bit color mode		
			0, 0, 0, 0, 0, 0,					  //ignore color bits, not used
			0,									  //no alpha buffer	
			0,									  //ignore shift bit
			0,									  //no accumulation buffer 
			0, 0, 0, 0,							  //ignore accumulation bits
			16,									  //16-bits z-buffer size
			0,									  //no stencil buffer
			0,									  //no auxiliary buffer 	
			PFD_MAIN_PLANE,						  //main drawing plane
			0,									  //reserved 	
			0, 0, 0 };

	//for GL context
	static HGLRC hRC;//rendering contex
	static HDC hDC = GetDC(hWnd);// device context
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);

	//Crating glcontext
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	glewInit();

	//Filling file
	const auto OutputFile = "GetOpenGLInfo.txt";
	output = _fsopen(OutputFile, "w", _SH_DENYNO);

	AddStr("----------General info----------");
	AddStr("");

	AddStr("Vendor:");
	char *GL_vendor = (char *)glGetString(GL_VENDOR);
	AddStr(GL_vendor);
	AddStr("");

	AddStr("Graphic card:");
	char *GL_renderer = (char *)glGetString(GL_RENDERER);
	AddStr(GL_renderer);
	AddStr("");

	AddStr("Driver version:");
	char *GL_version = (char *)glGetString(GL_VERSION);
	AddStr(GL_version);
	AddStr("");

	AddStr("Shading language version:");
	char *GL_shading = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	AddStr(GL_shading);
	AddStr("");

	AddStr("------Supported extensions------");
	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i = 0; i < n; i++) {

		const auto extension = (char *)glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(extension, "GL_ARB_shader_storage_buffer_object") == 0)
			GetShaderStorageBuffwerObjectInfo();
		else
			AddStr(extension);
	}
	AddStr("");

	fclose(output);

	//delete gl
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);

	return 0;
}

void AddStr(const char * copy)
{
	char* cInit = cursor;
	size_t bytes = strlen(copy);
	memcpy(cursor, copy, bytes);
	cursor += bytes;
	memcpy(cursor, "\n", 2);
	cursor += 2;
	fputs(cInit, output);
}

void GetShaderStorageBuffwerObjectInfo()
{
	AddStr("");
	int i;
	char numTemp[320];
	std::string numToChar;
	AddStr("GL_ARB_shader_storage_buffer_object:");
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &i);
	numToChar = "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &i);
	numToChar = "GL_MAX_SHADER_STORAGE_BLOCK_SIZE ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &i);
	numToChar = "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS ";
	sprintf_s(numTemp, "%d", i);
	numToChar += numTemp;
	AddStr(numToChar.data());

	AddStr("");

}
