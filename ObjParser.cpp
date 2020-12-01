#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string.h>
#include "ObjParser.h"
#include "bmpfuncs.h"
#include "environment.h"
#include <iostream>

using namespace std;

environment env;

#define M_PI 3.1415926535897
#define DIST 10
using std::cos;
using std::sin;
using std::sqrt;

// global variable for counting fps
//volatile int frame = 0, time, timebase = 0;
volatile float fps;

/* texture mapping set variable */
int game_step[3][3] = { 0 };
GLuint textureMonkey;
int game_mode = 0;//game:1 idle:0
int coin_input = 0; //0:coin 안들어감 1:coin들어감
float go[3] = { 0 };//x,y,z 좌표 이동하는 정도
int joy[5] = { 0 };//joystick의 상태 좌우상하 push
int stickop[5] = { 0 };//joystick에 따라 stick의 움직임
bool antialiase_on = true;
double radius = 10;
double theta = 45, phi = 45;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };
///////////////////////////////
char dp[50];
char cider[50];
char coke[50];
char present1[50];
char present2[50];
char present3[50];
char joystickboard[50];
char joystick[50];
char pushbutton[50];
char stick[50];
char coin[50];
///////////////////////////
 //object var
ObjParser* Dp;
ObjParser* Cider;
ObjParser* Coke;
ObjParser* Present1;
ObjParser* Present2;
ObjParser* Present3;
ObjParser* Joystickboard;
ObjParser* Joystick;
ObjParser* Joystick_ball;
ObjParser* Pushbutton;
ObjParser* Stick;
ObjParser* Machine;
ObjParser* Timer;
ObjParser* Coin;
ObjParser* Coinmachine;

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
void draw_obj_with_texture(ObjParser* objParser, char buf[50]);
void idle();
//...
void MyTimer(int value)
{
	coin_input++;
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
	glColor3f(red, green, blue);
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
		coin_input = 0;
		break;
	default:
		str = (char*)"READY";
		break;
	}
	

	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -0.2, 0.87, 1, 1, 0);
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

void light_default() {
	glClearColor(1.f, 1.f, 1.f, 1.0f);

	/* Light0 조명 관련 설정 */
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.9f, 1.0f };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/* Light1 조명 관련 설정 */
	GLfloat ambientLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuseLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light_position1[] = { 5.0, 5.0, 5.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	/************* spot position setting *************/
	/*GLfloat spot_direction[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/

	GLfloat specularMaterial[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/************* Material  setting *************/
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);


	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	/* DEPTH TEST ENABLE */
	glFrontFace(GL_CW);	// CW CCW바꿔보면서 front face 변경해보기!

	
}

void setTextureMapping(char buf[100]) {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData(buf, &imgWidth, &imgHeight, &channels);

	int texNum = 1;
	glGenTextures(texNum, &textureMonkey);
	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
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
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0, 500, 500, 0);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	

	light_default();
	
	/* TEXTURE MAPPING SET */

	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_REPLACE : polygon의 원래 색상은 무시하고 texture로 덮음
	
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
				theta += 5; //printf("theta %Lf\n", theta);
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
	}

	glutPostRedisplay();
}





/* Display callback function */
void draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glEnable(GL_DEPTH_TEST);

	

	cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
	cam[1] = radius * cos(theta * M_PI / 180);
	cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
	glDisable(GL_LIGHT1);
	gluLookAt(0, -4, 5, 0, -4, 0, 0, 1, 0);

	if ((0 <= theta && theta < M_PI / 2) || (3 * M_PI / 2 < theta && theta <= 2 * M_PI)) {
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, -1, 0);
	}
	else {
		gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);
	}
	//gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	if (game_mode == 1)
	{
		phi = 0; theta = 95; radius = 1;
		gluLookAt(3.4, 0.3, 0, 0, 0, 0, 0, 1, 0);
		glTranslatef(0, -4, 0);
		
		
		if (coin_input != 0)
		{

			draw_obj_with_texture(Coin, coin);
			draw_text();
		}
	
		
	}
	//printf("%Lf %Lf %Lf %d %d %d\n", cam[0], cam[1], cam[2], center[0], center[1], center[2]);

	env.draw_skybox(60);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(0.5,0.5,0.2, 1);	 //색깔바꾸기
	draw_obj (Machine);
	
	draw_obj(Coinmachine);
	draw_obj(Timer);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	
	//glPopMatrix();
	
	//dprack();


	glPushMatrix();                                                    


	dprack();
	glPopMatrix();
	
	
	joystick_op();
	glPopMatrix();
	pushstickop();

	glEnable(GL_LIGHTING);
	glColor4f(1, 1, 1, 1);	 //색깔바꾸기
	glEnable(GL_LIGHTING);
	
	glFlush();
	glutSwapBuffers();
	
}



