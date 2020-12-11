#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string.h>
#include "ObjParser.h"
#include "bmpfuncs.h"
#include "environment.h"
#include <iostream>
#include "setting_header.h"
using namespace std;

struct ab {
	float a, b;
};
float g_pPosition[6] = { -1.0f,-4.0f,-0.5f,-3.0f,0.0f,-2.0f };
environment env;

#define M_PI 3.1415926535897
#define DIST 10
using std::cos;
using std::sin;
using std::sqrt;

// global variable for counting fps
//volatile int frame = 0, time, timebase = 0;
volatile float fps;

int colordp[3][2] = { 0 };
int c_dp = 0;

ab lock[4];

/* texture mapping set variable */
int game_step[3][3] = { 0 };
int game_mode = 0;//game:1 idle:0
int coin_input = 0; //0:coin 안들어감 1:coin들어감
float go[3] = { 0 };//x,y,z 좌표 이동하는 정도
int joy[5] = { 0 };//joystick의 상태 좌우상하 push
int stickop[5] = { 0 };//joystick에 따라 stick의 움직임
bool antialiase_on = true;
double radius = 16;
double theta = 60, phi = 55;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };
bool pushed[6][5] = { 0 };
float push_obj[6][5] = { 0 };
///////////////////////////////


// user-defined function
void init(void);
void light_default();
void draw_axis();
void add_menu();
void set_obj();
void mouse(int, int, int, int);
void mouseWheel(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);
void keyboard(unsigned char, int, int);
void special_keyboard(int, int, int);
void draw(void);
void resize(int, int);
void dprack();
void joystick_op();
void pushstickop();
void draw_obj(ObjParser* objParser);
void draw_obj_with_texture(ObjParser* objParser,int i);
void idle();
void MyTimer(int value);
//...

int g_nX, g_nY;
int g_nSelect = 0;

int g_nGLWidth = 500, g_nGLHeight = 500;

