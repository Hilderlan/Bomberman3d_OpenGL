#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#define PI 3.1415
#define TAM_CENARIO 12

GLint WIDTH = 800; // Largura da janela
GLint HEIGHT = 600; // Altura da janela

GLint gouraundShader = 0; // Sombreamento
int rotacao = 0;
int rotacaoInicial[6] = {0, 0, 0, 0, 0, 0};
float pe[2] = {0, 0};
float peIni[6][2];
float vida = 0.2; // Quantidade de vidas do Bomberman

// Iluminacao
GLfloat posicao_luz0[]    = { 0.0, 10.0, 0.0, 1.0};
GLfloat cor_luz0[]        = { 1.0, 1.0, 1.0, 1.0};
GLfloat cor_luz0_ambiente[]    = { 0.3, 0.3, 0.3, 1.0};

// Bomba
int contBomba[50];
int quantBombas = 1;

// Cenario
int cenario[TAM_CENARIO][TAM_CENARIO];
int direcaoDeGeracao[6] = {1, 1, 1, 1, 1, 1};
int cont[6] = {1, 1, 1, 1, 1, 1};


float BombermanPosX = 9,  BombermanPosY = 7; // Posicao inicial do Bomberman
float inimigoPosX[6] = {3, 3, 9, 11, 5, 11}; // Posicao X do inimigo
float inimigoPosY[6] = {3, 9, 3, 9, 7, 7}; // Posicao Y do inimigo
float intensidadeFogo = 0;
float posicaoBomba[100][2];
float coordBomba[100][2];

bool quantInimigosVivos[6];

void reshape(int width, int height) {
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, width / (float)height, 0.1, 30.0);
	glMatrixMode(GL_MODELVIEW);
}

void Bomberman() {

	GLfloat cor[] = {1, 1, 1, 1 }; // Cor do Bomberman

	glPushMatrix();
	glTranslatef(BombermanPosX, 0.5, 0);
	glTranslatef(0, 0, BombermanPosY);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
	glScaled(0.4, 0.4, 0.4);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();

	glPopMatrix();
}

// Deteccao de colisao
bool colisao (float coord1[], float coord2[], float R1, float R2) {

	float D = sqrt(pow(coord1[0] - coord2[0], 2) + pow(coord1[1] - coord2[1], 2) + pow(coord1[2] - coord2[2], 2) );
	if( D < (R1 + R2) ) {
		return true;
	}
	return false;
}