void set_obj()
{
	
	Dp = new ObjParser("img/obj/dp.obj");
	Cider = new ObjParser("img/obj/cider.obj");
	Coke = new ObjParser("img/obj/cider.obj");
	Present1 = new ObjParser("img/obj/present.obj");
	Present2 = new ObjParser("img/obj/present.obj");
	Joystickboard = new ObjParser("img/obj/joystickboard.obj");
	Joystick = new ObjParser("img/obj/joystick.obj");
	Joystick_ball = new ObjParser("img/obj/joystick_ball.obj");
	Pushbutton = new ObjParser("img/obj/pushbutton.obj");
	Stick = new ObjParser("img/obj/stick.obj");
	Machine = new ObjParser("img/obj/machine.obj");
	Coinmachine = new ObjParser("img/obj/coinmachine.obj");
	Coin = new ObjParser("img/obj/coin.obj");
	Timer = new ObjParser("img/obj/timer.obj");

	//strcpy(dp, "img/obj/displayrack_r.bmp");
	//strcpy(cider, "img/obj/Cider.bmp");
	//strcpy(coke, "img/obj/Coke.bmp");
	strcpy(present1, "img/obj/present11.bmp");
	strcpy(present2, "img/obj/present2.bmp");
	
	//strcpy(joystickboard, "img/obj/joystickboard.bmp");
	strcpy(joystick, "img/obj/joystick_ball.bmp");
	//strcpy(pushbutton, "img/obj/pushbutton1.bmp");
	//strcpy(stick, "img/obj/machine.bmp");
	strcpy(coin, "img/obj/coin.bmp");
}
void dprack() {

	glColor3f(1.f, 1.f, 1.f);
	draw_axis();
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glColor4f(1, 0, 0, 1);	 //색깔바꾸기
	draw_obj(Dp);//(0,0,0)
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	
	glPushMatrix();
	
	
	draw_obj_with_texture(Present1, present1);
	//draw_obj(Present1);
	//glPopMatrix();

	//draw_obj_with_texture(Cider, cider);
	draw_obj(Cider);
	//glPopMatrix();

	glTranslatef(0, -2.3, 0);
	//draw_obj_with_texture(Coke, coke);
	draw_obj(Coke);
	//glPopMatrix();

	draw_obj_with_texture(Present2, present2);
	//draw_obj(Present2);

}
void joystick_op()
{
	//glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(0,1,0,1);	 //색깔바꾸기
	draw_obj(Joystickboard);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	//draw_obj_with_texture(Joystickboard,joystick);
	
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
	draw_obj_with_texture(Joystick_ball,joystick);
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
	draw_obj(Pushbutton);
	glEnable(GL_TEXTURE_2D);

	
	


	//glutPostRedisplay();
}

void pushstickop() {
	
	glTranslatef(go[0], go[1], go[2]);
	if (stickop[0] == 1)//좌
	{
		if (go[2] < 1)
		{
			go[2] += 0.3;
		}
		stickop[0] = 0;

	}
	else if (stickop[1] == 1)//우
	{
		if (go[2] > -5)
		{
			go[2] -= 0.3;
		}
		stickop[1] = 0;
	}
	else if (stickop[2] == 1)//위
	{
		if (go[1] < 4)
		{
			go[1] += 0.3;
		}
		stickop[2] = 0;
	}
	else if (stickop[3] == 1)//아래
	{
		if (go[1] > 0)
		{
			go[1] -= 0.3;
		}
		stickop[3] = 0;
	}
	else if (stickop[4] == 1)//push
	{
		if (go[0] > -1)
		{
			go[0] -= 1;
		}
		stickop[4] = 0;
	}
	
	//glRotatef(90, 0, 0, 1);

	draw_obj(Stick);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (state) {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	else {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
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
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void main_menu(int option)
{
	if (option == 99) exit(0);
	else if (option == 10) {
		game_mode = 1;
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

void draw_obj_with_texture(ObjParser* objParser, char buf[50])
{

	setTextureMapping(buf);
	glDisable(GL_BLEND);
	// glEnable(GL_TEXTURE_2D);	// texture 색 보존을 위한 enable
	glBindTexture(GL_TEXTURE_2D, textureMonkey);
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