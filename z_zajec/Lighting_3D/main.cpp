/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi 
//  uk�adu wsp�rzednych

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
	float normal[3] = { 0 };
};



vector<vector<Point>> points;


static GLfloat theta_m_x = 0.0;   // k�t obrotu obiektu

static GLfloat theta_m_y = 0.0;   // k�t obrotu obiektu
static GLfloat pix2angleX;     // przelicznik pikseli na stopnie
static GLfloat pix2angleY;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naci�ni�to �adnego klawisza
							   // 1 - naci�ni�ty zosta� lewy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_x = 0;        // r�nica pomi�dzy pozycj� bie��c�
									  // i poprzedni� kursora myszy

static int y_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_y = 0;        // r�nica pomi�dzy pozycj� bie��c�
									  // i poprzedni� kursora myszy



GLfloat light0_position[] = { 0,0.0, -5.0, 1.0 };
GLfloat light1_position[] = { -5.0, 3.0, -5.0, 1.0 };

GLfloat* current_light_position = light0_position;

GLfloat light0_ambient[] = { 0.05, 0.0, 0.0, 1.0 };
// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
// Ia = [Iar,Iag,Iab]

GLfloat light0_diffuse[] = { 0.6, 0.0, 0.0, 1.0 };
// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

GLfloat light0_specular[] = { 1.0, 0.0, 0.0, 1.0 };

GLfloat light1_ambient[] = { 0.0, 0.0, 0.05, 1.0 };
// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
// Ia = [Iar,Iag,Iab]

GLfloat light1_diffuse[] = { 0.0, 0.0, 0.6, 1.0 };
// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

GLfloat light1_specular[] = { 0.0, 0.0, 1.0, 1.0 };
// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
// odbicie kierunkowe Is = [Isr,Isg,Isb]
// po�o�enie �r�d�a

GLfloat* current_light_specular = light0_specular;

/*************************************************************************************/

// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych

# pragma region Egg

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


void designateNormalVector(float u, float v, float *outNormal, int i, int j) {
	float x_u = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
	float x_v = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI* v);
	float y_u = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
	float y_v = 0;
	float z_u = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
	float z_v = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI* v);
	outNormal[0] = y_u * z_v - z_u * y_v;
	outNormal[1] = z_u * x_v - x_u * z_v;
	outNormal[2] = x_u * y_v - y_u * x_v;
	float outNormalLen = sqrt(pow(outNormal[0], 2) + pow(outNormal[1], 2) + pow(outNormal[2], 2));
	for (int i = 0; i < 3; ++i) {
		outNormal[i] /= outNormalLen;
	}
	if (u > 1/2.0) {
		for (int i = 0; i < 3; ++i) {
			outNormal[i] *= -1;
		}
	}
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
			designateNormalVector(u_val, v_val, points[i][j].normal, i, j);

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
	const int nPoints = points.size();
	glPushMatrix();
	glTranslatef(0, -5, 0);
	glBegin(GL_TRIANGLES);
	Point pdl, pdr, pul, pur;
	for (int i = 0; i < nPoints - 1; ++i) {
		pdl = points[i][0];
		for (int j = 0; j < nPoints - 1; ++j) {
			glNormal3fv(pdl.normal);
			glVertex3f(pdl.x, pdl.y, pdl.z);

			
			pdr = points[i][j + 1];
			glNormal3fv(pdr.normal);
			glVertex3f(pdr.x, pdr.y, pdr.z);

			pul = points[i + 1][j];
			glNormal3fv(pul.normal);
			glVertex3f(pul.x, pul.y, pul.z);

			glNormal3fv(pdr.normal);
			glVertex3f(pdr.x, pdr.y, pdr.z);

			glNormal3fv(pul.normal);
			glVertex3f(pul.x, pul.y, pul.z);

			pur = points[i + 1][j + 1];
			glNormal3fv(pur.normal);
			glVertex3f(pur.x, pur.y, pur.z);

			pdl = pdr;
		}
	}
	glEnd();
	glPopMatrix();
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
#pragma endregion 

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
	glutSolidTeapot(3.0); // Narysowanie obrazu czajnika do herbaty
}