// Desenha o fogo
void fogo(int i) {
	\
	GLfloat cor[] = {1, 0.4 , 0, 0.7 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
	glTranslatef(coordBomba[i][0], 0.5, 0);
	glTranslatef(0, 0, coordBomba[i][1]);

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
	glScalef (intensidadeFogo, 0.4, 0.4);
	glutSolidCube (1.0);
	glPopMatrix();

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
	glScalef (0.4, 0.4, intensidadeFogo);
	glutSolidCube (1.0);
	glPopMatrix();

	glPopMatrix();

	float c1[3] = {coordBomba[i][0], 0.5, coordBomba[i][1]};

	for(int j = 0 ; j < 6 ; j++) {
		float c2[3] = {inimigoPosX[j], 0.5, inimigoPosY[j]};
		if(colisao(c1, c2, intensidadeFogo, 0.2)) { // Se houver colisao entre o fogo e algum inimigo, o mesmo eh eliminado
			quantInimigosVivos[j] = false;
		}
	}
	float c2[3] = {BombermanPosX, 0.5, BombermanPosY}; // 0.5 eh a altura no plano

	// Se houver colisao entre o fogo e o bomberman, decrementa a vida e coloca-o na posicao original do cenario
	if(colisao(c1, c2, intensidadeFogo, 0.2)) {
		BombermanPosX = 9;
		BombermanPosY = 7;
		vida -= 0.1;
	}

	// Retirando o bloco do cenario caso haja colisao com um bloco destrutivel
	int x = int(coordBomba[i][0] + 0.2);
	int y = int(coordBomba[i][1] + 0.8);
	cenario[x][y] = 0;
	cenario[x + 1][y] = 0;
	cenario[x - 1][y] = 0;
	cenario[x][y + 1] = 0;
	cenario[x][y - 1] = 0;
}

void bomber(int i) {
	GLfloat cor[] = {0.1, 0.1, 0.1, 1 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
	glTranslatef(coordBomba[i][0], 0.5, 0);
	glTranslatef(0, 0, coordBomba[i][1]);

	glPushMatrix();
	glScaled(0.3, 0.3, 0.3);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();

	glPopMatrix();
}

void caixa(int i, int j) {
	GLfloat corCubo[] = {0.50, 0.5, 0.5, 1 };

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, corCubo);
	glTranslatef (i, 0.5, j);
	glScalef (1, 1, 1);
	glutSolidCube (1.0);
	glPopMatrix();

	cenario[i][j] = 1;
}

void bloco(int i, int j) {
	GLfloat corCubo[] = {0.92, 0.35, 0.6, 1 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, corCubo);
	glTranslatef (i, 0.5, j);

	glScalef (1, 1, 1);
	glColor3f(1, 2.0, 0.0);
	glutSolidCube (1.0);
	glPopMatrix();
	cenario[i][j] = 1;
}

void display(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	glClearColor(0.9, 0.9, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	gluLookAt( 12, 8, 6, 8, 0.5, 6, 0.0, 1.0, 0.0);
	glNormal3f(0, 1, 0);

	// Desenha o plano
	GLfloat corPlano[] = {0, 0.4, 0, 1};
	glBegin(GL_QUADS);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, corPlano );

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, TAM_CENARIO);
	glVertex3f(TAM_CENARIO, 0, TAM_CENARIO);
	glVertex3f(TAM_CENARIO, 0, 0);

	glEnd(); // End Plano
	
	// Desenha as caixas
	for(int i = 0; i < TAM_CENARIO; i += 2) {
		for(int j = 0; j < TAM_CENARIO; j += 2) {
			caixa(i, j);
		}
	}

	for(int i = 0; i <= TAM_CENARIO; i++) {
		caixa(0, i);
		caixa(i, 0);
		caixa(i, TAM_CENARIO);
		caixa(TAM_CENARIO, i);
	} // End caixas
	
	
	if(vida >= 0){ // Mostra o bomberman no cenario enquanto ele tiver vidas sobrando
		Bomberman();
	}
	
	// Desenha os blocos
	for(int i = 1; i < 12; i++){
		for(int j = 1; j < 12; j++){
			if(cenario[i][j] == 1) {
				bloco(i, j);
			}
		}
	} // End blocos
	
	// Responsavel por desenhar as bombas
	for(int i = 1; i < quantBombas; i++) {
		if(contBomba[i] < 80) {
			bomber(i);
			int x = int(coordBomba[i][0] + 0.2);
			int y = int(coordBomba[i][1] + 0.8);
			cenario[x][y] = 2;
		}
		if(contBomba[i] > 80 && contBomba[i] < 100) {
			if(contBomba[i] < 90)
				intensidadeFogo += 2;
			else {
				intensidadeFogo -= 2;
			}
			fogo(i);
		}

	} // End bombas

	// Design dos inimigos
	for(int i = 0; i < 5; i++){	// Sao 5 inimigos vivos
		if(quantInimigosVivos[i]) {

			GLfloat cor[] = {0.1, 0.2, 0.2, 1 };

			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
			glTranslatef(inimigoPosX[i], 0.5, 0);
			glTranslatef(0, 0, inimigoPosY[i]);

			glPushMatrix();
			glScaled(0.3, 0.3, 0.3);
			glTranslatef(0, 0.8, 0);
			glutSolidSphere(1, 30, 30);
			glPopMatrix();


			glPopMatrix();
		}
	} // End inimigos

	glPopMatrix();
	glutSwapBuffers();
}

void inimigos(int i) {
	if(quantInimigosVivos[i]) {
		float aux;

		srand((unsigned)time(0));

		cont[i]++;

		if(cont[i] > 50 + i * 10) {
			int rand(void);
			direcaoDeGeracao[i] = rand() % 4;

			cont[i] = 1;
		}


		if( direcaoDeGeracao[i] == 0) {
			aux = inimigoPosX[i];
			aux += 0.05;

			if(cenario[int(aux + 0.8)][int(inimigoPosY[i] + 0.2)] == 0) {
				inimigoPosX[i] = aux;
				rotacaoInicial[i] = 180 % 360;

			} else
				direcaoDeGeracao[i] = rand() % 4;

		} else if( direcaoDeGeracao[i] == 1) {
			aux = inimigoPosX[i];
			aux -= 0.05;
			if(cenario[int(aux + 0.2)][int(inimigoPosY[i] + 0.8)] == 0) {
				inimigoPosX[i] = aux;
				rotacaoInicial[i] = 0;

			} else
				direcaoDeGeracao[i] = rand() % 4;

		} else if( direcaoDeGeracao[i] == 2) {
			aux = inimigoPosY[i];
			aux += 0.05;
			if(cenario[int(inimigoPosX[i] + 0.2)][int(aux + 0.8)] == 0) {
				inimigoPosY[i] = aux;
				rotacaoInicial[i] = 90 % 360;

			} else
				direcaoDeGeracao[i] = rand() % 4;

		} else if( direcaoDeGeracao[i] == 3) {
			aux = inimigoPosY[i];
			aux -= 0.05;

			if(cenario[int(inimigoPosX[i] + 0.8)][int(aux + 0.2)] == 0) {
				inimigoPosY[i] = aux;
				rotacaoInicial[i] = 270 % 360;

			} else
				direcaoDeGeracao[i] = rand() % 4;

		}

		float c1[3] = {inimigoPosX[i], 0.5, inimigoPosY[i]};
		float c2[3] = {BombermanPosX, 0.5, BombermanPosY};

		if(colisao(c1, c2, 0.3, 0.3)) {
			BombermanPosX = 9;
			BombermanPosY = 7;
			vida -= 0.1;
		}
	}
}

