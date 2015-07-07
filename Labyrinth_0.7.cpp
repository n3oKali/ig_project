#include <math.h>
#include <ctime>
#include <stdio.h>
#include <assert.h>
#include "SOIL.h"
#include <GL/glut.h>


using namespace std;

int width = 1280, height = 720;						//dimensioni finestra//										//DEFAULT: 1280, 720//

const int labWidth = 12, labHeight = 8;				//grandezza della matrice del labitinto (width,height)//
const float side = 3, hside = side/2;				//larghezza nello spazio di una cella della matrice (side)//

float posX = 0, posY = 0, lookX = 0, lookY = 0;		//parametri di posizione e vista (lookAt)//					//DEFAULT: 0, 0, 0, 0//
float angle = 0, rotationSpeed = 5, runSpeed = side/10;//angolo e velocità di rotazione e corsa del giocatore//	//DEFAULT: 0, 2, 0.2//
float lookAngle = 60, zNear = 0.0005, zFar = 40;	//angolo di visuale, distanze (min e max) di vista//		//DEFAULT: 60, 0.0005, 40//

float tposX = 0, tposY = 0, tposZ = side*0.4, tangle = 0, trotSpeed = 2;	//parametri di posizione e rotazione del "cuboRotante/sfera"//
float tside = side/4, tray = tside*0.7, tslices = 32, tstacks = 32;			//lato cuboRot. (tside) parametri della sfera (tray,tslices,tstacks)//
GLfloat cubeColor[] = {0, 0.8, 0, 0.8}, sphereColor[] = {0.8, 0, 0, 0.8};	//colori di cubo e sfera//
GLfloat texColor[] = {0.8, 0.8, 0.8, 1};									//colore di sfondo alla texture//

bool winMODE = false, endMODE = false;				//vale true dopo la collisione con il cuboRotante//
clock_t startTime;									//contiene il tempo trascorso dall'inizio della partita//
const int playTime = 20;							//tempo disponibile per vincere la partita [in sec]//
const int standTime = 5;							//tempo che trascorre prima che termini il programma dopo la vittoria [in sec]//
const float PIGRECO = 3.14;							//costante che rappresenta il pi-greco//


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

void disegnaParete(float x, float y, int direction, int normal, int w, int h){
	//disegna una parete a partire dalle coordinate x,y ricevute e verso la direzione indicata//
	//direction = 0 disegna la parete estendendola verso l'asse x//
	//direction = 1 disegna la parete estendendola verso l'asse y//
	//normal = 1 imposta la normale con verso positivo verso l'asse opposto alla direction//
	//normal = -1 imposta la normale con verso negativo verso l'asse opposto alla direction//
	//w rappresenta la larghezza, h l'altezza//

	glBegin(GL_QUADS);

	//imposta la normale
	if (direction == 0)
		glNormal3f(0, normal, 0);
	else
		glNormal3f(normal, 0, 0);

	//definisce vertici per geometria e texture//
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
					//disegna una parete verso x e con normale verso la cella vuota//
					disegnaParete(i*side, j*side, 0, (matrixLab[i][j]==1) ? 1 : -1, side, side);
			if (i != 0)
				if (matrixLab[i-1][j] != matrixLab[i][j])
					//disegna una parete verso y e con normale verso la cella vuota//
					disegnaParete(i*side, j*side, 1, (matrixLab[i][j] == 1) ? 1 : -1, side, side);
		}
	}

}

void disegnaSuperficie(float x, float y, float z, int normal){
	//disegna le due superfici che rappresentano pavimento e soffitto in base alla grandezza della matrice//

	glBegin(GL_QUADS);

	//imposta la normale verso l'alto, con 1 (per il pavimento)//
	//imposta la normale verso il basso, con -1 (per il soffitto)//
	glNormal3f(0, 0, normal);

	//definizione vertici per geometria e texture//
	glTexCoord2f(0, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(side, 0);
	glVertex3f(x+side, y, z);
	glTexCoord2f(side, side);
	glVertex3f(x+side, y+side, z);
	glTexCoord2f(0, side);
	glVertex3f(x, y+side, z);

	glEnd();

}

void disegnaPavimentiESoffitto(int h, int normal){
	//disegna parete e soffitto quando trova una cella con valore 1//

	for (int i = 0; i < labHeight; i++){
		for (int j = 0; j < labWidth; j++){
			if (matrixLab[i][j] == 1)
				disegnaSuperficie(i*side, j*side, h, normal);
		}
	}

}

void loadTex(const char*, int, int);

void displayLabyrinth(){

	//abilita le texture
	glEnable(GL_TEXTURE_2D);

	//setta il colore di sfondo per le texture
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, texColor);

	//disegna le pareti
	//glColor3f(1, 0.35, 0);
	loadTex(texPareti, 256, 256);
	disegnaMuri();

	//disegna il pavimento
	//glColor3f(0, 0, 1);
	loadTex(texPavimento, 256, 256);
	disegnaPavimentiESoffitto(0, 1);

	//disegna il soffitto
	//glColor3f(0, 1, 1);
	loadTex(texSoffitto, 256, 256);
	disegnaPavimentiESoffitto(side, -1);

	//sposta il cursore di disegno nella posizione del cuboRotante
	glTranslatef(tposX, tposY, tposZ);

	//disabilita texture
	glDisable(GL_TEXTURE_2D);

	//se hai vinto disegna la sfera
	if (winMODE){
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
		glutSolidSphere(tray, tslices, tstacks);
		//se lo stand time è scaduto passa alla endingDisplay
		if (clock() > startTime + (standTime * 1000)){
			endMODE = true;
		}
	}
	//altrimenti ruota e disegna il cubo
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sphereColor);
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

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	loadTex((winMODE)? texVittoria : texSconfitta, 256, 256);
	glTranslatef(0, 0, (0.6*side)-0.5);
	disegnaParete(-0.5, -0.5, 0, 0, 1, 1);

}

void lightSetting(){

	//abilita le luci e la light0//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//imposta la luce ambientale, diffusa, e speculare//
	GLfloat ambientalLight[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1 };
	GLfloat specularLight[] = { 0.2, 0.2, 0.2, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientalLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);

}

void setting(){
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("LABYRINTH - BPS™15 (n3o@kali)");
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);//attiva lo smooth shading
	glEnable(GL_DEPTH_TEST);//attiva il depth buffering

	srand(clock());//passa un seme all'oggetto random
	placePlayer();//piazza il giocatore
	placeTarget();//piazza l'obiettivo (il cubo)
	lightSetting();//imposta il modello di illuminazione
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