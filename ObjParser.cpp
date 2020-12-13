#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string.h>
#include "ObjParser.h"
#include "bmpfuncs.h"
#include "environment.h"
#include <iostream>
#include <Windows.h>
#include "setting_header.h"
using namespace std;

struct ab {
	float a, b;
};
struct xyz {
	double x, y, z;
};
int per[4] = { 60,500 / 500,1,1000 };
//float g_pPosition[6] = { -1.0f,-4.0f,-0.5f,-3.0f,0.0f,-2.0f };
float g_pPosition[5] = { 0.4f,0.1f,0.1f,0.1f };

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

int totalmoney = 0;
int getdoll[2][7] = { 0 };
int money = 0;
/* texture mapping set variable */
int game_step[3][3] = { 0 };
int game_mode = 0;//game:1 idle:0
int coin_input = 0; //0:coin 안들어감 1:coin들어감
float go[3] = { 0 };//x,y,z 좌표 이동하는 정도
int joy[5] = { 0 };//joystick의 상태 좌우상하 push
int joyjoy = 0;
int stickop[5] = { 0 };//joystick에 따라 stick의 움직임
bool antialiase_on = true;
double radius = 16;
double theta = 90, phi = 00;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };
bool pushed[7][5] = { 0 };
float push_obj[7][5] = { 0 };
///////////////////////////////
xyz dollposition[7];



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
void draw_gamenum();
//...
double predoor = 0;

int g_nX, g_nY;
int g_nSelect = 0;

int g_nGLWidth = 500, g_nGLHeight = 500;

void DrawSphere() {
	glLoadName(1);
	glPushMatrix();
	glTranslatef(g_pPosition[0]/15, g_pPosition[1]/15, 0.0f);
	
	if (-4 < g_pPosition[0] / 15 && -2 > g_pPosition[0] / 15)
	{
		if (-5 < g_pPosition[0] / 15 && -3 > g_pPosition[0] / 15)
		{
			money++;
			totalmoney++;
			g_pPosition[0] = 0.4f;
			g_pPosition[1] = 0.1f;
		}
	}
	draw_obj_with_texture(Coin, 9);
	glPopMatrix();

	glLoadName(2);
	glPushMatrix();
	
	if (predoor >= 0.1 )
	{
		
		glTranslatef(-1.16, -2.96, 1.48);
		glRotatef(-predoor, 1, 0, 0);
		glTranslatef(1.16, 2.96, -1.48);
	}
	
	if(lock[0].a==0)
		draw_obj_with_texture(Presentbox,12);
	glPopMatrix();
	
}
void Picking(int x, int y)
{
	printf("%f %f\n", g_pPosition[0] / 15, g_pPosition[1] / 15);
	cout << theta << "  " << phi << endl;
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
	gluPickMatrix(x, y, 0.01, 0.01, viewport);
	//표시 영역이 마우스 포인터 주위(viewport)만 사용하도록 변환 행렬 설정
	//마우스 클릭 시 x,y가 선택영역의 중심 좌표가 된다. 그 뒤 인자는 dx, dy로
	//작아질 수록 세밀하게 선택가능하다.
	
	//fourth step
	gluPerspective(per[0], per[1], per[2], per[3]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	DrawSphere();
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
	if (g_nSelect == 2) predoor = 0.1;
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
	//cout << phi << " " << theta << endl;
	//printf("theta %f phi %f cam[0] %f cam[1] %f cam[2] %f radius %f\n", theta, phi, cam[0], cam[1], cam[2], radius);

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
		str = (char*)"4";
		break;
	case 7:
		str = (char*)"5";
		break;
	case 8:
		str = (char*)"6";
		break;
	case 9:
		str = (char*)"7";
		break;
	case 10:
		str = (char*)"end";
		break;
	case 11:
		str = (char*)"READY";
		go[0] = 0; go[1] = 0; go[2] = 0;
		ccco = 0;
		if(money==0)
			coin_input = 0;
		else if (money != 0)
		{
			money--;
			coin_input = 1;
		}
		break;
	default:
		str = (char*)"READY";
		break;
	}
	

	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -0.2, 0.87, 1,0 , 0);
	glColor4f(1, 1, 1,1);
}
void draw_gamenum()
{
	char* str = (char*)"0 coin";

	switch (money) {
	case 1:
		str = (char*)"1 COIN";
		break;
	case 2:
		str = (char*)"2 COIN";
		break;
	case 3:
		str = (char*)"3 COIN";
		break;
	case 4:
		str = (char*)"4 COIN";
		break;
	case 5:
		str = (char*)"5 COIN";
		break;
	case 6:
		str = (char*)"6 COIN";
		break;
	case 7:
		str = (char*)"7 COIN";
		break;
	
	default:
		str = (char*)"0 COIN";
		break;
	}
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -0.7, 0.87, 1, 0, 0);
	glColor4f(1, 1, 1, 1);
}
void draw_money()
{
	char str[50];
	sprintf_s(str, "total money : %d", totalmoney * 500);

	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -0.7, 0.75, 1, 0, 0);
	glColor4f(1, 1, 1, 1);
}

