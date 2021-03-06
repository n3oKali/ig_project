#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

int width = 1280, height = 720;				//dimensioni finestra//						//DEFAULT: 1280, 720//

float posX = 0, posY = 0, lookX = 0, lookY = 0;		//parametri di posizione e vista (lookAt)//			//DEFAULT: 0, 0, 0, 0//
float angle = 0, rotationSpeed = 2, runSpeed = 0.2;	//angolo e velocità di rotazione e corsa del giocatore//	//DEFAULT: 0, 2, 0.2//
float lookAngle = 60, zNear = 0.0005, zFar = 40;	//angolo di visuale, distanze (min e max) di vista//		//DEFAULT: 60, 0.0005, 40//

const int labWidth = 6, labHeight = 5;			//grandezza della matrice del labitinto (width,height)//
const int side = 8, hside = side / 2;			//larghezza nello spazio di una cella della matrice (side)//

float tposX = 0, tposY = 0, tposZ = side*4/9, tangle = 0, trotSpeed = 0.2;	//parametri di posizione e rotazione del "cuboRotante/sfera"//
float tside = side/4, tray = tside*0.7, tslices = 32, tstacks = 32;		//lato cuboRot. (tside) parametri della sfera (tray,tslices,tstacks)//

bool winMODE = false;					//vale true dopo la collisione con il cuboRotante//
const float PIGRECO = 3.14;				//costante che rappresenta il pi-greco//

int matrixLab[labHeight][labWidth] = {	0, 0, 0, 0, 0, 0,
										0, 1, 1, 1, 0, 0,
										0, 1, 0, 1, 0, 0,
										0, 1, 1, 1, 1, 0,
										0, 0, 0, 0, 1, 0 };

/*int matrixLab[labHeight][labWidth] = {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1,
										0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0,
										0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0,
										0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
										0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };*/

void updatePos(int direction){
	//direction = 1 muove in AVANTI di distanza pari a "runSpeed"//
	//direction = -1 muove INDIETRO di distanza pari a "runSpeed"//

	float px = posX+(direction*(runSpeed*cos((angle*PIGRECO) / 180)));
	float py = posY+(direction*(runSpeed*sin((angle*PIGRECO) / 180)));
	
	//gestione delle collisioni..
	if (matrixLab[(int)(px/side)][(int)(py/side)] == 1){//..con le pareti//

		if (px<(tposX + tside) && px>(tposX - tside) && py<(tposY + tside) && py>(tposY - tside)){//..con il cuboRotante//
			winMODE = true;
			return;
		}

		posX = px;
		posY = py;
	}

}

void updateLook(){
	//aggiorna la direzione in cui si guarda in base ad "angle"(angolo di rotazione del giocatore)//

	lookX = posX + cos((angle*PIGRECO) / 180);
	lookY = posY + sin((angle*PIGRECO) / 180);

}

void placePlayer(){
	//cerca una posizione casuale all'interno della matrice che sia libera(diversa da 0)//

	int mX = 0, mY = 0;
	while (matrixLab[mX][mY] == 0){
		mX = rand() % labHeight;
		mY = rand() % labWidth;
	}

	//posiziona il giocatore, alle coordinate spaziali corrispondenti a quelle trovate per la matrice// 

	posX = (mX*side)+hside;
	posY = (mY*side)+hside;

	updateLook();

}

void placeTarget(){
	//cerca una posizione casuale all'interno della matrice che sia libera(diversa da 0 && non occupata dal giocatore)//
	
	//coordinate di matrice per la posizione del giocatore//
	int pX = (posX-hside)/side;
	int pY = (posY-hside)/side;

	int tX = 0; int tY = 0;
	while (matrixLab[tX][tY] == 0 || (tX==pX && tY==pY)){
		tX = rand() % labHeight;
		tY = rand() % labWidth;
	}

	//posiziona il cubo, alle coordinate spaziali corrispondenti a quelle trovate per la matrice//

	tposX = (tX*side) + hside;
	tposY = (tY*side) + hside;

}

void disegnaParete(int x, int y, int direction){
	//disegna una parete a partire dalle coordinate x,y ricevute e verso la direzione indicata//
	//direction = 0 disegna la parete verso l'asse x//
	//direction = 1 disegna la parete verso l'asse y//

	glBegin(GL_QUADS);

	glVertex3f(x, y, 0);
	if (direction==0)
	glVertex3f(x+side, y, 0);
	if (direction==1)
	glVertex3f(x, y+side, 0);
	if (direction==0)
	glVertex3f(x+side, y, side);
	if (direction==1)
	glVertex3f(x, y+side, side);
	glVertex3f(x, y, side);

	glEnd();

}

void disegnaMuri(){
	//disegna una parete nello spazio ogni volta che trova due celle della matrice adiacenti con valori diversi//

	for (int i = 0; i < labHeight; i++){
		for (int j = 0; j < labWidth; j++){
			if (j != 0)
				if (matrixLab[i][j-1] != matrixLab[i][j])
					disegnaParete(i*side, j*side, 0);//disegna verso x
			if (i != 0)
				if (matrixLab[i-1][j] != matrixLab[i][j])
					disegnaParete(i*side, j*side, 1);//disegna verso y
		}
	}

}

void disegnaSuperficie(){
	//disegna le due superfici che rappresentano pavimento e soffitto in base alla grandezza della matrice//

	glBegin(GL_QUADS);

	glVertex2f(0,0);
	glVertex2f(0, labWidth*side);
	glVertex2f(labHeight*side, labWidth*side);
	glVertex2f(labHeight*side, 0);

	glEnd();

}

void labyrinth(){

	//disegna il pavimento
	glColor3f(0, 0, 1);
	disegnaSuperficie();

	//sposta il cursore di disegno in alto
	glTranslatef(0, 0, side);
	//disegna il soffitto
	glColor3f(0, 1, 1);
	disegnaSuperficie();

	//riporta il cursore di disegno in (0,0,0)
	glTranslatef(0, 0, -side);
	//disegna le pareti
	glColor3f(1, 0.35, 0);
	disegnaMuri();

	//sposta il cursore di disegno nelle posizione del cuboRotante
	glTranslatef(tposX, tposY, tposZ);
	//se hai vinto disegna la sfera
	if (winMODE){
		glColor3f(0, 1, 0);
		glutSolidSphere(tray, tslices, tstacks);
	}
	//altrimenti ruota e disegna il cubo
	else {
		glColor3f(1, 0, 0);
		tangle += trotSpeed;
		glRotatef(tangle, 0, 0, tangle);
		glutSolidCube(tside);
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

	srand(time(NULL));
	placePlayer();
	placeTarget();

}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluLookAt(posX, posY, hside+(0.1*side), lookX, lookY, hside+(0.09*side), 0, 0, hside+(0.1*side));
	labyrinth();
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {

	case 'w':
		updatePos(1);
		updateLook();
		break;

	case's':
		updatePos(-1);
		updateLook();
		break;

	case 'a':
		angle += rotationSpeed;
		updateLook();
		break;

	case 'd':
		angle -= rotationSpeed;
		updateLook();
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