// Funkcja ustalaj�ca stan renderowania
void drawLightSources() {
	glPushMatrix();
	glTranslated(light0_position[0], light0_position[1], light0_position[2]);
	glColor3f(light0_specular[0], light0_specular[1], light0_specular[2]);
	glutWireSphere(1, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslated(light1_position[0], light1_position[1], light1_position[2]);
	glColor3f(light1_specular[0], light1_specular[1], light1_specular[2]);
	glutWireSphere(1, 16, 16);
	glPopMatrix();
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
	else

		status = 0;          // nie zosta� wci�ni�ty �aden klawisz
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
//	glutPostRedisplay(); //od�wie�enie zawarto�ci aktualnego okna
//}

void keys(unsigned char key, int x, int y)
{
	float move_speed = 0.1;
	float change_color_speed = 0.1;
	switch (key) {
	case '1':
		current_light_position = light0_position;
		current_light_specular = light0_specular;
		break;
	case '2':
		current_light_position = light1_position;
		current_light_specular = light1_specular;
		break;
	case 'w':
		current_light_position[1] += move_speed;
		break;
	case 'a':
		current_light_position[0] -= move_speed;
		break;
	case 's':
		current_light_position[1] -= move_speed;
		break;
	case 'd':
		current_light_position[0] += move_speed;
		break;
	case 'r':
		current_light_position[2] += move_speed;
		break;
	case 'f':
		current_light_position[2] -= move_speed;
		break;
	case 'p':
		if (current_light_specular == light0_specular) {
			current_light_specular[1] += change_color_speed;
		}
		else {
			current_light_specular[1] += change_color_speed;
		}
		break;
	case 'o':
		if (current_light_specular == light0_specular) {
			current_light_specular[1] -= change_color_speed;
		}
		else {
			current_light_specular[1] -= change_color_speed;
		}
		break;
	default:
		break;
	}


	

	glutPostRedisplay();
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


	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy�ej



	if (status == 1)                     // je�li lewy klawisz myszy wci�ni�ty
	{
		theta_m_x += delta_x * pix2angleX;    // modyfikacja k�ta obrotu o kat proporcjonalny
		theta_m_y += delta_y * pix2angleY;
	}                                  // do r�nicy po�o�e� kursora myszy

	glRotatef(theta_m_x, 0.0, 1.0, 0.0);  //obr�t obiektu o nowy k�t
	glRotatef(theta_m_y, 1.0, 0.0, 0.0);  //obr�t obiektu o nowy k�t


	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);


	// Render designed objects
	//printPot(); // OK
	//drawEggFromPoints(); // OK
	//drawEggFromLines(); // OK
	drawEggFromTriangles();
	//drawEggFromTriangleStrips();

	Axes();
	drawLightSources();

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania


	glutSwapBuffers();
	//
}

/*************************************************************************************/



void MyInit(void)
{
	// Disable color fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

	srand(time(nullptr));

	createEggPointModel(100);

	//region LIGHT
	/*************************************************************************************/

//  Definicja materia�u z jakiego zrobiony jest czajnik
//  i definicja �r�d�a �wiat�a

/*************************************************************************************/


/*************************************************************************************/
// Definicja materia�u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego               

	GLfloat mat_shininess = { 20.0 };
	// wsp�czynnik n opisuj�cy po�ysk powierzchni


/*************************************************************************************/
// Definicja �r�d�a �wiat�a





	GLfloat att_constant = { 1.0 };
	// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_linear = { 0.05 };
	// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_quadratic = { 0.001 };
	// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

/*************************************************************************************/
// Ustawienie parametr�w materia�u i �r�d�a �wiat�a

/*************************************************************************************/
// Ustawienie patrametr�w materia�u


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametr�w �r�d�a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);


	/*************************************************************************************/
	// Ustawienie opcji systemu o�wietlania sceny

	glShadeModel(GL_SMOOTH); // w�aczenie �agodnego cieniowania
	glEnable(GL_LIGHTING);   // w�aczenie systemu o�wietlenia sceny
	glEnable(GL_LIGHT0);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_LIGHT1);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_DEPTH_TEST); // w��czenie mechanizmu z-bufora

/*************************************************************************************/
	//endregion LIGHT
}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych
// w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s�
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angleX = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	pix2angleY = 360.0 / (float)vertical;  // przeliczenie pikseli na stopnie

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okre�laj�cej proporcj�
	// wymiar�w okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bie��cej            
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okre�lenie tak zwanej
	// przestrzeni ograniczaj�cej pozwalaj�cej zachowa� w�a�ciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj�cej s�u�y funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else

		glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu                                   

	glLoadIdentity();
	// Czyszcenie macierzy bie��cej
}

/*************************************************************************************/

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 600);

	glutCreateWindow("EGG 3-D");

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