/* Main method */
int main(int argc, char** argv)
{
	// glut initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);// | GLUT_DEPTH);
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
	glutMotionFunc(motion);
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
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	colordp[0][0] = 1; colordp[1][0] = 0; colordp[2][0] = 0;
	colordp[0][1] = 0; colordp[1][1] = 1; colordp[2][1] = 0;

	dollposition[4] = { 1.77,0.63,0.47 };
	dollposition[5] = { 1.97, 0.63, 0.57 };
	dollposition[1] = { 4.17, 3.23, 0.57 };
	dollposition[2] = { 0.53,3.83,0.77 };
	dollposition[3] = { 2.83 ,3.23,0.47 };
	dollposition[6] = { 2.67, 1.23 , 0.77 };

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
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(per[0], per[1], per[2], per[3]);
	cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
	cam[1] = radius * cos(theta * M_PI / 180);
	cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
	glDisable(GL_LIGHT1);
	if (theta > 180)
	{
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, -1, 0);
	}

	else if (theta <= 180) {
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);
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
		joyjoy = 1;
	}
	else if (key == GLUT_KEY_RIGHT) {
		joy[0] = 0;
		joy[1] = 1;
		joy[2] = 0;
		joy[3] = 0;
		joyjoy = 2;

	}
	else if (key == GLUT_KEY_UP) {
		joy[0] = 0;
		joy[1] = 0;
		joy[2] = 1;
		joy[3] = 0;
		joyjoy = 3;

	}
	else if (key == GLUT_KEY_DOWN) {
		joy[0] = 0;
		joy[1] = 0;
		joy[2] = 0;
		joy[3] = 1;
		joyjoy = 4;

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
			break;
		}
		case 'h':
		{
			phi += 5;
			if (phi >= 360) phi = 0;
			break;
		}
		case 'y':
		{
			if (theta <= 5) theta += 360;
			theta -= 5;
				break;
		}
		case 'b':
		{
			if (theta >= 360) theta -= 360;
			theta += 5;
			break;
		}
		case 32://space bar//push
		{
			printf("go %f %f \n", go[1], go[2]);
		
			if(coin_input!=0)
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
		case 'd':
		{
			getdoll[1][0] = 1;
			predoor = 0;
			break;
		}
		case 'm':
		{
			if (lock[0].a == 1 && lock[1].a == 0)
				totalmoney=0;
			break;
		}
	}
	cout << "theta " << theta << " phi " << phi << endl;
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
	//draw_axis();

	cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
	cam[1] = radius * cos(theta * M_PI / 180);
	cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
	
	
	glDisable(GL_LIGHT1);
	if (theta > 180 )  
	{
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, -1, 0);
	}
	
	else if (theta <= 180) {
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);                                               
	}
	env.draw_skybox(60,  0  );
	//glColor3f(1, 1, 1);
	glTranslatef(0, -2, 0);
	
	glPushMatrix();
	
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	
	glPushMatrix();

	
	draw_gamenum();
	if(game_mode!=1)
		draw_money();
	DrawSphere();
	if (game_mode == 1)
	{
		glPopMatrix();
		//cout << g_nGLWidth << " " << g_nGLHeight << endl;

		phi = 0; theta = 90; radius = 8.5;
		//phi = 0; theta = 80; radius = 10;


		//gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);

		//glTranslatef(0, -4, 0);
		//glPopMatrix();
			if (coin_input != 0 && money != 0)
			{
				ccco = 1;
				draw_text();
			}
			

	}
	glPopMatrix();
	glPushMatrix();

	draw_obj_with_texture(Dphand, 8);


	if (lock[0].a == 1)
	{
		if (lock[1].a == 1)
		{
			
			glTranslatef(0.5, 0, -3.4);
			glRotatef(lock[0].b, 0, 1, 0);
			glTranslatef(-1.5, 0, 3.4);
			if (lock[0].b <= 0) { lock[0].a = 0; lock[1].a = 0; }
		}
		else {
			
			//glTranslatef(0, 0, 5);
			glTranslatef(3, 0, -0);
			glRotatef(lock[0].b, 0, 1, 0);
			glTranslatef(-3, 0, 0.0);
		}
	}
	
	draw_obj_with_texture(Machine, 8);


	//draw_obj(Timer);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1, 0, 0, 0.3);	 //색깔바꾸기
	draw_obj(Glass1);
	draw_obj(Glass2);

	draw_obj(DpPlane);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	draw_obj_with_texture(Dpsecret2, 13);
	
	draw_obj_with_texture(Dpsecret, 14);
	

	joystick_op();

	glPopMatrix();
	


	dprack();

	glPopMatrix();

	

	pushstickop();
	 
	glPopMatrix();
	
	//glPopMatrix();

	//dprack();
	 //색깔바꾸기
	/*draw_obj_with_texture(Boxpresent, 10);
	glPushMatrix();
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
	glPopMatrix();
	*/
	


	glEnable(GL_LIGHTING);
	glColor4f(1, 1, 1, 1);	 //색깔바꾸기
	
	glFlush();
	glutSwapBuffers();
	
}