void movimento(int i) {
	for(int i = 1; i < quantBombas; i++) {
		contBomba[i]++;
	}
	for(int i = 0; i < 4 ; i++)
		inimigos(i);

	glutPostRedisplay();
	glutTimerFunc(33, movimento, 1);
}


void keyboard(unsigned char key, int x, int y) {

	float aux;

	switch (key) {
		case 27:	// Tecla ESC
			exit(0);
			break;
	
		case 's':
			aux = BombermanPosX;
			aux += 0.1;
			if(cenario[int(aux + 0.8)][int(BombermanPosY + 0.2)] == 0 || cenario[int(BombermanPosX + 0.8)][int(BombermanPosY + 0.2)] == 2) {
				BombermanPosX = aux;
				rotacao = 180 % 360;
			}
			glutPostRedisplay();
			break;
		case 'w':
			aux = BombermanPosX;
			aux -= 0.1;
	
			if(cenario[int(aux + 0.2)][int(BombermanPosY + 0.8)] == 0 || cenario[int(BombermanPosX + 0.2)][int(BombermanPosY + 0.8)] == 2) {
				BombermanPosX = aux;
				rotacao = 0;
			}
			glutPostRedisplay();
			break;
		case 'a':
			aux = BombermanPosY;
			aux += 0.1;
	
			if(cenario[int(BombermanPosX + 0.2)][int(aux + 0.8)] == 0 || cenario[int(BombermanPosX + 0.2)][int(BombermanPosY + 0.8)] == 2) {
				rotacao = 90 % 360;
				BombermanPosY = aux;
	
			}
			glutPostRedisplay();
			break;
		case 'd':
			aux = BombermanPosY;
			aux -= 0.1;
			if(cenario[int(BombermanPosX + 0.8)][int(aux + 0.2)] == 0 || cenario[int(BombermanPosX + 0.8)][int(BombermanPosY + 0.2)] == 2) {
				rotacao = 270 % 360;
				BombermanPosY = aux;
			}
			glutPostRedisplay();
			break;
		case 32:
			coordBomba[quantBombas][0] = BombermanPosX;
			coordBomba[quantBombas][1] = BombermanPosY;
			quantBombas += 1;
			glutPostRedisplay();
			break;
	}
}


void init() {
	gouraundShader = 1;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Iluminacao
	glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz0_ambiente);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	srand((unsigned)time(0));
	int rand (void);

	for(int i = 0; i < TAM_CENARIO; i++)
		for(int j = 0; j < TAM_CENARIO; j++) {
			int pos = rand() % 2;
			cenario[i][j] = pos;
		}

	for(int i = 0; i < 100; i++) {
		contBomba[i] = 0;
		for(int j = 0; j < 2; j++)
			coordBomba[i][j] = 0;
	}
	for(int i = 0; i <  6; i++) {
		quantInimigosVivos[i] = true;
		cenario[int(inimigoPosX[i] + 0.8)][int(inimigoPosY[i] + 0.2)] = 0;
		direcaoDeGeracao[i] = rand() % 4;
		peIni[i][0] = 0.1;
		peIni[i][1] = 0;
	}
	cenario[int(BombermanPosX)][int(BombermanPosY)] = 0;
	cenario[int(BombermanPosX)][int(BombermanPosY - 1)] = 0;
	cenario[int(BombermanPosX)][int(BombermanPosY + 1)] = 0;
	
	printf("------------> BOMBERMAN 3D <-------------\n\n");
	printf("\tw - Andar para frente\n");
	printf("\ts - Andar para atras\n");
	printf("\ta - Andar para a esquerda\n");
	printf("\td - Andar para a direita\n");
	printf("\tEspaco - Soltar bomba\n");
	printf("\n-----------------------------------------\n");
}

int main(int argc, char * * argv) {
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit( & argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("Bomberman 3D");

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(33, movimento, 1);
	glutMainLoop();
	return 0;
}