void DrawSphere() {
	glLoadName(1);
	glPushMatrix();
	glTranslatef(g_pPosition[0], g_pPosition[1], -10.0f);
	glColor4f(1, 0, 0,1);
	glutSolidSphere(1.0f, 30, 30);
	//glutSolidCube(25);
	glPopMatrix();

	glLoadName(2);
	glPushMatrix();
	glTranslatef(g_pPosition[2], g_pPosition[3], -8.0f);
	glColor4f(0, 1, 0,1);
	glutSolidSphere(1.0f, 30, 30);
	glPopMatrix();

	glLoadName(3);
	glPushMatrix();
	glTranslatef(g_pPosition[4], g_pPosition[5], -9.0f);
	glColor4f(0, 0, 1,1);
	glutSolidSphere(1.0f, 30, 30);
	//gluPartialDisk(myobject, 0.0, 25.0, 60, 4, 0.0, 270.0);
	glPopMatrix();
}
void Picking(int x, int y)
{
	GLuint selectBuf[100];
	for (int i = 0; i < 100; i++)
		selectBuf[i] = 0;
	glSelectBuffer(100, selectBuf);
	//buffer에 GLuint 형의 배열에 picking한 개체에 대한 정보 저장
	glRenderMode(GL_SELECT);
	//GL_SELECT를 사용하여 선택 모드 설정
	glMatrixMode(GL_PROJECTION);
	//선택의 처리는 시점 좌표계에서 실시하므로 투시 변환 행렬 설정
	glInitNames();
	//개체의 이름 (정수)를 등록해두면 이름 스택을 초기화
	//반드시 glRenderMode(GL_SELECT)를 호출 후 실행해야한다.
	glPushName(-1);
	//glPushName에 들어가는 parameter는 개체의 이름을 나타낸다.
	//glInitName() 직 후 호출하면 이름 스택의 선두에 임시로 -1을 넣어둔다.
	glPushMatrix();//투시 변환 행렬(glMatrix(GL_PROJECTION)를 matrix stack에 저장
	glLoadIdentity();//투시 변환 행렬 초기화

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPickMatrix(x, y, 0.1, 0.1, viewport);
	//표시 영역이 마우스 포인터 주위(viewport)만 사용하도록 변환 행렬 설정
	//마우스 클릭 시 x,y가 선택영역의 중심 좌표가 된다. 그 뒤 인자는 dx, dy로
	//작아질 수록 세밀하게 선택가능하다.
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//fourth step
	glOrtho(-g_nGLWidth / 2.0f, g_nGLWidth / 2.0f, -g_nGLHeight / 2.0f, g_nGLHeight / 2.0f, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();

	GLint hits = glRenderMode(GL_RENDER);
	//sixth step
	if (hits <= 0)  return;

	int stack = selectBuf[0];
	unsigned int zMax = selectBuf[2];

	unsigned int zMin = selectBuf[1];
	g_nSelect = selectBuf[3];

	int index = 3 + stack;
	int i;
	for (i = 1; i < hits; i++)
	{
		stack = selectBuf[index];
		if (zMax < selectBuf[index + 2]) {
			zMax = selectBuf[index + 2];
			g_nSelect = selectBuf[index + 3];
		}
		index += 3 + stack;
	}
	/*for (i = 1; i < hits; i++)
	{
		stack = selectBuf[index];
		if (zMin > selectBuf[index + 1]) {
			zMin = selectBuf[index + 1];
			g_nSelect = selectBuf[index + 3];
		}
		index += 3 + stack;
	}*/
	printf("hit is %d\n", hits);
	for (int i = 0; i < 100; i++)
	{
		if (selectBuf[i] == 0)
			break;
		if (i % 4 == 0)
			printf("stack : %u\n", selectBuf[i]);
		else if (i % 4 == 1)
			printf("zmin : %u\n", selectBuf[i]);
		else if (i % 4 == 2)
			printf("zmax : %u\n", selectBuf[i]);
		else if (i % 4 == 3)
			printf("g_nSelect : %u\n", selectBuf[i]);
	}

}


int ccco = 0;
void MyTimer(int value)
{
	if(ccco==1)
		coin_input++;
	if (c_dp == 0)
		c_dp = 1;
	else if (c_dp == 1)
		c_dp = 0;
	glutPostRedisplay();
	glutTimerFunc(100000 / 60, MyTimer, 1); // 타이머는 한번만 불리므로 타이머 함수 안에서 다시 불러준다.
}

void draw_string(void* font, const char* str, float x_position, float y_position, float red, float green, float blue)
{
	glPushAttrib(GL_LIGHTING_BIT);//속성을 push하는 것
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-5, -5, -5, 5); //이제 화면의 좌표는 (-5,-5) ~ (5,5)
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(red, green, blue,1);
	glRasterPos3f(x_position, y_position, 0);
	for (unsigned int i = 0; i < strlen(str); i++)
		glutBitmapCharacter(font, str[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

void draw_text() {
	char* str = (char*)"READY";

	switch (coin_input) {
	case 2:
		str = (char*)"START";
		break;
	case 3:
		str = (char*)"1";
		break;
	case 4:
		str = (char*)"2";
		break;
	case 5:
		str = (char*)"3";
		break;
	case 6:
		str = (char*)"end";
		break;
	case 7:
		coin_input = 1;
		ccco = 0;
		break;
	default:
		str = (char*)"READY";
		break;
	}
	

	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -0.2, 0.87, 1, 1, 0);
	glColor4f(1, 1, 1,1);
}


/* Main method */
int main(int argc, char** argv)
{
	// glut initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(250, 250);
	glutCreateWindow("GLUT Test");	// 위의 순서 무조건 고정
	
	// 사용자 초기화 함수
	init();

	// pop-up 메뉴 등록 함수
	add_menu();

	// obj 로드 함수
	set_obj();
	

	/* Create a single window with a keyboard and display callback */
	glutMouseFunc(&mouse);
	glutMouseWheelFunc(&mouseWheel);
	glutMotionFunc(&motion);
	glutPassiveMotionFunc(&passiveMotion);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&special_keyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);
	glutTimerFunc(1000 / 60, MyTimer, 1); // 한번만 실행, 타이머 함수 등록

	glutIdleFunc(&idle);

	/* Run the GLUT event loop */
	glutMainLoop();

	return EXIT_SUCCESS;
}


void init()
{
	printf("init func called\n");
	// clear background color
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glLoadIdentity();
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	colordp[0][0] = 1; colordp[1][0] = 0; colordp[2][0] = 0;
	colordp[0][1] = 0; colordp[1][1] = 1; colordp[2][1] = 0;

	light_default();
	setting();
	for (int i = 0; i < 4; i++)
	{
		lock[i].a = 0, lock[i].b = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			pushed[i][j] = 0;
			push_obj[i][j] = 0;
		}
	}
	env.envset();
}