void dprack() {

	glColor4f(1.f, 1.f, 1.f,1.0f);

		 //색깔바꾸기
	draw_obj_with_texture(Dp,0);//(0,0,0)
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glPushMatrix();
	draw_obj(hlight);
	glTranslatef(0.5, 0, 0);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);
	glColor4f(colordp[0][c_dp], colordp[1][c_dp], colordp[2][c_dp], 1);
	glTranslatef(0.5, 0, 0);
	draw_obj(hlight);

	glPopMatrix();
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	

	///////////////////////////////////////////////////////////////////////////(4)
	if (pushed[4][0] == 1)
	{
		glTranslatef(-dollposition[4].x, 0, -dollposition[4].z);
		glRotatef(push_obj[4][0], 0, 1, 0);
		glTranslatef(dollposition[4].x, 0, dollposition[4].z);
	}
	else if (pushed[4][1] == 1)
	{
		glTranslatef(0, 0, push_obj[4][1]);
	}
	else if (pushed[4][2] == 1)
	{
		glTranslatef(-dollposition[4].x, 0, -dollposition[4].z);
		glRotatef(push_obj[4][2], 0, 1, 0);
		glTranslatef(dollposition[4].x, 0, dollposition[4].z);
	}
	if (pushed[4][4] == 1)
	{
		glTranslatef(0, -dollposition[4].y*9.5, dollposition[4].z*4);
		getdoll[0][0]++;
		getdoll[0][4] = 1;
	}
	else if (pushed[4][3] == 1)
	{
		glTranslatef(0, dollposition[4].y/10, 0);
		glTranslatef(-dollposition[4].x, 0, -dollposition[4].z);
		glRotatef(-push_obj[4][3], 1, 0, 0);
		glTranslatef(dollposition[4].x, 0, dollposition[4].z);
	}
	if(getdoll[1][0] != 1 || getdoll[0][4] != 1 )
		draw_obj_with_texture(Present4, 4);
	glPopMatrix();
	glPushMatrix();
	///////////////////////////////////////////////////////////////////////////(5)
	if (pushed[5][0] == 1)
	{
		glTranslatef(0, 0, -dollposition[5].z);
		glRotatef(push_obj[5][0], 0, 1, 0);
	}
	else if (pushed[5][1] == 1)
	{
		glTranslatef(0, 0, -dollposition[5].z);
	}
	else if (pushed[5][2] == 1)
	{
		glTranslatef(0, 0, -dollposition[5].z);
		glRotatef(push_obj[5][2], 0, 1, 0);
	}
	if (pushed[5][4] == 1)
	{
		
		glTranslatef(-dollposition[5].x*1.5, -dollposition[5].y*9.5, dollposition[5].z );
		getdoll[0][0]++;
		getdoll[0][5] = 1;
	}
	if (pushed[5][3] == 1)
	{
		glTranslatef(0, dollposition[5].y/2, 0);
		glRotatef(-push_obj[5][3], 1, 0, 0);
	}
	if (getdoll[1][0] != 1 || getdoll[0][5] != 1)
		draw_obj_with_texture(Cider, 2);
	glPopMatrix();
	glPushMatrix();
	/////////////////////////////////////////////////////////////////////////(1)
	
	if (pushed[1][0] == 1)//left
	{
		glTranslatef(-dollposition[1].x / 15, 0, 0);
		glRotatef(push_obj[1][0], 0, 1, 0);
		
	
	}
	if (pushed[1][1] == 1)//good
	{
		glTranslatef(0, 0, push_obj[1][1]);
	}
	if (pushed[1][2] == 1)//right
	{
		glTranslatef(0, 0, -dollposition[1].z * 1.5);
		glRotatef(push_obj[1][2], 0, 1, 0);
	
	}
	if (pushed[1][4] == 1)//good
	{
		
		glTranslatef(0, -dollposition[1].y*2.6, dollposition[1].z * 4);
		getdoll[0][0]++;
		getdoll[0][1] = 1;
	}
	if (pushed[1][3] == 1)//back
	{
		glTranslatef(0, dollposition[1].y/2, dollposition[1].z*4);
		glTranslatef(-dollposition[1].x, 0, -dollposition[1].z);
		glRotatef(-push_obj[1][3], 1, 0, 0);
		glTranslatef(dollposition[1].x, 0, dollposition[1].z);
	}
	if (getdoll[1][0] != 1 || getdoll[0][1] != 1)
		draw_obj_with_texture(Coke,1);
	glPopMatrix();
	glPushMatrix();
	/////////////////////////////////////////////////////////////////////////(2)
	if (pushed[2][0] == 1)//left
	{
		glTranslatef(0, 0, -dollposition[2].z / 2);
		glTranslatef(-dollposition[2].x / 15, 0,0);
		glRotatef(push_obj[2][0], 0, 1, 0);

	}
	else if (pushed[2][1] == 1)//good
	{
		glTranslatef(0, 0, push_obj[2][1]);
	}
	else if (pushed[2][2] == 1)//right
	{
		glTranslatef(dollposition[2].x/2, 0, 0);
		glRotatef(push_obj[2][2], 0, 1, 0);

	}
	if (pushed[2][4] == 1)//good
	{
	
		glTranslatef(-dollposition[2].x * 3.5, -dollposition[1].y * 2.6, -push_obj[2][1] * 2);
		getdoll[0][0]++;
		getdoll[0][2] = 1;
	}
	else if (pushed[2][3] == 1)//back
	{
		glTranslatef(0 , dollposition[2].y/5, dollposition[2].z * 3.5);
		glTranslatef(-dollposition[2].x, 0, -dollposition[2].z);
		glRotatef(-push_obj[2][3], 1, 0, 0);
		glTranslatef(dollposition[2].x, 0, dollposition[2].z);
	}
	if (getdoll[1][0] != 1 || getdoll[0][2] != 1)
		draw_obj_with_texture(Present1, 17);
	glPopMatrix();
	glPushMatrix();
	//////////////////////////////////////////////////////(6)
	if (pushed[6][0] == 1)//left
	{
		glTranslatef(0 , 0, -dollposition[6].z*2.5);
		glRotatef(push_obj[6][0], 0, 1, 0);
	}
	else if (pushed[6][1] == 1)//good
	{
		glTranslatef(0, 0, push_obj[6][1]);
	}
	else if (pushed[6][2] == 1)//right
	{
	
		glTranslatef(dollposition[6].x, 0, -dollposition[6].z);
		glRotatef(push_obj[6][2], 0, 1,0);
		glTranslatef(-dollposition[6].x, 0, dollposition[6].z);

	}
	if (pushed[6][4] == 1)//good
	{
		glTranslatef(-dollposition[6].x * 1.7, -dollposition[6].y * 4.8, -push_obj[6][1]*2 );
		getdoll[0][0]++;
		getdoll[0][6] = 1;
	}
	else if (pushed[6][3] == 1)//back
	{
		glTranslatef(0 ,0, dollposition[6].z);
		glTranslatef(-dollposition[6].x, 0, -dollposition[6].z);
		glRotatef(-push_obj[6][3], 1, 0, 0);
		glTranslatef(dollposition[6].x, 0, dollposition[6].z);
	}
	if (getdoll[1][0] != 1 || getdoll[0][6] != 1)
		draw_obj_with_texture(Present2, 16);
	glPopMatrix();
	glPushMatrix();
	///////////////////////////////////////////////////////(3)
	if (pushed[3][0] == 1)
	{
		glTranslatef(dollposition[3].x, 0, -dollposition[3].z);
		glRotatef(push_obj[3][0], 0, 1, 0);
		glTranslatef(-dollposition[3].x, 0, dollposition[3].z);
	}
	if (pushed[3][1] == 1)
	{
		glTranslatef(0, 0, push_obj[3][1]);
	}
	if (pushed[3][2] == 1)
	{
		glTranslatef(dollposition[3].x, 0, -dollposition[3].z);
		glRotatef(push_obj[3][2], 0, 1, 0);
		glTranslatef(-dollposition[3].x, 0, dollposition[3].z);
	}
	if (pushed[3][4] == 1)
	{
		glTranslatef(-dollposition[3].x * 1.5, -dollposition[3].y*2.6, -push_obj[3][1] * 2);
		getdoll[0][0]++;
		getdoll[0][3] = 1;
	}
	if (pushed[3][3] == 1)
	{
		glTranslatef(0, dollposition[3].y/2.3, dollposition[3].z*3 );
		glTranslatef(-dollposition[3].x, 0, dollposition[3].z);
		glRotatef(-push_obj[3][3], 1, 0, 0);
		glTranslatef(dollposition[3].x, 0, -dollposition[3].z);
	}
	if (getdoll[1][0] != 1 || getdoll[0][3] != 1)
		draw_obj_with_texture(Present3, 3);
	glPopMatrix();


}
void joystick_op()
{
	draw_obj_with_texture(Joystickboard,5);
	draw_obj_with_texture(Pushbutton, 7);
	if (joyjoy==0)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 0, 1);	 //색깔바꾸기
		draw_obj(Joystick);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		draw_obj_with_texture(Joystick_ball, 6);
	}
	
	if (joy[0] == 1 || joyjoy==1)//좌
	{
		glTranslatef(-0.467, -0.77, 2.013);
		glRotatef(30, 0, 0, 1);
		glTranslatef(0.467, 0.77, -2.013);
		if(joy[0]==1)
			stickop[0] = 1;
		joy[0] = 0;
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 0, 1);	 //색깔바꾸기
		draw_obj(Joystick);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		draw_obj_with_texture(Joystick_ball, 6);
		glPopMatrix();
		glPushMatrix();
	}
	else if (joy[1] == 1 || joyjoy == 2)//우
	{
		
		glTranslatef(-0.467, -0.77, 2.013);
		glRotatef(-30, 0, 0, 1);
		glTranslatef(0.467, 0.77, -2.013);
		if (joy[1] == 1)
			stickop[1] = 1;
		joy[1] = 0;
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 0, 1);	 //색깔바꾸기
		draw_obj(Joystick);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		draw_obj_with_texture(Joystick_ball, 6);
		glPopMatrix();
		glPushMatrix();

	}
	else if (joy[2] == 1 || joyjoy == 3)//위
	{
		
		glTranslatef(-0.467, -0.77, 2.013);
		glRotatef(-30, 1, 0, 0);
		glTranslatef(0.467, 0.77, -2.013);
		if (joy[2] == 1)
			stickop[2] = 1;
		joy[2] = 0;
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 0, 1);	 //색깔바꾸기
		draw_obj(Joystick);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		draw_obj_with_texture(Joystick_ball, 6);
		glPopMatrix();
		glPushMatrix();

	}
	else if (joy[3] == 1 || joyjoy == 4)//아래
	{
		glTranslatef(-0.467, -0.77, 2.013);
		glRotatef(30, 1, 0, 0);
		glTranslatef(0.467, 0.77, -2.013);
		if (joy[3] == 1)
			stickop[3] = 1;
		joy[3] = 0;
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 0, 1);	 //색깔바꾸기
		draw_obj(Joystick);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		draw_obj_with_texture(Joystick_ball, 6);
		glPopMatrix();
		glPushMatrix();

	}

	
	//glPushMatrix();
	if (joy[4] == 1 )
	{
		glTranslatef(0, -1, 0);
		
		stickop[4] = 1;
		joy[4] = 0;
		draw_obj_with_texture(Pushbutton, 7);
		glPopMatrix();
		glPushMatrix();
	}

	

	
}

