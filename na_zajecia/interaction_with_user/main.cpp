/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi 
//  uk�adu wsp�rzednych

/*************************************************************************************/
#define _USE_MATH_DEFINES // for C++


#include <cmath>
#include <windows.h>
#include <gl/gl.h>
#include <GL/glut.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>


using std::vector;

/////////////////////////////////////////////////////////////////

typedef float point3[3];

struct Point {
	float x, y, z;
	struct Color {
		float r, g, b;
	} color;
};

static GLfloat theta_kb[] = { 0.0, 0.0, 0.0 };
vector<vector<Point>> points;

static GLfloat viewer[] = { 0.0, 0.0, 10.0 };

static GLfloat thetaCamera = 0.0;   // k�t obrotu obiektu
static GLfloat phiCamera = 0.0;
static GLfloat thetaTransform = 0.0;   // k�t obrotu obiektu
static GLfloat phiTransform = 0.0;

static GLfloat pix2angle_x;     // przelicznik pikseli na stopnie
static GLfloat pix2angle_y;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naci�ni�to �adnego klawisza
							   // 1 - naci�ni�ty zosta� lewy klawisz
							   // 2 - PPM

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;


static int delta_x = 0;        // r�nica pomi�dzy pozycj� bie��c�
									  // i poprzedni� kursora myszy
static int delta_y = 0;

static GLfloat scale = 1.0;
static GLfloat pix2Norm01;

static GLfloat horizontalNorm;
static GLfloat verticalNorm;

static float radius = 10;

static bool handleVirtualCamera = true;

/////////////////////////////////////////////////////////////////

// [0, 1]
float getRand() {
	return static_cast<float>(rand()) / RAND_MAX;
}

float getColor(int u, int v) {
	return (u * v) % 4; // %2
}

// u in [0, 1], v in [0, 1]
void calculatePoint3D(float u, float v, float& x, float& y, float& z) {
	x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
	y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
}

void createEggPointModel(const int edges) {
	const int nPoints = edges + 1;
	points = vector<vector<Point>>(nPoints, vector<Point>(nPoints));
	float step = 1.0f / edges;
	float u_val, v_val;

	// i -> u, j -> v
	for (int i = 0; i < nPoints; ++i) {
		for (int j = 0; j < nPoints; ++j) {
			u_val = i * step;
			v_val = j * step;
			if (i == nPoints - 1) {
				u_val = 1;
			}
			if (j == nPoints - 1) {
				v_val = 1;
			}
			calculatePoint3D(u_val, v_val, points[i][j].x, points[i][j].y, points[i][j].z);

			// My pattern for coloring the egg
			/*points[i][j].color.r = getColor(i ,j);
			points[i][j].color.g = getColor(i, j);
			points[i][j].color.b = getColor(i, j);*/

			// Disco colors
			points[i][j].color.r = getRand();
			points[i][j].color.g = getRand();
			points[i][j].color.b = getRand();
		}
	}

	// Artifact line fix
	for (int i = 0; i < nPoints; ++i) {
		points[i][0].color.r = points[nPoints - 1 - i][nPoints - 1].color.r;
		points[i][0].color.g = points[nPoints - 1 - i][nPoints - 1].color.g;
		points[i][0].color.b = points[nPoints - 1 - i][nPoints - 1].color.b;
	}

}

void designatePointOnSphere(float& x, float& y, float& z)
{
	x = radius * cos(thetaCamera) * cos(phiCamera);
	y = radius * sin(phiCamera);
	z = radius * sin(thetaCamera) * cos(phiCamera);
}

#pragma region Egg misc draws 

// OK
void drawEggFromPoints() {
	glBegin(GL_POINTS);
	const int nPoints = points.size();
	for (int i = 0; i < nPoints; ++i) {
		for (int j = 0; j < nPoints; ++j) {
			glColor3f(points[i][j].color.r, points[i][j].color.g, points[i][j].color.b);
			glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
		}
	}
	glEnd();
}

// OK
void drawEggFromLines() {
	glBegin(GL_LINES);
	const int nPoints = points.size();
	Point lastPoint, currentPoint;

	// Horizontal lines
	for (int i = 0; i < nPoints; ++i) {
		lastPoint = points[i][0];
		for (int j = 1; j < nPoints; ++j) {
			glColor3f(lastPoint.color.r, lastPoint.color.g, lastPoint.color.b);
			glVertex3f(lastPoint.x, lastPoint.y, lastPoint.z);

			currentPoint = points[i][j];
			glColor3f(currentPoint.color.r, currentPoint.color.g, currentPoint.color.b);
			glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

			lastPoint = currentPoint;
		}
	}

	// Vertical lines
	for (int j = 0; j < nPoints; ++j) {
		lastPoint = points[0][j];
		for (int i = 1; i < nPoints; ++i) {
			glColor3f(lastPoint.color.r, lastPoint.color.g, lastPoint.color.b);
			glVertex3f(lastPoint.x, lastPoint.y, lastPoint.z);

			currentPoint = points[i][j];
			glColor3f(currentPoint.color.r, currentPoint.color.g, currentPoint.color.b);
			glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

			lastPoint = currentPoint;
		}
	}
	glEnd();
}

