#include <GL/glut.h>
#include <math.h>

using namespace std;

int width = 1280, height = 720;			//dimensioni finestra//									//DEFAULT: 1280, 720//
float posX = 1, posY = 1, lookX = 0, lookY = 0;	//parametri di posizione e vista (lookAt)//		//DEFAULT: 0, 0, 0, 0//
float angle = 0, pgreco = 3.14;			//angolo di rotazione del giocatore//			//DEFAULT: 0, 3.14//
float rotationSpeed = 2, runSpeed = 0.2;	//velocità di rotazione e corsa del giocatore//		//DEFAULT: 2, 0.3//
float lookAngle = 60, zNear = 1.0, zFar = 40;	//angolo di visuale, distanze (min e max) di vista//	//DEFAULT: 60, 1, 40//

void labyrinth(){

	glColor3f(1, 0, 0);
	glutSolidCube(2);
	glTranslatef(0, 2, 0);
	glColor3f(0, 1, 0);
	glutSolidCube(2);
	glTranslatef(2, 0, 0);
	glColor3f(0, 0, 1);
	glutSolidCube(2);
	glTranslatef(0, -2, 0);
	glColor3f(1, 0.35, 0);
	glutSolidCube(2);

}

void updatePos(int direction){
	//direction = 1 muove in AVANTI di distanza pari a "runSpeed"//
	//direction = -1 muove INDIETRO di distanza pari a "runSpeed"//

	if (posX==0||posY==0)return;
	posX += direction*(runSpeed*cos((angle*pgreco) / 180));
	posY += direction*(runSpeed*sin((angle*pgreco) / 180));

}

void updateLook(){
	//aggiorna la direzione in cui si guarda in base ad "angle"(angolo di rotazione del giocatore)//

	lookX = posX + cos((angle*pgreco) / 180);
	lookY = posY + sin((angle*pgreco) / 180);

}

void setting(){

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("LABYRINTH - BPS™15");
	glClearColor(0, 0, 0, 0.0);
	glShadeModel(GL_FLAT);//or GL_SMOOTH
	glEnable(GL_DEPTH_TEST);//attiva il depth buffering

}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluLookAt(posX, posY, 1.0, lookX, lookY, 1.0, 0, 0, 1.0);
	labyrinth();
	glPopMatrix();
	glutSwapBuffers();

}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {

	case 'w':
		updatePos(1);
		updateLook();
		glutPostRedisplay();
		break;

	case's':
		updatePos(-1);
		updateLook();
		glutPostRedisplay();
		break;

	case 'a':
		angle += rotationSpeed;
		updateLook();
		glutPostRedisplay();
		break;

	case 'd':
		angle -= rotationSpeed;
		updateLook();
		glutPostRedisplay();
		break;

	default:
		break;

	}

}

void reshape(int w, int h) {

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(lookAngle, (GLfloat)w / (GLfloat)h, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	setting();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;

}
