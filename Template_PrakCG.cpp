#define _CRT_SECURE_NO_WARNINGS
#define __EXPORT_HELP

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "Include/freeglut.h"
#include "Include/glext.h"
#include "help.h"
#include "window.h"
#include "wavefront.h"
#include "input.h"
#include "light.h"
#include "image.h"



void setCamera ();		// Kamera platzieren, siehe Maus-Callbacks
void drawScene ();		// Zeichnet die Szene im Weltkoordinatensystem
void initVec();
void initObjs();
void initLight();
void initDisplayLists();
void initTexture();
cg_object3D g_obj;
cg_object3D toyTunnel;
cg_object3D train;
cg_object3D wheel;
short whichLight=0;
short whichCam=0;

//Aus Wavefront.cpp behebt Exportbug
inline float* normieren (float v[3] )
{
	float l = 1.0f / sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
	v[0] *= l;
	v[1] *= l;
	v[2] *= l;
	return v;
}


/////////////////////////////////////////////////////////////////////////////////
//	Kamerafunktion
/////////////////////////////////////////////////////////////////////////////////

bool cameraAtTrain=false;
static float coord[]={0,0,0};
static int vecNumber=0;
const float vecs[][3]={
	{20.0,0.0,0.0},
    {1.0,1.0,0},
    {0.0,3.0,0},
    {-1.0,1,0.0},
    {-20.0,0.0,0.0},
    {-1.0,-1.0,0.0},
    {0.0,-3.0,0},
    {1.0,-1.0,0.0},
    };
static float vecsN[8][3];
float vecsPos[8][3];

float distance = 0;
int vecsSize=8;
void setCamera ()
{
    static float camCoord[]={10,0,10};
    cg_key key;
	cg_mouse mouse;
	// Ansichtstransformationen setzen,
	// SetCamera() zum Beginn der Zeichenfunktion aufrufen
	double x, y, z, The, Phi;
	static double radius = 45;
	// Maus abfragen
	if ( cg_mouse::buttonState( GLUT_LEFT_BUTTON ) )
	{
		cg_globState::cameraHelper[0] += mouse.moveX();
		cg_globState::cameraHelper[1] += mouse.moveY();
	}
	if ( cg_mouse::buttonState( GLUT_MIDDLE_BUTTON ) )
	{
		radius += 0.1 * mouse.moveY();
		if ( radius < 6.0 ) radius = 6.0;
	}
	if(key.keyState('+')>0){
            camCoord[0]+=0.1;
    }
    else if( key.keyState('*')>0){
        camCoord[1]+=0.1;
    }
    else if(key.keyState('-')>0){
            camCoord[0]-=0.1;
	}
	else if(key.keyState('_')>0){
          camCoord[1]-=0.1;
    }
	Phi =  cg_globState::cameraHelper[0] / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The =  cg_globState::cameraHelper[1] / cg_globState::screenSize[1] * M_PI;
	x = radius * cos ( Phi ) * cos ( The );
	z = radius * sin ( The );
	y = radius * sin ( Phi ) * cos ( The );
	int Oben = ( The <= 0.5 * M_PI || The > 1.5 * M_PI ) * 2 - 1;
	if(whichCam==0){
	gluLookAt ( x+11, y+2.5, 10, 11, 2.5, 0, 0, 0, 1 );
	}else if(whichCam==1){
        //gluLookAt(5,-25,25,11,0,0,0,1,0);
        gluLookAt(15,-1.5,0.5,0,5,1,0,0,1);
    }else if(whichCam==2){
        gluLookAt(coord[0],coord[1],coord[2]+1,coord[0]+vecsN[vecNumber][0],coord[1]+vecsN[vecNumber][1],coord[2]+vecsN[vecNumber][2]+1,0,0,1);
    } else{
        gluLookAt(camCoord[0],camCoord[1],20,camCoord[0],camCoord[1],0,0,1,0);
    }
}



/////////////////////////////////////////////////////////////////////////////////
//	Anfang des OpenGL Programmes
/////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char **argv )
{
	init( argc, argv );
    initVec();
    initObjs();
    initLight();
    initTexture();
    initDisplayLists();
	// hier Objekte laden, erstellen etc.
	// ...

	// Die Hauptschleife starten
	glutMainLoop ();
	return 0;
}