void special_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) {
		joy[0] = 1;
		joy[1] = 0;
		joy[2] = 0;
		joy[3] = 0;
	}
	else if (key == GLUT_KEY_RIGHT) {
		joy[0] = 0;
		joy[1] = 1;
		joy[2] = 0;
		joy[3] = 0;
	}
	else if (key == GLUT_KEY_UP) {
		joy[0] = 0;
		joy[1] = 0;
		joy[2] = 1;
		joy[3] = 0;
	}
	else if (key == GLUT_KEY_DOWN) {
		joy[0] = 0;
		joy[1] = 0;
		joy[2] = 0;
		joy[3] = 1;
	}
	glutPostRedisplay();
}

/* Keyboard callback function */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
			/* Exit on escape key press */
		case '\x1B':
		{
			exit(EXIT_SUCCESS);
			break;
		}
		case 'g':
		{
			phi -= 5;
			if (phi < 0) phi = 355;
			//printf("phi %Lf\n", phi);
			break;
		}
		case 'h':
		{
			phi += 5;
			if (phi >= 360) phi = 0;
			//printf("phi %Lf\n", phi);
			break;
		}
		case 'y':
		{
			//if (theta > 10) 
				theta -= 5; //printf("theta %Lf\n", theta);
				break;
		}
		case 'b':
		{
			//if (theta < 170) 
				theta += 5;// printf("theta %Lf\n", theta);
			break;
		}
		case 32://space bar//push
		{
			joy[4] = 1;
			break;
		}
		case 'c':
		{
			coin_input = 1;
			break;
		}
		case 'l':
		{
			if (lock[0].a == 0 && lock[1].a==0)
				lock[0].a = 1;
			break;
		
		}
		case 'L':
		{
			if (lock[0].a == 1 && lock[1].a == 0)
				lock[1].a = 1;
			break;
		}
	}
	//printf("%Lf %Lf %Lf\n", cam[0], cam[1], cam[2]);
	glutPostRedisplay();
}

/* Display callback function */
void draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
		cam[1] = radius * cos(theta * M_PI / 180);
		cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
		glDisable(GL_LIGHT1);
		//gluLookAt(0, -4, 5, 0, -4, 0, 0, 1, 0);

		if ((0 <= theta && theta < M_PI / 2) || (3 * M_PI / 2 < theta && theta <= 2 * M_PI)) {
			gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, -1, 0);
		}
		else {
			gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);
		}
		
		
		
	//gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	if (game_mode == 1)
	{
		glPopMatrix();
		phi = 0; theta = 78; radius = 0.05;
	
		gluLookAt(8.9, 0.8, 0, 0, 0, 0, 0, 1, 0);
		glTranslatef(0, -4, 0);
		
		if (coin_input != 0)
		{

			ccco = 1;
			draw_obj_with_texture(Coin, 9);
			draw_text();
		}
		glPopMatrix();
	}
	glPopMatrix();
	env.draw_skybox(60,1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	
	
	//glTranslatef(-5, 0, 0);
	draw_obj_with_texture (Machine,8);
	draw_obj(Coinmachine);
	draw_obj(Timer);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1, 0, 0, 0.3);	 //색깔바꾸기
	draw_obj(Glass1);
	draw_obj(Glass2);
	
	glPushMatrix();
	if (lock[0].a == 1)
	{
		if (lock[1].a == 1)
		{
			
			glTranslatef(1.5, 0, -3.4);
			glRotatef(lock[0].b, 0, 1, 0);
			glTranslatef(-1.5, 0, 3.4);
			if (lock[0].b <= 0) { lock[0].a = 0; lock[1].a = 0; }
		}
		else {
			glTranslatef(1.5, 0, -3.4);
			glRotatef(lock[0].b, 0, 1, 0);
			glTranslatef(-1.5, 0, 3.4);
		}
	}
	
	
	draw_obj(DpPlane);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	draw_obj(Dphand);
	draw_obj_with_texture(Dpsecret2, 13);
	draw_obj_with_texture(Dpsecret, 14);
	///////////////////////
	DrawSphere();
	/////////////////////
	



	glPushMatrix();                                                    


	dprack();
	glPopMatrix();
	
	
	joystick_op();
	glPopMatrix();
	glPushMatrix();
	pushstickop();
	 
	glPopMatrix();
	draw_obj_with_texture(Presentbox, 12);
	//glPopMatrix();

	//dprack();
	 //색깔바꾸기
	draw_obj_with_texture(Boxpresent, 10);
	
			glTranslatef(0, 0, -8.0);
			glRotatef(180 + lock[0].b, 0, 0, 1);
			glTranslatef(0, 0, 8.0);
			if (lock[0].a == 0 && lock[1].a == 0)
				glTranslatef(0, 3.5, 0);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1, 0, 0, 0.1);
	draw_obj(Boxpresenttop);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	draw_obj_with_texture(Boxpresenthat, 11);

	glEnable(GL_LIGHTING);
	glColor4f(1, 1, 1, 1);	 //색깔바꾸기
	
	glFlush();
	glutSwapBuffers();
	
}