void pushstickop() {
	if(coin_input!=0)
		glTranslatef(go[2], 0, 0);
	draw_obj_with_texture(Stickbarh, 15);
	if (coin_input != 0)
		glTranslatef(0, go[1], 0);
	if (stickop[0] == 1)//좌
	{
		if (go[2] > -0.8)
		{
			go[2] -= 0.1;
		}
		stickop[0] = 0;

	}
	else if (stickop[1] == 1)//우
	{
		if (go[2] < 4.5)
		{
			go[2] += 0.1;
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
	else if (coin_input != 0 && stickop[4] == 1)//push
	{
		if (go[0] > -1)
		{
			//가장 왼쪽 아래
			if (go[1] > -0.1 && go[1] <= 0.8 && go[2] >= -0.3 && go[2] < -0.1)
			{
				printf("pushed down\n");
				pushed[4][1] = 1;
			}
			else if (go[1] > -0.1 && go[1] <= 1.0 && go[2] >= -0.5 && go[2] < -0.3)
			{
				printf("pushed dleft\n");
				pushed[4][0] = 1;
			}
			else if (go[1] > -0.1 && go[1] <= 1.0 && go[2] >= -0.1 && go[2] <= 0.0)
			{
				printf("pushed dright\n");
				pushed[4][2] = 1;
			}
			else if (go[1] > 0.8 && go[1] <= 1.0 && go[2] > -0.3 && go[2] < -0.1)
			{
				printf("pushed dup\n");
				pushed[4][3] = 1;
			}
			//가운데 아래
			if (go[1] > 0 && go[1] <= 0.4 && go[2] >= 1.8 && go[2] < 2.1)
			{
				printf("pushed mdown\n");
				pushed[5][1] = 1;
			}
			else if (go[1] > 0 && go[1] <= 0.8 && go[2] >= 1.5 && go[2] < 1.8)
			{
				printf("pushed mleft\n");

				pushed[5][0] = 1;
			}
			else if (go[1] >  0 && go[1] <= 0.8 && go[2] >= 2.1 && go[2] <= 2.3)
			{
				printf("pushed mdright\n");
				pushed[5][2] = 1;
			}

			else if (go[1] > 0.4 && go[1] <= 0.8 && go[2] > 1.8 && go[2] < 2.1)
			{
				printf("pushed mdup\n");
				pushed[5][3] = 1;
			}
			//가장 왼쪽 위
			if (go[1] > 2.3 && go[1] <= 3.0 && go[2] >= -0.3 && go[2] < 0.0)
			{
				printf("pushed lup\n");
				pushed[1][1] = 1;
			}
			else if (go[1] > 2.3 && go[1] <= 3.4 && go[2] >= -0.5 && go[2] < -0.3)
			{
				printf("pushed luleft\n");

				pushed[1][0] = 1;
			}
			else if (go[1] > 2.3 && go[1] <= 3.4 && go[2] >= 0 && go[2] <= 0.2)
			{
				printf("pushed ludright\n");
				pushed[1][2] = 1;
			}

			else if (go[1] > 3 && go[1] <= 3.4 && go[2] > -0.3 && go[2] < 0)
			{
				printf("pushed ludup\n");
				pushed[1][3] = 1;
			}
			
			//가운데 위
			if (go[1] > 2.4 && go[1] <= 3.0 && go[2] >= 1.8 && go[2] < 2.1)
			{
				printf("pushed mup\n");
				pushed[2][1] = 1;
			}
			else if (go[1] > 2.4 && go[1] <= 3.6 && go[2] >= 1.5 && go[2] < 1.8)
			{
				printf("pushed muleft\n");

				pushed[2][0] = 1;
			}
			else if (go[1] > 2.4 && go[1] <= 3.6 && go[2] >= 2.1 && go[2] <= 2.4)
			{
				printf("pushed muright\n");
				pushed[2][2] = 1;
			}

			else if (go[1] > 3 && go[1] <= 3.6 && go[2] > 1.8 && go[2] < 2.1)
			{
				printf("pushed muup\n");
				pushed[2][3] = 1;
			}
			//가장 오른쪽 아래
			if (go[1] > 0.0 && go[1] <= 0.8 && go[2] >= 3.7 && go[2] < 4.0)
			{
				printf("pushed rdown\n");
				pushed[6][1] = 1;
			}
			else if (go[1] > 0.0 && go[1] <= 1.2 && go[2] >= 3.5 && go[2] < 3.7)
			{
				printf("pushed rdleft\n");

				pushed[6][0] = 1;
			}
			else if (go[1] > 0.0 && go[1] <= 1.2 && go[2] >= 4.0 && go[2] <= 4.4)
			{
				printf("pushed rdright\n");
				pushed[6][2] = 1;
			}

			else if (go[1] > 0.8 && go[1] <= 1.2 && go[2] > 3.7 && go[2] < 4.0)
			{
				printf("pushed rdup\n");
				pushed[6][3] = 1;
			}
			//가장 오른쪽 위
			if (go[1] > 2.5 && go[1] <= 3.0 && go[2] >= 4.1 && go[2] < 4.3)
			{
				printf("pushed rup\n");
				pushed[3][1] = 1;
			}
			else if (go[1] > 2.5 && go[1] <= 3.6 && go[2] >= 3.9 && go[2] < 4.1)
			{
				printf("pushed ruleft\n");

				pushed[3][0] = 1;
			}
			else if (go[1] > 2.5 && go[1] <= 3.6 && go[2] >= 4.3 && go[2] <= 4.5)
			{
				printf("pushed ruright\n");
				pushed[3][2] = 1;
			}

			else if (go[1] > 3 && go[1] <= 3.6 && go[2] > 4.1 && go[2] < 4.3)
			{
				printf("pushed ruup\n");
				pushed[3][3] = 1;
			}
			
			go[0] -= 1;
			coin_input=11;
		}
		stickop[4] = 0;
	}
	
	//glRotatef(90, 0, 0, 1);

	draw_obj(Stickbar);
	if (coin_input != 0)
		glTranslatef(0, 0, go[0]);
	draw_obj(Stick);

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	y = g_nGLHeight - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		cout << "mouse x y " << x <<" "<< y << endl;
		Picking(x, y);
		g_nX = x;
		g_nY = y;
		
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		g_nSelect = 0;
	}
	//glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		

		if (radius > 1) radius -= 0.5;
	}
	else {
		
		if (radius < 100) radius += 0.5;
	}
	printf("%f\n", radius);
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
		g_pPosition[(g_nSelect - 1) * 2 + 0] += (x - g_nX);
		g_pPosition[(g_nSelect - 1) * 2 + 1] += (y - g_nY);
		
		
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
		theta = 90;
		phi = 00;
		radius = 16;
	}
	else if (option == 1) {
		for (int k = 0; k < 7; k++)
		{
			for (int i = 0; i < 5; i++)
			{
				pushed[k][i] = 0;
				push_obj[k][i] = 0;
			}
		}
		
		dollposition[4] = { 1.77,0.63,0.47 };
		dollposition[5] = { 1.97, 0.63, 0.57 };
		dollposition[1] = { 4.17, 3.23, 0.57 };
		dollposition[2] = { 0.53,3.83,0.77 };
		dollposition[3] = { 2.83 ,3.23,0.47 };
		dollposition[6] = { 2.67, 1.23 , 0.77 };
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 7; j++)
				getdoll[i][j] = 0;
		}
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
	if (predoor >= 0.1 && predoor <= 80)
	{
		predoor += 0.1;
	}
	if (lock[0].a == 1) {
		
		if (lock[0].b < 90 && lock[1].a == 0)
			lock[0].b += 0.1;
		else if (lock[1].a == 1 && lock[0].b >= 0)
			lock[0].b -= 0.1;
	}
	
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
			push_obj[4][0] -= 0.5;
	
	}
	if (pushed[4][2] == 1)
	{
		if (push_obj[4][2] < 60)
			push_obj[4][2] += 0.5;

	}
	if (pushed[4][3] == 1)
	{
		if (push_obj[4][3] < 60)
			push_obj[4][3] += 0.5;

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
			push_obj[5][0] -= 0.5;

	}
	if (pushed[5][2] == 1)
	{
		if (push_obj[5][2] < 60)
			push_obj[5][2] += 0.5;

	}
	if (pushed[5][3] == 1)
	{
		if (push_obj[5][3] < 60)
			push_obj[5][3] += 0.5;

	}
	//////////////////////////////////////////////////////////////////////
	if (pushed[1][1] == 1)
	{
		if (push_obj[1][1] > -1.5)
			push_obj[1][1] -= 0.01;
		else if (push_obj[1][1] <= -1.5)
			pushed[1][4] = 1;
	}
	if (pushed[1][0] == 1)
	{
		if (push_obj[1][0] > -10)
			push_obj[1][0] -= 0.1;

	}
	if (pushed[1][2] == 1)
	{
		if (push_obj[1][2] < 6)
			push_obj[1][2] += 0.1;

	}
	if (pushed[1][3] == 1)
	{
		if (push_obj[1][3] < 60)
			push_obj[1][3] += 0.5;

	}
	//////////////////////////////////////////////
	if (pushed[2][1] == 1)
	{
		if (push_obj[2][1] > -1.5)
			push_obj[2][1] -= 0.01;
		else if (push_obj[2][1] <= -1.5)
			pushed[2][4] = 1;
	}
	if (pushed[2][0] == 1)
	{
		if (push_obj[2][0] > -45)
			push_obj[2][0] -= 0.5;

	}
	if (pushed[2][2] == 1)
	{
		if (push_obj[2][2] < 60)
			push_obj[2][2] += 0.5;

	}
	if (pushed[2][3] == 1)
	{
		if (push_obj[2][3] < 45)
			push_obj[2][3] += 0.5;

	}
	/////////////////////////////////////////////////////
	if (pushed[6][1] == 1)
	{
		if (push_obj[6][1] > -1.5)
			push_obj[6][1] -= 0.01;
		else if (push_obj[6][1] <= -1.5)
			pushed[6][4] = 1;
	}
	if (pushed[6][0] == 1)
	{
		if (push_obj[6][0] > -45)
			push_obj[6][0] -= 0.5;

	}
	if (pushed[6][2] == 1)
	{
		if (push_obj[6][2] < 60)
			push_obj[6][2] += 0.5;

	}
	if (pushed[6][3] == 1)
	{
		if (push_obj[6][3] < 45)
			push_obj[6][3] += 0.5;

	}
	/////////////////////////////////////////////////////
	if (pushed[3][1] == 1)
	{
		if (push_obj[3][1] > -1.5)
			push_obj[3][1] -= 0.01;
		else if (push_obj[3][1] <= -1.5)
			pushed[3][4] = 1;
	}
	if (pushed[3][0] == 1)
	{
		if (push_obj[3][0] > -60)
			push_obj[3][0] -= 0.5;

	}
	if (pushed[3][2] == 1)
	{
		if (push_obj[3][2] < 60)
			push_obj[3][2] += 0.5;

	}
	if (pushed[3][3] == 1)
	{
		if (push_obj[3][3] < 45)
			push_obj[3][3] += 0.1;
	}
}

void resize(int width, int height)
{
	g_nGLWidth = width;
	g_nGLHeight = height;
	printf("resize func called\n");
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 500, 0, 500);
	gluPerspective(per[0], per[1], per[2], per[3]);
	

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
	glVertex3f(10.f, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 1.f, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 5.f);

	glEnd();
	glLineWidth(1);
}