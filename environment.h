#pragma once
#pragma warning (disable : 4996)
#include <iostream>
#include <vector>
#include <string>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include "GL/glext.h"
#include "GL/glcorearb.h"
#include "bmpfuncs.h"

class environment {
public:
    GLuint texName[6];
    GLuint g_nCubeTex[3];
    uchar* img0[3];
    uchar* img1[3];
    uchar* img2[3];
    uchar* img3[3];
    uchar* img4[3];
    uchar* img5[3];

	void envset() {
        glGenTextures(1, &g_nCubeTex[0]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex[0]);
        int imgWidth[3][6], imgHeight[3][6], channels[3][6];

        img0[0] = readImageData("img/environment/pattern.bmp", &imgWidth[0][0], &imgHeight[0][0], &channels[0][0]);
        img1[0] = readImageData("img/environment/pattern.bmp", &imgWidth[0][1], &imgHeight[0][1], &channels[0][1]);
        img2[0] = readImageData("img/environment/pattern.bmp", &imgWidth[0][2], &imgHeight[0][2], &channels[0][2]);
        img3[0] = readImageData("img/environment/wood.bmp", &imgWidth[0][3], &imgHeight[0][3], &channels[0][3]);
        img4[0] = readImageData("img/environment/game.bmp", &imgWidth[0][4], &imgHeight[0][4], &channels[0][4]);
        img5[0] = readImageData("img/environment/game.bmp", &imgWidth[0][5], &imgHeight[0][5], &channels[0][5]);



        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth[0][0], imgHeight[0][0], 0, GL_RGB, GL_UNSIGNED_BYTE, img0[0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth[0][1], imgHeight[0][1], 0, GL_RGB, GL_UNSIGNED_BYTE, img1[0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth[0][2], imgHeight[0][2], 0, GL_RGB, GL_UNSIGNED_BYTE, img2[0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth[0][3], imgHeight[0][3], 0, GL_RGB, GL_UNSIGNED_BYTE, img3[0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth[0][4], imgHeight[0][4], 0, GL_RGB, GL_UNSIGNED_BYTE, img4[0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth[0][5], imgHeight[0][5], 0, GL_RGB, GL_UNSIGNED_BYTE, img5[0]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex[0]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

        //////////////////////////////////////////////////////////////////////////////////
        glGenTextures(1, &g_nCubeTex[1]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex[1]);
       

        img0[1] = readImageData("img/environment/front.bmp", &imgWidth[1][0], &imgHeight[1][0], &channels[1][0]);
        img1[1] = readImageData("img/environment/back.bmp", &imgWidth[1][1], &imgHeight[1][1], &channels[1][1]);
        img2[1] = readImageData("img/environment/top.bmp", &imgWidth[1][2], &imgHeight[1][2], &channels[1][2]);
        img3[1] = readImageData("img/environment/bottom.bmp", &imgWidth[1][3], &imgHeight[1][3], &channels[1][3]);
        img4[1] = readImageData("img/environment/right.bmp", &imgWidth[1][4], &imgHeight[1][4], &channels[1][4]);
        img5[1] = readImageData("img/environment/left.bmp", &imgWidth[1][5], &imgHeight[1][5], &channels[1][5]);



        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth[1][0], imgHeight[1][0], 0, GL_RGB, GL_UNSIGNED_BYTE, img0[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth[1][1], imgHeight[1][1], 0, GL_RGB, GL_UNSIGNED_BYTE, img1[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth[1][2], imgHeight[1][2], 0, GL_RGB, GL_UNSIGNED_BYTE, img2[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth[1][3], imgHeight[1][3], 0, GL_RGB, GL_UNSIGNED_BYTE, img3[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth[1][4], imgHeight[1][4], 0, GL_RGB, GL_UNSIGNED_BYTE, img4[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth[1][5], imgHeight[1][5], 0, GL_RGB, GL_UNSIGNED_BYTE, img5[1]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex[1]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

        glLoadIdentity();
	}
    void draw_skybox(float size,int i) {
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex[i]);

        glBegin(GL_QUADS);
        glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
        glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
        glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
        glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
        glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-size, -size, size);
        glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
        glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
        glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
        glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
        glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
        glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-size, -size, size);
        glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
        glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord3d(-1.0, -1.0, +1.0); glVertex3f(-size, -size, size);
        glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
        glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
        glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
        glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
        glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
        glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
        glEnd();

        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_LIGHTING);
    }
};