void dprack() {

	glColor4f(1.f, 1.f, 1.f,1.0f);
	draw_axis();
		 //색깔바꾸기
	draw_obj_with_texture(Dp,0);//(0,0,0)
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glPushMatrix();
	draw_obj(hlight);
	glTranslatef(0, 0, -0.5);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.5);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0, 0, -0.4);
	draw_obj(hlight);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	
	
	
	draw_obj_with_texture(Coke,1);
	draw_obj_with_texture(Present1, 17);
	draw_obj_with_texture(Present2, 16);
	draw_obj_with_texture(Present3, 3);
	/*glTranslatef(-0.5, 0, 2.3);
	glRotatef(60, 0, 1, 0 );
	glTranslatef(0.5, 0, -2.3);*/
	if (pushed[4][0] == 1)
	{
		glTranslatef(-0.5, 0, 2.3);
		glRotatef(push_obj[4][0], 0, 1, 0);
		glTranslatef(0.5, 0, -2.3);
	}
	if (pushed[4][1] == 1)
	{
		glTranslatef(push_obj[4][1], 0, 0);
	}
	if (pushed[4][2] == 1)
	{
		glTranslatef(-0.5, 0, 2.3);
		glRotatef(push_obj[4][2], 0, 1, 0);
		glTranslatef(0.5, 0, -2.3);
	}
	if (pushed[4][4] == 1)
	{
		glTranslatef(-push_obj[4][1], -5.8, 0);
	}
	if (pushed[4][3] == 1)
	{
		glTranslatef(0.7, 0.5, 0);
		glTranslatef(-0.5, 0, 2.3);
		glRotatef(push_obj[4][3], 0, 0, 1);
		glTranslatef(0.5, 0, -2.3);
	}

	draw_obj_with_texture(Present4, 4);
	glPopMatrix();
	//////////////////////////////////////////////////
	if (pushed[5][0] == 1)
	{
		glTranslatef(-0.4, 0, 0);
		glRotatef(push_obj[5][0], 0, 1, 0);
	}
	if (pushed[5][1] == 1)
	{
		glTranslatef(push_obj[5][1], 0, 0);
	}
	if (pushed[5][2] == 1)
	{
		glTranslatef(-0.4, 0, 0);
		glRotatef(push_obj[5][2], 0, 1, 0);
	}
	if (pushed[5][4] == 1)
	{
		glTranslatef(-push_obj[5][1], -5.8, 0);
	}
	if (pushed[5][3] == 1)
	{
		glTranslatef(0.6, 1, 0);
		glRotatef(push_obj[5][3], 0, 0, 1);
		
	}
	draw_obj_with_texture(Cider, 2);
	glPopMatrix();

}
void joystick_op()
{
	//glDisable(GL_TEXTURE_2D);
	
	draw_obj_with_texture(Joystickboard,5);
	
	glPushMatrix();
	//glEnable(GL_TEXTURE_2D);
	if (joy[0] == 1)//좌
	{
		glTranslatef(0, 0, 0.5);
		glTranslatef(0, 0.4, 0);
		glRotatef(30, 1, 0, 0);
		stickop[0] = 1;
		joy[0] = 0;
	}
	else if (joy[1] == 1)//우
	{
		
		glTranslatef(0,0, -0.5);
		glTranslatef(0, -0.7, 0);
		glRotatef(-30, 1, 0, 0);
		stickop[1] = 1;
		joy[1] = 0;
	}
	else if (joy[2] == 1)//위
	{
		glTranslatef(0, -1.15, 0);
		glRotatef(30, 0, 0, 1);
		stickop[2] = 1;
		joy[2] = 0;
	}
	else if (joy[3] == 1)//아래
	{
		glTranslatef(0.5, 0.8, 0);
		glRotatef(-30, 0, 0, 1);
		stickop[3] = 1;
		joy[3] = 0;
	}

	//draw_obj_with_texture(Joystick, joystick);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1, 1, 0, 1);	 //색깔바꾸기
	draw_obj(Joystick);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	draw_obj_with_texture(Joystick_ball,6);
	glPopMatrix();
	glPushMatrix();
	if (joy[4] == 1)
	{
		glTranslatef(0, -0.1, 0);
		stickop[4] = 1;
		joy[4] = 0;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1, 0, 0, 1);	 //색깔바꾸기
	draw_obj_with_texture(Pushbutton,7);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	


	//glutPostRedisplay();
}

