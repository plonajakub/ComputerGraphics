/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi 
//  uk�adu wsp�rzednych

/*************************************************************************************/
#define _USE_MATH_DEFINES // for C++


#include <cmath>
#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
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
	float u_val, v_val;
};


static GLfloat theta[] = { 0.0, 0.0, 0.0 };
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

enum Texture { First, Second };
Texture textureChoice = First;

/*************************************************************************************/

// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych


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
			points[i][j].u_val = u_val;
			points[i][j].v_val = v_val;
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
	glTranslatef(0, -5, 0);
	glBegin(GL_TRIANGLES);
	const int nPoints = points.size();
	
	Point pdl, pdr, pul, pur;
	for (int i = 0; i < nPoints - 1; ++i) {
		pdl = points[i][0];
		for (int j = 0; j < nPoints - 1; ++j) {
			//glColor3f(pdl.color.r, pdl.color.g, pdl.color.b);
			glTexCoord2f(points[i][j].u_val, points[i][j].v_val);
			glVertex3f(pdl.x, pdl.y, pdl.z);

			pdr = points[i][j + 1];
			//glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
			glTexCoord2f(points[i][j + 1].u_val, points[i][j + 1].v_val);
			glVertex3f(pdr.x, pdr.y, pdr.z);

			pul = points[i + 1][j];
			//glColor3f(pul.color.r, pul.color.g, pul.color.b);
			glTexCoord2f(points[i + 1][j].u_val, points[i + 1][j].v_val);
			glVertex3f(pul.x, pul.y, pul.z);

			//glColor3f(pdr.color.r, pdr.color.g, pdr.color.b);
			glTexCoord2f(points[i][j + 1].u_val, points[i][j + 1].v_val);
			glVertex3f(pdr.x, pdr.y, pdr.z);

			//glColor3f(pul.color.r, pul.color.g, pul.color.b);
			glTexCoord2f(points[i + 1][j].u_val, points[i + 1][j].v_val);
			glVertex3f(pul.x, pul.y, pul.z);

			pur = points[i + 1][j + 1];
			glTexCoord2f(points[i + 1][j + 1].u_val, points[i + 1][j + 1].v_val);
			//glColor3f(pur.color.r, pur.color.g, pur.color.b);
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

void drawTextureTriangle() {
	glTranslatef(0.5*-5, (1.0 / 3)*-5, 0);
	glScalef(5, 5, 1);

	glBegin(GL_TRIANGLES);

	glVertex3f(0.0, 0.0, 0.0);
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0f, 0.0f);

	glVertex3f(1.0f, 0.0f, 0.0);
	glNormal3f(1.0f, 0.0f, 1.0);
	glTexCoord2f(1.0f, 0.0f);

	glVertex3f(0.5f, 1.0f, 0.0);
	glNormal3f(0.0f, 0.0f, 1.0);
	glTexCoord2f(0.5f, 1.0f);

	glEnd();
}


void drawCube(float x, float y, float z, float size) {
	const float texture_higth = 1.0 / 3;
	const float texture_width = 1.0 / 4;
	const float texture_x_beg = 0.0;
	const float texture_y_beg = texture_higth;
	glTranslatef(-size / 2, -size / 2, -size / 2);

	glBegin(GL_QUAD_STRIP);

	glTexCoord2f(texture_x_beg, texture_y_beg);
	glVertex3f(x, y, z);

	glTexCoord2f(texture_x_beg, texture_y_beg + texture_higth);
	glVertex3f(x, y + size, z);

	glTexCoord2f(texture_x_beg + texture_width, texture_y_beg);
	glVertex3f(x + size, y, z);

	glTexCoord2f(texture_x_beg + texture_width, texture_y_beg + texture_higth);
	glVertex3f(x + size, y + size, z);

	glTexCoord2f(texture_x_beg + 2 * texture_width, texture_y_beg);
	glVertex3f(x + size, y, z + size);
	glTexCoord2f(texture_x_beg + 2 * texture_width, texture_y_beg + texture_higth);
	glVertex3f(x + size, y + size, z + size);

	glTexCoord2f(texture_x_beg + 3 * texture_width, texture_y_beg);
	glVertex3f(x, y, z + size);
	glTexCoord2f(texture_x_beg + 3 * texture_width, texture_y_beg + texture_higth);
	glVertex3f(x, y + size, z + size);

	glTexCoord2f(texture_x_beg + 4 * texture_width, texture_y_beg);
	glVertex3f(x, y, z);
	glTexCoord2f(texture_x_beg + 4 * texture_width, texture_y_beg + texture_higth);
	glVertex3f(x, y + size, z);

	glEnd();

	const float texture_x_beg_qd = 2 * texture_width;
	const float texture_y_beg_qd = texture_higth;

	const float texture_x_beg_qu = 2 * texture_width;
	const float texture_y_beg_qu = 3 * texture_higth;
	glBegin(GL_QUADS);



	glTexCoord2f(texture_x_beg_qd + texture_width, texture_y_beg_qd - texture_higth);
	glVertex3f(x, y, z);

	glTexCoord2f(texture_x_beg_qd - texture_width, texture_y_beg_qd - texture_higth);
	glVertex3f(x + size, y, z);

	glTexCoord2f(texture_x_beg_qd, texture_y_beg_qd);
	glVertex3f(x + size, y, z + size);

	glTexCoord2f(texture_x_beg_qd + texture_width, texture_y_beg_qd);
	glVertex3f(x, y, z + size);

	glTexCoord2f(texture_x_beg_qu, texture_y_beg_qu);
	glVertex3f(x, y + size, z);
	glTexCoord2f(texture_x_beg_qu + texture_width, texture_y_beg_qu);
	glVertex3f(x + size, y + size, z);
	glTexCoord2f(texture_x_beg_qu + texture_width, texture_y_beg_qu - texture_higth);
	glVertex3f(x + size, y + size, z + size);
	glTexCoord2f(texture_x_beg_qu, texture_y_beg_qu - texture_higth);
	glVertex3f(x, y + size, z + size);
	glEnd();



}

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

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{

	/*************************************************************************************/

	// Struktura dla nag��wka pliku  TGA


#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;


	/*************************************************************************************/

	// Warto�ci domy�lne zwracane w przypadku b��du

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	fopen_s(&pFile, FileName, "rb");
	if (pFile == NULL)
		return NULL;

	/*************************************************************************************/
	// Przeczytanie nag��wka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);


	/*************************************************************************************/

	// Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;


	/*************************************************************************************/
	// Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pami�ci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


	/*************************************************************************************/

	// Alokacja pami�ci dla danych obrazu


	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}


	/*************************************************************************************/

	// Ustawienie formatu OpenGL


	switch (sDepth)

	{

	case 3:

		*ImFormat = GL_BGR_EXT;

		*ImComponents = GL_RGB8;

		break;

	case 4:

		*ImFormat = GL_BGRA_EXT;

		*ImComponents = GL_RGBA8;

		break;

	case 1:

		*ImFormat = GL_LUMINANCE;

		*ImComponents = GL_LUMINANCE8;

		break;

	};



	fclose(pFile);



	return pbitsperpixel;

}