void displayFunc ()
{
	// Hilfe-Instanzen
	cg_help help;
	cg_globState globState;
	cg_key key;

	// Tastatur abfragen
	// Achtung: einmaliges Betätigen funktioniert so nur mit glutIgnoreKeyRepeat(true) (siehe main())
	if ( key.keyState( 27 ) )
	{
		exit ( 0 ); // Escape -> Programm beenden
	}
	else if ( 1 == key.keyState( 'f' ) || 1 == key.keyState( 'F' ) )
	{
		help.toggleFps();	// Framecounter on/off
	}
	else if ( 1 == key.keyState( 'h' ) || 1 == key.keyState( 'H' ) || 1 == key.specialKeyState( GLUT_KEY_F1 ) )
	{
		help.toggle();	// Hilfetext on/off
	}
	else if ( 1 == key.keyState( 'k' ) || 1 == key.keyState( 'K' ) )
	{
		help.toggleKoordsystem();	// Koordinatensystem on/off
	}
	else if ( 1 == key.keyState( 'w' ) || 1 == key.keyState( 'W' ) )
	{
		globState.drawMode = ( globState.drawMode == GL_FILL ) ? GL_LINE : GL_FILL; // Wireframe on/off
	}
	else if ( 1 == key.keyState( 'l' ) || 1 == key.keyState( 'L' ) )
	{
		globState.lightMode = !globState.lightMode;	// Beleuchtung on/off
	}
	else if ( 1 == key.keyState( 'c' ) || 1 == key.keyState( 'C' ) )
	{
		globState.cullMode = !globState.cullMode; // Backfaceculling on/off
	}
	else if ( 1 == key.keyState( 'i' ) || 1 == key.keyState( 'I' ) )
	{
		globState.cameraHelper[0] = 0;	// Initialisierung der Kamera
		globState.cameraHelper[1] = 0;
	}else if ( key.keyState('z')==1|| key.keyState('Z')==1){
        whichCam=(whichCam+1)%4;
	} else if(key.keyState('m')==1|| key.keyState('M')==1){
            whichLight=(whichLight+1)%3;
	}


	// Szene zeichnen: CLEAR, SETCAMERA, DRAW_SCENE

	// Back-Buffer neu initialisieren
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();

	// Kamera setzen (spherische Mausnavigation)
	setCamera();

	// Koordinatensystem zeichnen
	help.drawKoordsystem ( -8, 10, -8, 10, -8, 10 );

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode( GL_FRONT_AND_BACK, globState.drawMode );

	// Backface Culling on/off);
	glFrontFace( GL_CCW );
	glCullFace( GL_BACK );
	if ( globState.cullMode ) glEnable( GL_CULL_FACE );
	else glDisable( GL_CULL_FACE );

	// Farbmodus oder Beleuchtungsmodus ?
	if ( globState.lightMode == GL_TRUE ) // Beleuchtung aktivieren
	{
		float m_amb[4] = {0.2, 0.2, 0.2, 1.0};
		float m_diff[4] = {0.2, 0.2, 0.6, 1.0};
		float m_spec[4] = {0.8, 0.8, 0.8, 1.0};
		float m_shine = 32.0;
		float m_emiss[4] = {0.0, 0.0, 0.0, 1.0};

		SetMaterial( GL_FRONT_AND_BACK, m_amb, m_diff, m_spec, m_shine, m_emiss );

		//SetLights();

		glEnable( GL_LIGHTING );
	}
	else   // Zeichnen im Farbmodus
	{
		glDisable( GL_LIGHTING );
		glColor4f( 0.2, 0.2, 0.6, 1.0 );
	}


	// Geometrie zeichnen /////////////////!!!!!!!!!!!!!!!!!!!!!!!!///////////////////////
    //glutSolidCube(1);
    drawScene();


	// Hilfetext zeichnen
	help.draw();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable( GL_CULL_FACE );

	glFlush ();				// Daten an Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}

float wheelRotation=0;

float calcSpeed(){
    cg_key key;
    static float posMomentum=1.01;
	static float negativMomentum=1.04;
	static float posSpeed=0;
	static float negativSpeed=0;
	float maxSpeed=0.3;
	if(key.specialKeyState(GLUT_KEY_RIGHT)>0){
        if(negativSpeed!=0){
            negativSpeed=negativSpeed*0.9*0.9;
        }
        if(posSpeed==0 && negativSpeed==0){
            posSpeed=0.01;
            printf("Start\n");
            wheelRotation+=1;
        }else{
            wheelRotation+=1;
            posSpeed=posSpeed*posMomentum;
        }
            if(wheelRotation==45){
                wheelRotation=0;
        }
	}
    else if(key.specialKeyState(GLUT_KEY_LEFT)>0){
        if(posSpeed!=0){
            posSpeed=posSpeed*0.9*0.9;
        }
        if(negativSpeed==0 && posSpeed==0){
            negativSpeed=0.01;
        }else{
            negativSpeed=negativSpeed*negativMomentum;
        }
	}
	else{
        if(posSpeed!=0){
            posSpeed=posSpeed*0.9;
        }
        if(negativSpeed!=0){
            negativSpeed=negativSpeed*0.9;
        }
	}
	if(posSpeed<0.001){
        posSpeed=0;
	}
	if(negativSpeed<0.001){
        negativSpeed=0;
	}
	if(posSpeed>maxSpeed){
        posSpeed=maxSpeed;
	}
	if(negativSpeed>maxSpeed){
        negativSpeed=maxSpeed;
	}
    return posSpeed-negativSpeed;
}

float vecLength(const float *vec){
    return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}




float calcCoord(float *coord, int vecNumber, float speed){
    float length = vecLength(vecs[vecNumber]);
    float *vecNormal = vecsN[vecNumber];
	//float speed = calcSpeed();
	if(speed<(length-distance)){
        coord[0]+=speed*vecNormal[0];
        coord[1]+=speed*vecNormal[1];
        coord[2]+=speed*vecNormal[2];
        distance = distance+speed;
        return 0;
    }else{
        coord[0]=vecsPos[(vecNumber+1)%vecsSize][0];
        coord[1]=vecsPos[(vecNumber+1)%vecsSize][1];
        coord[2]=vecsPos[(vecNumber+1)%vecsSize][2];
        speed-=(length-distance);
        distance=0;
        return speed;
    }

}

void initVec(){
    for(int i=0;i<vecsSize;i++){
        for(int j=0;j<3;j++){
            vecsN[i][j]=vecs[i][j];
            if(i==0){
                vecsPos[0][j]=0;
            }else{
                vecsPos[i][j]+=vecsPos[i-1][j]+vecs[i-1][j];
            }
        }
        normieren(vecsN[i]);
    }

}

void initObjs(){
g_obj.load("../objects/toyCube.obj", true);
toyTunnel.load("../objects/toyTunnel.obj", true);
train.load("../objects/toyTrain.obj",true);
wheel.load("../objects/trainWheel.obj",true);
wheel.setMaterial(0.1,0.1,0.8,1,0.8,0.3,0);
wheel.setPosition(0,0,0);
train.setMaterial(0.5,0.1,0.1,1,0.7,0.7,0);
g_obj.setPosition(0,0,10);
toyTunnel.setPosition(9,0,1);
g_obj.setMaterial(0.7,0.0,0.0,1.0,0.5,0.3,0.0);
toyTunnel.setMaterial(0.5,0.5,0,1.0,0.3,0.3,0.0);
}

GLuint path;
GLuint plane;

void initDisplayLists(){
    path = glGenLists(1);
    glNewList(path, GL_COMPILE);
        glEnable(GL_LINE_SMOOTH);
        glColor3f(1,0,0);
        glBegin(GL_LINE_LOOP);
            //glVertex3fv(coord);
            float point []={0,0,0};
            glNormal3f(0,0,1);
            for(int i=0;i<vecsSize;i++){
                glVertex3fv(vecsPos[i]);
                //glNormal3f(0,1,0);
            }
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    glEndList();


    plane = glGenLists(1);
    glNewList(plane, GL_COMPILE);
            float diff[]={0.1,0.7,0.1,1};
            glMaterialfv(GL_FRONT,GL_DIFFUSE,diff);
            glColor3f(0.1,0.7,0.1);
            glNormal3f(0,0,1);
                float zPos=-0.1;
                int totalHeigth=120;
                int totalWidth=120;
                float xstart = -50;
                float ystart = -57.5;
                float width = 0.1;
                float height = 0.1;
                int rows=totalHeigth/height;
                int columns=totalWidth/width;
                for(int i=0; i<rows;i++){

                    glBegin(GL_QUAD_STRIP);
                    for(int j=0;j<columns;j++){
                        glVertex3f(xstart+width*i,ystart+height*j,zPos);
                        glVertex3f(xstart+width*(i+1),ystart+height*j,zPos);
                    }
                    glEnd();
                }
               /* glBegin(GL_QUAD_STRIP);
                    glVertex3f(-70,-70,-0.1);
                    glVertex3f(-67,-70,-0.1);
                    glVertex3f(-70,-67,-0.1);
                    glVertex3f(-67,-67,-0.1);
                    glVertex3f(-70,-64,-0.1);
                    glVertex3f(-67,-64,-0.1);
                glEnd();*/
               /* glVertex3f(-70,-70,-0.1);
                //glNormal3f(0,0,1);
                glVertex3f(-70,70,-0.1);
                //glNormal3f(0,0,1);
                glVertex3f(70,70,-0.1);
                //glNormal3f(0,0,1);
                glVertex3f(70,-70,-0.1);
               */ //glNormal3f(0,0,1);
    glEndList();



}

float angleBetween2Normals(float *v1,float *v2){
    return cos((v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])/(vecLength(v1)*vecLength(v2)));
}

