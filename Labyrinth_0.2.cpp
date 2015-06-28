#include <GL/glut.h>
#include <math.h>
#include <time.h>

using namespace std;

int width = 1280, height = 720;			//dimensioni finestra//					//DEFAULT: 1280, 720//
float posX = 0, posY = 0, lookX = 0, lookY = 0;	//parametri di posizione e vista (lookAt)//		//DEFAULT: 0, 0, 0, 0//
float angle = 0, pgreco = 3.14;			//angolo di rotazione del giocatore//			//DEFAULT: 0, 3.14//
float rotationSpeed = 2, runSpeed = 0.2;	//velocità di rotazione e corsa del giocatore//		//DEFAULT: 2, 0.3//
float lookAngle = 60, zNear = 1.0, zFar = 40;	//angolo di visuale, distanze (min e max) di vista//	//DEFAULT: 60, 1, 40//

const int labWidth = 6, labHeight = 5, side = 8;

int matrixLab[labHeight][labWidth] = {	0, 0, 0, 0, 0, 0,
					0, 1, 1, 1, 0, 0,
					0, 1, 0, 1, 0, 0,
					0, 1, 1, 1, 1, 0,
					0, 0, 0, 0, 1, 0 };

void updatePos(int direction){
	//direction = 1 muove in AVANTI di distanza pari a "runSpeed"//
	//direction = -1 muove INDIETRO di distanza pari a "runSpeed"//

	float px = posX+(direction*(runSpeed*cos((angle*pgreco) / 180)));
	float py = posY+(direction*(runSpeed*sin((angle*pgreco) / 180)));

	if (matrixLab[(int)((py/side)+0.5)][(int)((px/side)+0.5)] == 1){
		posX = px;
		posY = py;
	}

}

void updateLook(){
	//aggiorna la direzione in cui si guarda in base ad "angle"(angolo di rotazione del giocatore)//

	lookX = posX + cos((angle*pgreco) / 180);
	lookY = posY + sin((angle*pgreco) / 180);

}

void placePlayer(){

	srand(time(NULL));
	int px = 0, py = 0;
	while (matrixLab[py][px] == 0){
		px = rand() % labHeight;
		py = rand() % labWidth;
	}

	posX = (px*side);
	posY = (py*side);

	updateLook();

}

void labyrinth(){

	glColor3f(1.0, 0.35, 0);

	for (int i = 0; i < labHeight; i++){
		for (int j = 0; j < labWidth; j++){
			if (matrixLab[i][j] != 1)glutSolidCube(side);
			glTranslatef(side, 0, 0);
		}
		glTranslatef(-(side*labWidth), side, 0);
	}

}

void setting(){
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("LABYRINTH - BPS™15");
	glClearColor(0, 0, 0, 0.0);
	glShadeModel(GL_SMOOTH);//or GL_FLAT
	glEnable(GL_DEPTH_TEST);//attiva il depth buffering
	placePlayer();

}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluLookAt(posX, posY, 1.0, lookX, lookY, 0.9, 0, 0, 1.0);
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
