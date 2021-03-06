#include <math.h>
#include <ctime>
#include <stdio.h>
#include <assert.h>
#include "SOIL.h"
#include <GL/glut.h>


using namespace std;

int width = 1280, height = 720;				//dimensioni finestra//						//DEFAULT: 1280, 720//

const int labWidth = 12, labHeight = 8;			//grandezza della matrice del labitinto (width,height)//
const float side = 4, hside = side/2;			//larghezza nello spazio di una cella della matrice (side)//

float posX = 0, posY = 0, lookX = 0, lookY = 0;		//parametri di posizione e vista (lookAt)//			//DEFAULT: 0, 0, 0, 0//
float angle = 0, rotationSpeed = 5, runSpeed = side/10;	//angolo e velocità di rotazione e corsa del giocatore//	//DEFAULT: 0, 2, 0.2//
float lookAngle = 60, zNear = 0.0005, zFar = 40;	//angolo di visuale, distanze (min e max) di vista//		//DEFAULT: 60, 0.0005, 40//

float tposX = 0, tposY = 0, tposZ = side*0.4, tangle = 0, trotSpeed = 2;	//parametri di posizione e rotazione del "cuboRotante/sfera"//
float tside = side/4, tray = tside*0.7, tslices = 32, tstacks = 32;		//lato cuboRot. (tside) parametri della sfera (tray,tslices,tstacks)//

bool winMODE = false, endMODE = false;				//vale true dopo la collisione con il cuboRotante//
clock_t startTime;						//contiene il tempo trascorso dall'inizio della partita//
const int playTime = 20;					//tempo disponibile per vincere la partita [in sec]//
const int standTime = 5;					//tempo che trascorre prima che termini il programma dopo la vittoria [in sec]//
const float PIGRECO = 3.14;					//costante che rappresenta il pi-greco//


//percorsi texture per pavimento, soffitto, pareti, e schermata di vittoria e sconfitta//
const char* texPavimento = "C:\\Users\\utente\\Documents\\Visual Studio 2013\\Projects\\Progetto5\\Texture\\Rock-128.jpg";
const char* texSoffitto = texPavimento;
const char* texPareti = "C:\\Users\\utente\\Documents\\Visual Studio 2013\\Projects\\Progetto5\\Texture\\BrickWall-256.jpg";
const char* texVittoria = "C:\\Users\\utente\\Documents\\Visual Studio 2013\\Projects\\Progetto5\\Texture\\Win.jpg";
const char* texSconfitta = "C:\\Users\\utente\\Documents\\Visual Studio 2013\\Projects\\Progetto5\\Texture\\Lose.jpg";

/*int matrixLab[labHeight][labWidth] = {	0, 0, 0, 0, 0, 0,
										0, 1, 1, 1, 0, 0,
										0, 1, 0, 1, 0, 0,
										0, 1, 1, 1, 1, 0,
										0, 0, 0, 0, 1, 0 };*/

int matrixLab[labHeight][labWidth] = {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1,
										0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0,
										0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0,
										0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
										0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };//*/

