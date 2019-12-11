/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzednych

/*************************************************************************************/
#define _USE_MATH_DEFINES // for C++


#include <cmath>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <cstdlib>
#include <vector>


using std::vector;


typedef float point3[3];

struct Point {
	float x, y, z;
	struct Color {
		float r, g, b;
	} color;
};


static GLfloat theta[] = { 0.0, 0.0, 0.0 };
vector<vector<Point>> points;

/*************************************************************************************/

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych


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

void drawEggFromTriangleStrips() {
	glBegin(GL_TRIANGLE_STRIP);
	const int nPoints = points.size();
	/*for (int j = 0; j < nPoints; ++j) {
		for (int i = 0; i < nPoints; ++i) {
			glColor3f(points[i][j].color.r, points[i][j].color.g, points[i][j].color.b);
			glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
		}
	}*/
	Point pdl, pdr, pur;
	for (int i = 0; i < nPoints - 1; ++i) {
		pdl = points[i][0];
		glColor3f(pdl.color.r, pdl.color.g, pdl.color.b);
		glVertex3f(pdl.x, pdl.y, pdl.z);
		for (int j = 0; j < nPoints - 1; ++j) {

			pur = points[i + 1][j + 1];
			glColor3f(pur.color.r, pur.color.g, pur.color.b);
			glVertex3f(pur.x, pur.y, pur.z);

			pdr = points[i][j + 1];
			glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
			glVertex3f(pdr.x, pdr.y, pdr.z);
		}
		pur = points[i][nPoints - 1];
		glColor3f(pur.color.r, pur.color.g, pur.color.b);
		glVertex3f(pur.x, pur.y, pur.z);
	}
	glEnd();
}

void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz¹tek i koniec obrazu osi y
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
	glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na bia³y
	//glRotated(60.0, 1.0, 1.0, 1.0);  // Obrót o 60 stopni
	glutWireTeapot(3.0); // Narysowanie obrazu czajnika do herbaty
}

//void spinEgg()
//{
//
//	theta[0] -= 0.2;
//	if (theta[0] > 360.0) theta[0] -= 360.0;
//
//	theta[1] -= 0.2;
//	if (theta[1] > 360.0) theta[1] -= 360.0;
//
//	theta[2] -= 0.2;
//	if (theta[2] > 360.0) theta[2] -= 360.0;
//
//	glutPostRedisplay(); //odœwie¿enie zawartoœci aktualnego okna
//}

void keys(unsigned char key, int x, int y)
{
	const float rotate_angle_speed = 5.0;
	switch (key) {
	case 'w':
		theta[0] += rotate_angle_speed;
		break;
	case 'a':
		theta[1] += rotate_angle_speed;
		break;
	case 's':
		theta[0] -= rotate_angle_speed;
		break;
	case 'd':
		theta[1] -= rotate_angle_speed;
		break;
	default:
		break;
	}

	for (float& angle : theta) {
		if (angle > 360) {
			angle -= 360;
			continue;
		}
		if (angle < 360) {
			angle += 360;
			continue;
		}
	}

	glutPostRedisplay();
}

/*************************************************************************************/

// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba
// przerysowaæ scenê)



void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	glMatrixMode(GL_MODELVIEW);
	//glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej

	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej

	// Rotate object on the screen
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);


	glTranslatef(0, -5.0, 0);

	// Render designed objects
	//printPot(); // OK
	//drawEggFromPoints(); // OK
	//drawEggFromLines(); // OK
	drawEggFromTriangles();
	//drawEggFromTriangleStrips();


	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania


	glutSwapBuffers();
	//
}

/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania



void MyInit(void)
{
	// Disable color fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny

	srand(time(nullptr));

	createEggPointModel(20);

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okreœlaj¹cej proporcjê
	// wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bie¿¹cej            
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wspó³czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okreœlenie tak zwanej
	// przestrzeni ograniczaj¹cej pozwalaj¹cej zachowaæ w³aœciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj¹cej s³u¿y funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else

		glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu                                   

	glLoadIdentity();
	// Czyszcenie macierzy bie¿¹cej
}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 600);

	glutCreateWindow("EGG 3-D");

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
	// (callback function).  Bedzie ona wywo³ywana za ka¿dym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst¹pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych

	//glutIdleFunc(spinEgg);

	glutKeyboardFunc(keys);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
	return 0;
}

/*************************************************************************************/