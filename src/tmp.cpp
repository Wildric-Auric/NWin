
#include <windows.h>
#include <WinUser.h>
#include <Uxtheme.h>

#include <GL/glew.h>
#include <iostream>
#include <string>

// Paint the title on the custom frame.










//-----------------------


void PrintOpenGLErrors(char const* const Function, char const* const File, int const Line)
{
	bool Succeeded = true;

	GLenum Error = glGetError();
	if (Error != GL_NO_ERROR)
	{
		char const* ErrorString = (char const*)gluErrorString(Error);
		if (ErrorString)
			std::cerr << ("OpenGL Error in %s at line %d calling function %s: '%s'", File, Line, Function, ErrorString) << std::endl;
		else
			std::cerr << ("OpenGL Error in %s at line %d calling function %s: '%d 0x%X'", File, Line, Function, Error, Error) << std::endl;
	}
}

#ifdef _DEBUG
#define CheckedGLCall(x) do { PrintOpenGLErrors(">>BEFORE<< "#x, __FILE__, __LINE__); (x); PrintOpenGLErrors(#x, __FILE__, __LINE__); } while (0)
#define CheckedGLResult(x) (x); PrintOpenGLErrors(#x, __FILE__, __LINE__);
#define CheckExistingErrors(x) PrintOpenGLErrors(">>BEFORE<< "#x, __FILE__, __LINE__);
#else
#define CheckedGLCall(x) (x)
#define CheckedGLResult(x) (x);
#define CheckExistingErrors(x)
#endif


void PrintShaderInfoLog(GLint const Shader)
{
	int InfoLogLength = 0;
	int CharsWritten = 0;

	glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		GLchar* InfoLog = new GLchar[InfoLogLength];
		glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
		std::cout << "Shader Info Log:" << std::endl << InfoLog << std::endl;
		delete[] InfoLog;
	}
}

int init()
{

	char const* VertexShaderSource = R"GLSL(
		#version 330
		in vec2 position;
		void main()
		{
			gl_Position = vec4(position, 0.0, 1.0);
		}
	)GLSL";

	char const* FragmentShaderSource = R"GLSL(
		#version 330
		out vec4 outColor;
		void main()
		{
			outColor = vec4(1.0, 0.0,0.0, 1.0);
		}
	)GLSL";

	GLfloat const Vertices[] = {
		-1.0f, -1.0f,
		0.0f, 1.0f,
		1.f, -1.0f
	};

	GLuint const Elements[] = {
		0, 1, 2
	};

	GLuint VAO;
	CheckedGLCall(glGenVertexArrays(1, &VAO));
	CheckedGLCall(glBindVertexArray(VAO));

	GLuint VBO;
	CheckedGLCall(glGenBuffers(1, &VBO));
	CheckedGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	CheckedGLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW));
	CheckedGLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	GLuint EBO;
	CheckedGLCall(glGenBuffers(1, &EBO));
	CheckedGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	CheckedGLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), Elements, GL_STATIC_DRAW));

	GLint Compiled;
	GLuint VertexShader = CheckedGLResult(glCreateShader(GL_VERTEX_SHADER));
	CheckedGLCall(glShaderSource(VertexShader, 1, &VertexShaderSource, NULL));
	CheckedGLCall(glCompileShader(VertexShader));
	CheckedGLCall(glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Compiled));
	if (!Compiled)
	{
		std::cerr << "Failed to compile vertex shader!" << std::endl;
		PrintShaderInfoLog(VertexShader);
	}

	GLuint FragmentShader = CheckedGLResult(glCreateShader(GL_FRAGMENT_SHADER));
	CheckedGLCall(glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL));
	CheckedGLCall(glCompileShader(FragmentShader));
	CheckedGLCall(glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Compiled));
	if (!Compiled)
	{
		std::cerr << "Failed to compile fragment shader!" << std::endl;
		PrintShaderInfoLog(FragmentShader);
	}

	GLuint ShaderProgram = CheckedGLResult(glCreateProgram());
	CheckedGLCall(glAttachShader(ShaderProgram, VertexShader));
	CheckedGLCall(glAttachShader(ShaderProgram, FragmentShader));
	CheckedGLCall(glBindFragDataLocation(ShaderProgram, 0, "outColor"));
	CheckedGLCall(glLinkProgram(ShaderProgram));
	CheckedGLCall(glUseProgram(ShaderProgram));

	GLint PositionAttribute = CheckedGLResult(glGetAttribLocation(ShaderProgram, "position"));
	CheckedGLCall(glEnableVertexAttribArray(PositionAttribute));

	CheckedGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	CheckedGLCall(glVertexAttribPointer(PositionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0));
	CheckedGLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	return 0;
}

void draw(int sizeX, int sizeY) {
	glViewport(0,0,sizeX,sizeY);
	glClearColor(0.05, 0.05, 0.1, 1);
	CheckedGLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	CheckedGLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));
	//glFlush(); // ??? apparently I need to call wglSwapBuffers instead of swap buffers; otherwise we glflush in order for swap to front buffer
}