void updatePos(int direction){
	//direction = 1 muove in AVANTI di distanza pari a "runSpeed"//
	//direction = -1 muove INDIETRO di distanza pari a "runSpeed"//
	
	float px = posX+(direction*(runSpeed*cos((angle*PIGRECO) / 180)));
	float py = posY+(direction*(runSpeed*sin((angle*PIGRECO) / 180)));
	
	//gestione delle collisioni..
	if (matrixLab[(int)(px/side)][(int)(py/side)] == 1){//..con le pareti//

		if (px<(tposX + tside) && px>(tposX - tside) && py<(tposY + tside) && py>(tposY - tside)){//..con il cuboRotante//
			if(!winMODE)startTime = clock();//azzera il tempo
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

void disegnaParete(float x, float y, int direction, int w, int h){
	//disegna una parete a partire dalle coordinate x,y ricevute e verso la direzione indicata//
	//direction = 0 disegna la parete verso l'asse x//
	//direction = 1 disegna la parete verso l'asse y//
	//w rappresenta la larghezza, h l'altezza//

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(x, y, 0);
	glTexCoord2f(w, 0);
	if (direction==0)
	glVertex3f(x+w, y, 0);
	if (direction==1)
	glVertex3f(x, y+w, 0);
	glTexCoord2f(w, h);
	if (direction==0)
	glVertex3f(x+w, y, h);
	if (direction==1)
	glVertex3f(x, y+w, h);
	glTexCoord2f(0, h);
	glVertex3f(x, y, h);

	glEnd();

}

void disegnaMuri(){
	//disegna una parete nello spazio ogni volta che trova due celle della matrice adiacenti con valori diversi//

	for (int i = 0; i < labHeight; i++){
		for (int j = 0; j < labWidth; j++){
			if (j != 0)
				if (matrixLab[i][j-1] != matrixLab[i][j])
					disegnaParete(i*side, j*side, 0, side, side);//disegna verso x
			if (i != 0)
				if (matrixLab[i-1][j] != matrixLab[i][j])
					disegnaParete(i*side, j*side, 1, side, side);//disegna verso y
		}
	}

}

void disegnaSuperficie(){
	//disegna le due superfici che rappresentano pavimento e soffitto in base alla grandezza della matrice//

	glBegin(GL_QUADS);

	glTexCoord2f(0,0);
	glVertex2f(0,0);
	glTexCoord2f(0, labWidth*side);
	glVertex2f(0, labWidth*side);
	glTexCoord2f(labHeight*side, labWidth*side);
	glVertex2f(labHeight*side, labWidth*side);
	glTexCoord2f(labHeight*side, 0);
	glVertex2f(labHeight*side, 0);

	glEnd();

}

void loadTex(const char*, int, int);

void displayLabyrinth(){

	//abilita le texture
	glEnable(GL_TEXTURE_2D);

	//disegna il pavimento
	//glColor3f(0, 0, 1);
	loadTex(texPavimento, 256, 256);
	disegnaSuperficie();
	//sposta il cursore di disegno in alto
	glTranslatef(0, 0, side);

	//disegna il soffitto
	//glColor3f(0, 1, 1);
	loadTex(texSoffitto, 256, 256);
	disegnaSuperficie();
	//riporta il cursore di disegno in (0,0,0)
	glTranslatef(0, 0, -side);

	//disegna le pareti
	//glColor3f(1, 0.35, 0);
	loadTex(texPareti, 256, 256);
	disegnaMuri();

	//disabilita texture
	glDisable(GL_TEXTURE_2D);

	//sposta il cursore di disegno nelle posizione del cuboRotante
	glTranslatef(tposX, tposY, tposZ);

	//se hai vinto disegna la sfera
	if (winMODE){
		glColor3f(0, 1, 0);
		glutSolidSphere(tray, tslices, tstacks);
		//se lo stand time è scaduto passa alla endingDisplay
		if (clock() > startTime + (standTime * 1000)){
			endMODE = true;
		}
	}
	//altrimenti ruota e disegna il cubo
	else {
		glColor3f(1, 0, 0);
		tangle += trotSpeed;
		glRotatef(tangle, 0, 0, tangle);
		glutSolidCube(tside);
		//se il play time è scaduto passa alla endingDisplay
		if (clock() > startTime + (playTime * 1000)){
			endMODE = true;
		}
	}

}

void loadTex(const char* filename, int w, int h){
	//carica la texture

	GLuint texture_id = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	unsigned char* img = SOIL_load_image(filename, &w, &h, NULL, 0);

	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	
	//setta i parametri

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SOIL_free_image_data(img);//libera la memoria

}

void displayEnding(){
	//sposta il giocatore nell'origine..
	posX = 0; posY = 0; lookX = 0; lookY = -1;
	
	//..e stampa di fronte al giocatore un immagine che indica il risultato della partita//
	glEnable(GL_TEXTURE_2D);
	loadTex((winMODE)? texVittoria : texSconfitta, 256, 256);
	glTranslatef(0, 0, (0.6*side)-0.5);
	disegnaParete(-0.5, -0.5, 0, 1, 1);

}

void setting(){
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("LABYRINTH - BPS15 (n3o@kali)");
	glClearColor(0, 0, 0, 0.0);
	glShadeModel(GL_FLAT);//o GL_SMOOTH
	glEnable(GL_DEPTH_TEST);//attiva il depth buffering

	srand(clock());//passa un seme all'oggetto random
	placePlayer();
	placeTarget();
	startTime = clock();//fa partire il cronometro

}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluLookAt(posX, posY, 0.6*side, lookX, lookY, 0.59*side, 0, 0, 0.6*side);

	if (!endMODE)
		displayLabyrinth();
	else
		displayEnding();
	
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