void pushstickop() {
	//if(coin_input!=0)
		glTranslatef(0, 0, go[2]);
	draw_obj_with_texture(Stickbarh, 15);
	//if (coin_input != 0)
		glTranslatef(0, go[1], 0);
	if (stickop[0] == 1)//좌
	{
		if (go[2] < 0.7)
		{
			go[2] += 0.1;
		}
		stickop[0] = 0;

	}
	else if (stickop[1] == 1)//우
	{
		if (go[2] > -5)
		{
			go[2] -= 0.1;
		}
		stickop[1] = 0;
	}
	else if (stickop[2] == 1)//위
	{
		if (go[1] < 4)
		{
			go[1] += 0.1;
		}
		stickop[2] = 0;
	}
	else if (stickop[3] == 1)//아래
	{
		if (go[1] > 0)
		{
			go[1] -= 0.1;
		}
		stickop[3] = 0;
	}
	else if (stickop[4] == 1)//push
	{
		if (go[0] > -1)
		{
			//가장 왼쪽 아래
			if (go[1] >= 0.0 && go[1] <= 0.8 && go[2] >= 0.2 && go[2] <= 0.6)
			{
				printf("pushed down\n");
				pushed[4][1] = 1;
			}
			if (go[1] >= 0.0 && go[1] <= 1.2 && go[2] >= 0.5 && go[2] <= 0.6)
			{
				printf("pushed dleft\n");
				pushed[4][0] = 1;
			}
			if (go[1] >= 0.0 && go[1] <= 1.2 && go[2] >= 0.0 && go[2] <= 0.2)
			{
				printf("pushed dright\n");
				pushed[4][2] = 1;
			}
			if (go[1] >= 0.7 && go[1] <= 1.2 && go[2] >= 0.2 && go[2] <= 0.6)
			{
				printf("pushed dup\n");
				pushed[4][3] = 1;
			}
			//가운데 아래
			if (go[1] >= 0.0 && go[1] <= 0.6 && go[2] >= -2.2 && go[2] <= -1.8)
			{
				printf("pushed mdown\n");
				pushed[5][1] = 1;
			}
			if (go[1] >= 0.0 && go[1] <= 0.9 && go[2] >= -2.3 && go[2] <= -2.1)
			{
				printf("pushed mdright\n");
				pushed[5][2] = 1;
			}
			if (go[1] >= 0.0 && go[1] <= 0.9 && go[2] >= -1.8 && go[2] <= -1.5)
			{
				printf("pushed mleft\n");
				
				pushed[5][0] = 1;
			}
			if (go[1] >=  0.6&& go[1] <= 0.9 && go[2] >= -2.2 && go[2] <= -1.8)
			{
				printf("pushed mdup\n");
				pushed[5][3] = 1;
			}
			//가장 왼쪽 위
			if (go[1] >= 2.4 && go[1] <= 3.0 && go[2] >= 0.0 && go[2] <= 0.4)
				printf("pushed up\n");
			//가운데 위
			if (go[1] >= 2.4 && go[1] <= 3.4 && go[2] >= -2.2 && go[2] <= -1.6)
				printf("pushed mup\n");
			//가장 오른쪽 아래
			if (go[1] >= 0.0 && go[1] <= 0.8 && go[2] >= -4.4 && go[2] <= -3.6)
				printf("pushed rdown\n");
			//가장 오른쪽 위
			if (go[1] >= 2.6 && go[1] <= 3.4 && go[2] >= -4.4 && go[2] <= -4.0)
				printf("pushed rup\n");
			printf("%f %f \n", go[0], go[2]);
			go[0] = 0;
			coin_input=6;
		}
		stickop[4] = 0;
	}
	
	//glRotatef(90, 0, 0, 1);

	draw_obj(Stickbar);
	//if (coin_input != 0)
		glTranslatef(go[0], 0, 0);
	draw_obj(Stick);

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	y = g_nGLHeight - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Picking(x, y);
		g_nX = x;
		g_nY = y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		g_nSelect = 0;
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		//printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		//printf("radius %f \n", radius);

		if (radius > 1) radius -= 0.5;
	}
	else {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius < 100) radius += 0.5;
	}
	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	y = g_nGLHeight - y;
	if (g_nSelect > 0) {
		g_pPosition[(g_nSelect - 1) * 2 + 0] += x - g_nX;
		g_pPosition[(g_nSelect - 1) * 2 + 1] += y - g_nY;
		g_nX = x;
		g_nY = y;
		glutPostRedisplay();
	}

}