// OK
void drawEggFromTriangles() {
	glBegin(GL_TRIANGLES);
	const int nPoints = points.size();

	Point pdl, pdr, pul, pur;
	for (int i = 0; i < nPoints - 1; ++i) {
		pdl = points[i][0];
		for (int j = 0; j < nPoints - 1; ++j) {
			glColor3f(pdl.color.r, pdl.color.g, pdl.color.b);
			glVertex3f(pdl.x, pdl.y, pdl.z);

			pdr = points[i][j + 1];
			glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
			glVertex3f(pdr.x, pdr.y, pdr.z);

			pul = points[i + 1][j];
			glColor3f(pul.color.r, pul.color.g, pul.color.b);
			glVertex3f(pul.x, pul.y, pul.z);

			glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
			glVertex3f(pdr.x, pdr.y, pdr.z);
			glColor3f(pul.color.r, pul.color.g, pul.color.b);
			glVertex3f(pul.x, pul.y, pul.z);

			pur = points[i + 1][j + 1];
			glColor3f(pur.color.r, pur.color.g, pur.color.b);
			glVertex3f(pur.x, pur.y, pur.z);

			pdl = pdr;
		}
	}
	glEnd();
}

#pragma endregion

// OK
void drawEggFromTriangleStrips() {
	const int nPoints = points.size();
	Point pul, pdl, pur, pdr;

	for (int i = 0; i < nPoints - 1; ++i) {
		glBegin(GL_TRIANGLE_STRIP);

		pul = points[i + 1][0];
		glColor3f(pul.color.r, pul.color.g, pul.color.b);
		glVertex3f(pul.x, pul.y, pul.z);

		for (int j = 0; j < nPoints - 1; ++j) {

			pdl = points[i][j];
			glColor3f(pdl.color.r, pdl.color.g, pdl.color.b);
			glVertex3f(pdl.x, pdl.y, pdl.z);

			pur = points[i + 1][j + 1];
			glColor3f(pur.color.r, pur.color.g, pur.color.b);
			glVertex3f(pur.x, pur.y, pur.z);
		}
		pdr = points[i][nPoints - 1];
		glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
		glVertex3f(pdr.x, pdr.y, pdr.z);

		glEnd();
	}

}


/*************************************************************************************/

// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych
void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz�tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz�tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz�tek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

void printPot() {
	glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na bia�y
	//glRotated(60.0, 1.0, 1.0, 1.0);  // Obr�t o 60 stopni
	glutWireTeapot(3.0); // Narysowanie obrazu czajnika do herbaty
}

//void spinEgg()
//{
//
//	theta_kb[0] -= 0.2;
//	if (theta_kb[0] > 360.0) theta_kb[0] -= 360.0;
//
//	theta_kb[1] -= 0.2;
//	if (theta_kb[1] > 360.0) theta_kb[1] -= 360.0;
//
//	theta_kb[2] -= 0.2;
//	if (theta_kb[2] > 360.0) theta_kb[2] -= 360.0;
//
//	glutPostRedisplay(); //od�wie�enie zawarto�ci aktualnego okna
//}

void keys(unsigned char key, int x, int y)
{
	if (key == 'c')
	{
		handleVirtualCamera = !handleVirtualCamera;
	}
}

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia warto�ci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
							 // jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;          // wci�ni�ty zosta� lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		y_pos_old = y;
		status = 2;
	}
	else
	{
		status = 0;          // nie zosta� wci�ni�ty �aden klawisz
	}
}

/*************************************************************************************/
// Funkcja "monitoruje" po�o�enie kursora myszy i ustawia warto�ci odpowiednich
// zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie r�nicy po�o�enia kursora myszy
	delta_y = y - y_pos_old;

	x_pos_old = x;            // podstawienie bie��cego po�o�enia jako poprzednie
	y_pos_old = y;

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/

// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana gdy trzeba
// przerysowa� scen�)
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym

	glMatrixMode(GL_MODELVIEW);
	//glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	// Czyszczenie macierzy bie��cej


	if (handleVirtualCamera) {
		if (status == 1)
		{
			thetaCamera += 2 * M_PI * horizontalNorm * delta_x;
			phiCamera += 2 * M_PI * verticalNorm * delta_y;
		}
		if (status == 2)
		{
			if ((delta_y < 0 && radius > 5) || (delta_y > 0 && radius < 15))
			{
				radius += 20 * verticalNorm * delta_y;
			}

		}
		designatePointOnSphere(viewer[0], viewer[1], viewer[2]);
	}
	else {
		if (status == 1)                     // je�li lewy klawisz myszy wci�ni�ty
		{
			thetaTransform += delta_x * pix2angle_x;    // modyfikacja k�ta obrotu o kat proporcjonalny
			phiTransform += delta_y * pix2angle_y;
		}                                  // do r�nicy po�o�e� kursora myszy
		if (status == 2)
		{
			scale += pix2Norm01 * -delta_y;
		}
	}
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(thetaTransform, 0.0, 1.0, 0.0);  //obr�t obiektu o nowy k�t
	glRotatef(phiTransform, 1.0, 0.0, 0.0);
	glScalef(scale, scale, scale);
	

	Axes();

	// Render designed objects
	printPot(); // OK

	//glTranslatef(0, -5.0, 0);
	//drawEggFromPoints(); // OK
	//drawEggFromLines(); // OK
	//drawEggFromTriangles(); // OK
	//drawEggFromTriangleStrips(); // OK

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania

	glutSwapBuffers();
}

/*************************************************************************************/

// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	// Disable color fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

	srand(time(nullptr));

	createEggPointModel(100);

}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle_x = 360.0 / (float)horizontal;
	pix2angle_y = 360.0 / (float)vertical;
	pix2Norm01 = 1.0 / vertical;

	horizontalNorm = 1.0 / horizontal;
	verticalNorm = 1.0 / vertical;

	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bie��cej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametr�w dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
	// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna

	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie��cej

}

/*************************************************************************************/

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 600);

	glutCreateWindow("Interaction with user");

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
	// (callback function).  Bedzie ona wywo�ywana za ka�dym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn�
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W��czenie mechanizmu usuwania powierzchni niewidocznych

	//glutIdleFunc(spinEgg);

	glutKeyboardFunc(keys);

	glutMouseFunc(Mouse);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
	return 0;
}

/*************************************************************************************/