void initLight(){
    GLfloat position2[]={0,3,0,1};
    GLfloat position[]={11,2.5,1,1};
    GLfloat amb[]={0,0,0,1};
    GLfloat diff[]={0.8,0.1,0.1,1};
    GLfloat spec[]={0.8,0.1,0.1,1};
    GLfloat spec2[]={1,1,1,1};
    GLfloat diff2[]={0.5,0.5,0,1};
    //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1)
    SetLights();

    glLightfv(GL_LIGHT1,GL_POSITION,position);
    glLightfv(GL_LIGHT1, GL_AMBIENT,amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR,spec);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 2);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_AMBIENT,amb);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,diff2);
    glLightfv(GL_LIGHT2, GL_SPECULAR,spec2);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,2);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,20);
    //glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2);

}

cg_image test;
void initTexture(){
    test.load("../textures/mahagoni-holz_02.bmp");
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

void drawScene ()
{
    if(whichLight==0){
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
    }else if(whichLight==1){
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT2);
    }else if (whichLight==2){
        glEnable(GL_LIGHT2);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }
    glEnable(GL_MULTISAMPLE);
	// Zeichnet die Szene 1x im Weltkoordinatensystem
    //glTranslatef(-10,-2.5,0);
    static int angle=0;
    float speed= calcSpeed();
    static int prevVecNumber=0;
    while(speed>0){
        speed = calcCoord(coord,vecNumber, speed);
        if(distance==0&&speed>0){
            vecNumber = (vecNumber+1)%vecsSize;
        }
	}
        glPushMatrix();
            glTranslatef(10,-9.5,0);
            glRotatef(90,0,0,1);
            toyTunnel.draw();
        glPopMatrix();
        g_obj.draw();
        glPushMatrix();
            glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
            glTranslatef(-5,0,1);
            //glColorMaterial(GL_FRONT, GL_EMISSION);
            glColor3f(0.1,0.5,0.6);
            glutSolidCube(1);
            glColor3f(0,0,0);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.1,0.6,0.1);
            glTranslatef(-5,5,1);
            glutSolidSphere(1,20,20);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.6,0.1,0.1);
            glTranslatef(26,0,1);
            glutSolidCone(1,2,10,10);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.1,0.1,0.8);
            glTranslatef(26,5,1);
            glScalef(2,2,2);
            glBegin(GL_TRIANGLES);
                glNormal3f(-0.707107, -0.707107, 0);
                glVertex3f(0,0,0);
                glNormal3f(-0.707107, 0.707107, 0);
                glVertex3f(0,1,0);
                glNormal3f(0.707107, -0.707107, 0);
                glVertex3f(1,0,0);
                glNormal3f(0.707107, -0.707107, 0);
                glVertex3f(1,0,0);
                glNormal3f(-0.707107, 0.707107, 0);
                glVertex3f(0,1,0);
                glNormal3f(0.707107, 0.707107, 0);
                glVertex3f(1,1,0);

                glNormal3f(0.707107, 0.707107, 0);
                glVertex3f(1,1,0);
                glNormal3f(-0.707107, 0.707107, 0);
                glVertex3f(0,1,0);
                glNormal3f(0,0,1);
                glVertex3f(0.5,0.5,1);

                glNormal3f(-0.707107, 0.707107, 0);
                glVertex3f(0,1,0);
                glNormal3f(-0.707107, -0.707107, 0);
                glVertex3f(0,0,0);
                glNormal3f(0, 0, 1);
                glVertex3f(0.5,0.5,1);

                glNormal3f(-0.707107, -0.707107, 0);
                glVertex3f(0,0,0);
                glNormal3f(0.707107, -0.707107, 0);
                glVertex3f(1,0,0);
                glNormal3f(0,0,1);
                glVertex3f(0.5,0.5,1);

                glNormal3f(0.707107, -0.707107, 0);
                glVertex3f(1,0,0);
                glNormal3f(0.707107, 0.707107, 0);
                glVertex3f(1,1,0);
                glNormal3f(0,0,1);
                glVertex3f(0.5,0.5,1);
            glEnd();
        glPopMatrix();
	glPushMatrix();
        glTranslatef(coord[0],coord[1],coord[2]);
        if(prevVecNumber!=vecNumber){
            angle=(angle+45)%360;
        }
        //printf("%f\n",angle);
        glRotatef(angle,0,0,1);
        glEnable(GL_COLOR_MATERIAL);


            glPushMatrix();
                glScalef(0.3,0.3,0.3);
                glTranslatef(0,0,1);
                glRotatef(90,1,0,0);
                //gluLookAt(0,0,0,vecsN[vecNumber][0],vecsN[vecNumber][1],vecsN[vecNumber][2],0,0,1);
                //gluLookAt(0,0,0,-1,0,0,0,0,1);
                GLfloat pos[]={1,0,0,1};
                glLightfv(GL_LIGHT2,GL_POSITION,pos);
                GLfloat direction[]={1,0,0};
                glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION,direction);
                GLfloat color[]={0.1,0.1,0.7};
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,color);
                //glutSolidTeapot(0.7);
                 if(whichCam!=2){
                glEnable( GL_TEXTURE_2D );
                test.bind();
                glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
                glTexParameteri ( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
                train.draw();
                glDisable(GL_TEXTURE_2D);
                    glRotatef(90,1,0,0);
                    glColor3f(0.1,0.1,0.7);
                    glPushMatrix();
                        glTranslatef(1,1,0);
                        glRotatef(-wheelRotation,0,1,0);
                        wheel.draw();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-1,1,0);
                        glRotatef(-wheelRotation,0,1,0);
                        wheel.draw();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-1,-1,0);
                        glRotatef(-wheelRotation,0,1,0);
                        wheel.draw();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(1,-1,0);
                        glRotatef(-wheelRotation,0,1,0);
                        wheel.draw();
                    glPopMatrix();
                 }
            glPopMatrix();
        prevVecNumber=vecNumber;
	glPopMatrix();
        glCallList(plane);
        glCallList(path);
        //Render Pfad

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_DST_ALPHA, GL_ONE);

        glPushMatrix();
            glTranslatef(5,2.5,0);
            glDisable(GL_CULL_FACE);
            glPushMatrix();
                glColor3f(0.45,0.05,0.95);
                glBegin(GL_TRIANGLES);
                    glNormal3f(-1,0,0);
                    glVertex3f(0,0,0);
                    glVertex3f(6,0,0);
                    glVertex3f(3,0,6);
                glEnd();
            glPopMatrix();

            glPushMatrix();
                glColor3f(0.2,0.8,0.24);
                glTranslatef(0,1,0);
                glBegin(GL_TRIANGLES);
                    glNormal3f(-1,0,0);
                    glVertex3f(3,0,0);
                    glVertex3f(0,0,6);
                    glVertex3f(6,0,6);
                glEnd();
            glPopMatrix();
            glEnable(GL_CULL_FACE);
        glPopMatrix();
        glDepthMask(GL_TRUE);

        glDisable(GL_BLEND);


}