void main_menu(int option)
{
	if (option == 99) exit(0);
	else if (option == 10) {
		game_mode = 1;
	}
	else if (option == 11) {
		game_mode = 0;
	}
	else if (option == 1) {
		game_step[0][0] = 1;
	}
	glutPostRedisplay();
}

void sub_menu(int option)
{
	printf("Submenu %d has been selected\n", option);
}

void add_menu()
{
	int mainmenu1 = glutCreateMenu(&main_menu);

	glutAddMenuEntry("game_mode", 10);
	glutAddMenuEntry("normal_mode", 11);
	glutAddMenuEntry("Quit", 99);
	glutAddMenuEntry("first game", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void idle()
{
	/*frame++;
	time = glutGet(GLUT_ELAPSED_TIME); // unit : ms

	if (time - timebase > 1000)
	{
		fps = frame * 1000.f / (time - timebase);
		timebase = time;
		frame = 0;
		//printf("fps : %.0f\n", fps);
	}*/
	if(lock[0].a==1 && lock[0].b<90 && lock[1].a==0)
		lock[0].b+=0.1;
	else if (lock[1].a == 1 && lock[0].b >=0 && lock[0].a==1)
		lock[0].b -= 0.1;
	if (pushed[4][1] == 1)
	{ 
		if (push_obj[4][1] > -1.5)
			push_obj[4][1] -= 0.01;
		else if (push_obj[4][1] <= -1.5)
			pushed[4][4] = 1;
	}
	if (pushed[4][0] == 1)
	{
		if (push_obj[4][0] > -60)
			push_obj[4][0] -= 1;
	
	}
	if (pushed[4][2] == 1)
	{
		if (push_obj[4][2] < 60)
			push_obj[4][2] += 1;

	}
	if (pushed[4][3] == 1)
	{
		if (push_obj[4][3] < 60)
			push_obj[4][3] += 1;

	}
	/////////////////////////////////////
	if (pushed[5][1] == 1)
	{
		if (push_obj[5][1] > -1.5)
			push_obj[5][1] -= 0.01;
		else if (push_obj[5][1] <= -1.5)
			pushed[5][4] = 1;
	}
	if (pushed[5][0] == 1)
	{
		if (push_obj[5][0] > -60)
			push_obj[5][0] -= 1;

	}
	if (pushed[5][2] == 1)
	{
		if (push_obj[5][2] < 60)
			push_obj[5][2] += 1;

	}
	if (pushed[5][3] == 1)
	{
		if (push_obj[5][3] < 60)
			push_obj[5][3] += 1;

	}
}

void resize(int width, int height)
{
	printf("resize func called\n");
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 500, 0, 500);
	gluPerspective(60, (double)width / (double)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}
void draw_obj(ObjParser* objParser)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}

void draw_obj_with_texture(ObjParser* objParser,int i)
{

	
	glDisable(GL_BLEND);
	// glEnable(GL_TEXTURE_2D);	// texture 색 보존을 위한 enable
	glBindTexture(GL_TEXTURE_2D, textureMonkey[i]);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}

void draw_axis(void)
{
	glLineWidth(1.5f);
	glBegin(GL_LINES);

	glColor4f(1.f, 0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(4.f, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 4.f, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 4.f);

	glEnd();
	glLineWidth(1);
}