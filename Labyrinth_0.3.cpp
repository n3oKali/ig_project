#include <GL/glut.h>
#include <math.h>
#include <time.h>

using namespace std;

int width = 1280, height = 720;					//dimensioni finestra//									//DEFAULT: 1280, 720//
float posX = 0, posY = 0, lookX = 0, lookY = 0;	//parametri di posizione e vista (lookAt)//				//DEFAULT: 0, 0, 0, 0//
float angle = 0, pgreco = 3.14;					//angolo di rotazione del giocatore//					//DEFAULT: 0, 3.14//
float rotationSpeed = 2, runSpeed = 0.2;		//velocità di rotazione e corsa del giocatore//			//DEFAULT: 2, 0.2//
float lookAngle = 60, zNear = 0.0005, zFar = 40;//angolo di visuale, distanze (min e max) di vista//	//DEFAULT: 60, 0.0005, 40//

const int labWidth = 6, labHeight = 5, side = 8, hside = side*0.5;

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

	if (matrixLab[(int)py/side][(int)(px/side)] == 1){
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
	//cerca una posizione casuale all'interno della matrice che sia libera(diversa da 0)//

	srand(time(NULL));
	int mX = 0, mY = 0;
	while (matrixLab[mY][mX] == 0){
		mX = rand() % labHeight;
		mY = rand() % labWidth;
	}

	//posiziona graficamente il giocatore, alle coordinate spaziali corrispondenti a quelle trovate per la matrice// 

	posX = (mX*side)+hside;
	posY = (mY*side)+hside;

	updateLook();

}

void disegnaMuri(){
	//disegna muri(sottoforma di cubi) nelle coordinate spaziali corrispondenti a quelle degli 0 nella matrice//

	for (int i = 0; i < labHeight; i++){
		for (int j = 0; j < labWidth; j++){
			if (matrixLab[i][j] != 1)glutSolidCube(side);
			glTranslatef(side, 0, 0);
		}
		glTranslatef(-(side*labWidth), side, 0);
	}

}

void disegnaSuperficie(){
	//disegna le due superfici che rappresentano pavimento e soffitto in base alla grandezza della matrice//

	glBegin(GL_QUADS);
	glVertex2d(0.0, 0.0);
	glVertex2d(0.0, labHeight*side);
	glVertex2d(labWidth*side, labHeight*side);
	glVertex2d(labWidth*side, 0.0);
	glEnd();

}

void labyrinth(){

	glColor3f(0, 0, 1);
	disegnaSuperficie();//disegna il pavimento
	glTranslatef(0, 0, side);//sposta il cursore di disegno in alto
	glColor3f(0, 1, 1);
	disegnaSuperficie();//disegna il soffitto
	glTranslatef(hside, hside, -hside);//sposta il cursore a metà tra pavimento e soffitto
	glColor3f(1, 0.35, 0);
	disegnaMuri();//disegna le pareti

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
	gluLookAt(posX, posY, hside+1.0, lookX, lookY, hside+0.9, 0, 0, hside+1.0);
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