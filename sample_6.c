#define _CRT_SECURE_NO_WARNINGS

#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;

GLint width, height;
GLuint shaderID;
GLuint vertexshader;
GLuint fragmentShader;
GLuint VAO, VBO;
GLuint s_program;

GLfloat ox, oy;

bool left_button = false;

GLfloat triShape[3][3] = {
	{-0.5,-0.5,0.0} , {0.5, -0.5, 0.0}, {0.0,0.5,0.0}};

/*float vPositionList[] = {
	0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	-0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f
};

unsigned int index[] = {
	0,1,3,
	1,2,3
};*/

GLfloat colors[3][3] = {
	{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0} };

struct Tri {
	GLfloat triShape[3][3];
};

struct Tricolor {
	GLfloat colors[3][3];
};

struct Tri tri[4];
struct Tri tri2[4];
struct Tricolor color[4];

GLuint vao, vbo[2];
GLuint VAO, VBO_pos, EBO;

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
void InitShader();
char* filetobuf(const char* file);
void Use();
void checkCompileErrors(GLuint shader, char* type);
void InitTri();
void InitColor();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
void DtoOD(int x, int y, float* ox, float* oy);


void main(int argc, char** argv)
{
	width = 500;
	height = 500;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	glewInit();

	InitTri();
	InitColor();
	InitShader();
	InitBuffer();
	
	glutDisplayFunc(drawScene);        //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

void make_vertexShaders()
{
	vertexSource = filetobuf ("vertex.glsl");

	vertexshader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexshader, 1, (const GLchar**) & vertexSource, 0);

	glCompileShader(vertexShader);


	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		printf("ERROR: vertex shader 컴파일 실패 %s\n", errorLog);
		return false;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	glCompileShader (fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		printf("ERROR: fragment shader 컴파일 실패 %s\n", errorLog);
		return false;
	}
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), tri, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), color, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(1);


	/*glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);*/
}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShaders();

	s_program = glCreateProgram();

	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	checkCompileErrors(s_program, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//glUseProgram(s_program);
	Use();
}

GLvoid drawScene()                                                             //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 
	//glUseProgram(s_program);
	//glBindVertexArray(VAO);
	Use();
	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)                            //--- 콜백  함수: 다시  그리기  콜백  함수
{
	glViewport(0, 0, w, h);
}

void Use()
{
	glUseProgram(s_program);
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, 1, length, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

void checkCompileErrors(GLuint shader, char* type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, GL_COMPILE_STATUS, &success);
			printf("ERROR: SHADER_COMPILATION_ERROR %s %s\n", type, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("ERROR: PROGRAM_LINKING_ERROR %s %s\n", type, infoLog);
		}
	}
}

void InitTri()
{
	tri[0].triShape[0][0] = -0.8;
	tri[0].triShape[0][1] = -0.8;
	tri[0].triShape[0][2] = 0.0;
	tri[0].triShape[1][0] = -0.5;
	tri[0].triShape[1][1] = -0.8;
	tri[0].triShape[1][2] = 0.0;
	tri[0].triShape[2][0] = -0.65;
	tri[0].triShape[2][1] = -0.3;
	tri[0].triShape[2][2] = 0.0;

	tri2[0].triShape[0][0] = 0.5;
	tri2[0].triShape[0][1] = -0.8;
	tri2[0].triShape[0][2] = 0.0;
	tri2[0].triShape[1][0] = 0.8;
	tri2[0].triShape[1][1] = -0.8;
	tri2[0].triShape[1][2] = 0.0;
	tri2[0].triShape[2][0] = 0.65;
	tri2[0].triShape[2][1] = -0.3;
	tri2[0].triShape[2][2] = 0.0;

	tri[1].triShape[0][0] = 0.5;
	tri[1].triShape[0][1] = -0.8;
	tri[1].triShape[0][2] = 0.0;
	tri[1].triShape[1][0] = 0.8;
	tri[1].triShape[1][1] = -0.8;
	tri[1].triShape[1][2] = 0.0;
	tri[1].triShape[2][0] = 0.65;
	tri[1].triShape[2][1] = -0.3;
	tri[1].triShape[2][2] = 0.0;

	tri[2].triShape[0][0] = 0.8;
	tri[2].triShape[0][1] = 0.8;
	tri[2].triShape[0][2] = 0.0;
	tri[2].triShape[1][0] = 0.5;
	tri[2].triShape[1][1] = 0.8;
	tri[2].triShape[1][2] = 0.0;
	tri[2].triShape[2][0] = 0.65;
	tri[2].triShape[2][1] = 0.3;
	tri[2].triShape[2][2] = 0.0;

	tri[3].triShape[0][0] = -0.8;
	tri[3].triShape[0][1] = 0.8;
	tri[3].triShape[0][2] = 0.0;
	tri[3].triShape[1][0] = -0.5;
	tri[3].triShape[1][1] = 0.8;
	tri[3].triShape[1][2] = 0.0;
	tri[3].triShape[2][0] = -0.65;
	tri[3].triShape[2][1] = 0.3;
	tri[3].triShape[2][2] = 0.0;
}

void InitColor()
{
	color[0].colors[0][0] = 1.0f;
	color[0].colors[0][1] = 0.0f;
	color[0].colors[0][2] = 0.0f;

	color[0].colors[1][0] = 0.0f;
	color[0].colors[1][1] = 0.0f;
	color[0].colors[1][2] = 0.0f;

	color[0].colors[2][0] = 0.0f;
	color[0].colors[2][1] = 0.0f;
	color[0].colors[2][2] = 0.0f;

	color[1].colors[0][0] = 0.0f;
	color[1].colors[0][1] = 0.0f;
	color[1].colors[0][2] = 0.0f;

	color[1].colors[1][0] = 0.0f;
	color[1].colors[1][1] = 1.0f;
	color[1].colors[1][2] = 0.0f;

	color[1].colors[2][0] = 0.0f;
	color[1].colors[2][1] = 0.0f;
	color[1].colors[2][2] = 0.0f;

	color[2].colors[0][0] = 0.0f;
	color[2].colors[0][1] = 0.0f;
	color[2].colors[0][2] = 0.0f;

	color[2].colors[1][0] = 0.0f;
	color[2].colors[1][1] = 0.0f;
	color[2].colors[1][2] = 0.0f;

	color[2].colors[2][0] = 0.0f;
	color[2].colors[2][1] = 0.0f;
	color[2].colors[2][2] = 1.0f;

	color[3].colors[0][0] = 0.5f;
	color[3].colors[0][1] = 0.0f;
	color[3].colors[0][2] = 0.0f;

	color[3].colors[1][0] = 0.0f;
	color[3].colors[1][1] = 0.5f;
	color[3].colors[1][2] = 0.0f;

	color[3].colors[2][0] = 0.0f;
	color[3].colors[2][1] = 0.0f;
	color[3].colors[2][2] = 0.5f;

}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case'a':
	case'A':
		break;
	case 'b':
	case 'B':
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		left_button = true;
}

GLvoid Motion(int x, int y)
{
	DtoOD(x, y, &ox, &oy); // x, y좌표를 OpenGL 좌표로 변경
	if (left_button == true)
	{

	}
}

void DtoOD(int x, int y, float* ox, float* oy)
{
	int w = width;
	int h = height;
	*ox = (float)((x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)));
	*oy = -(float)((y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0)));
}