/*************************************************************************************/


void keys(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		textureChoice = First;
		break;
	case '2':
		textureChoice = Second;
		break;
	default:
		break;
	}

	// Tekstura
	GLbyte* pBytes = NULL;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	/*************************************************************************************/

// Teksturowanie b�dzie prowadzone tyko po jednej stronie �ciany

	//glEnable(GL_CULL_FACE);


	/*************************************************************************************/

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	switch (textureChoice) {
	case First:
		pBytes = LoadTGAImage("textures/british_cat_flip.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
		
		break;
	case Second:
		pBytes = LoadTGAImage("textures/znany_celebryta.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
		break;
	}
	if (pBytes == NULL) {
		throw std::exception();
	}


	/*************************************************************************************/

   // Zdefiniowanie tekstury 2-D

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	/*************************************************************************************/

	// Zwolnienie pami�ci

	free(pBytes);

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	/*************************************************************************************/

	// Okre�lenie sposobu nak�adania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy�ej

	// Rotate object on the screen
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);

	if (status == 1)                     // je�li lewy klawisz myszy wci�ni�ty
	{
		theta_m_x += delta_x * pix2angleX;    // modyfikacja k�ta obrotu o kat proporcjonalny
		theta_m_y += delta_y * pix2angleY;
	}                                  // do r�nicy po�o�e� kursora myszy

	glRotatef(theta_m_x, 0.0, 1.0, 0.0);  //obr�t obiektu o nowy k�t
	glRotatef(theta_m_y, 1.0, 0.0, 0.0);  //obr�t obiektu o nowy k�t
	// Render designed objects
	//printPot(); // OK
	//drawEggFromPoints(); // OK
	//drawEggFromLines(); // OK
	//drawEggFromTriangles();
	//drawEggFromTriangleStrips();

	// Tekstury
	//drawTextureTriangle();
	drawCube(0, 0, 0, 5);


	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania


	glutSwapBuffers();
	//
}

/*************************************************************************************/
 // Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
 // FileName, alokuje pami�� i zwraca wska�nik (pBits) do bufora w kt�rym
 // umieszczone s� dane.
 // Ponadto udost�pnia szeroko�� (ImWidth), wysoko�� (ImHeight) obrazu
 // tekstury oraz dane opisuj�ce format obrazu wed�ug specyfikacji OpenGL
 // (ImComponents) i (ImFormat).
 // Jest to bardzo uproszczona wersja funkcji wczytuj�cej dane z pliku TGA.
 // Dzia�a tylko dla obraz�w wykorzystuj�cych 8, 24, or 32 bitowy kolor.
 // Nie obs�uguje plik�w w formacie TGA kodowanych z kompresj� RLE.
/*************************************************************************************/



/*************************************************************************************/

// Funkcja ustalaj�ca stan renderowania



void MyInit(void)
{
	// Disable color fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

	srand(time(nullptr));

	createEggPointModel(50);

	// Tekstura
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	/*************************************************************************************/

// Teksturowanie b�dzie prowadzone tyko po jednej stronie �ciany

	//glEnable(GL_CULL_FACE);


	/*************************************************************************************/

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

	pBytes = LoadTGAImage("textures/british_cat_flip.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	if (pBytes == NULL) {
		throw std::exception();
	}


	/*************************************************************************************/

   // Zdefiniowanie tekstury 2-D

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	/*************************************************************************************/

	// Zwolnienie pami�ci

	free(pBytes);


	/*************************************************************************************/

	// W��czenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	/*************************************************************************************/

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*************************************************************************************/

	// Okre�lenie sposobu nak�adania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	// �wiat�o
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

	GLfloat mat_shininess = { 70.0 };
	// wsp�czynnik n opisuj�cy po�ysk powierzchni


/*************************************************************************************/
// Definicja �r�d�a �wiat�a


	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	// po�o�enie �r�d�a


	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

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


	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	///*************************************************************************************/
	//// Ustawienie parametr�w �r�d�a

	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	///*************************************************************************************/
	//// Ustawienie opcji systemu o�wietlania sceny

	//glShadeModel(GL_SMOOTH); // w�aczenie �agodnego cieniowania
	//glEnable(GL_LIGHTING);   // w�aczenie systemu o�wietlenia sceny
	//glEnable(GL_LIGHT0);     // w��czenie �r�d�a o numerze 0
	//glEnable(GL_DEPTH_TEST); // w��czenie mechanizmu z-bufora

/*************************************************************************************/



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

	glutCreateWindow("Textures");

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