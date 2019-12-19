/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzednych

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

static GLfloat theta_m_x = 0.0;   // k¹t obrotu obiektu

static GLfloat theta_m_y = 0.0;   // k¹t obrotu obiektu
static GLfloat pix2angleX;     // przelicznik pikseli na stopnie
static GLfloat pix2angleY;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciœniêto ¿adnego klawisza
							   // 1 - naciœniêty zostaæ lewy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_x = 0;        // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹
									  // i poprzedni¹ kursora myszy

static int y_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_y = 0;        // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹
									  // i poprzedni¹ kursora myszy

enum Texture { First, Second };
Texture textureChoice = First;

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

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartoœci odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
							 // jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;          // wciêniêty zosta³ lewy klawisz myszy
	}
	else

		status = 0;          // nie zosta³ wciêniêty ¿aden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy
	delta_y = y - y_pos_old;

	x_pos_old = x;            // podstawienie bie¿¹cego po³o¿enia jako poprzednie
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
//	glutPostRedisplay(); //odœwie¿enie zawartoœci aktualnego okna
//}

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{

	/*************************************************************************************/

	// Struktura dla nag³ówka pliku  TGA


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

	// Wartoœci domyœlne zwracane w przypadku b³êdu

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	fopen_s(&pFile, FileName, "rb");
	if (pFile == NULL)
		return NULL;

	/*************************************************************************************/
	// Przeczytanie nag³ówka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);


	/*************************************************************************************/

	// Odczytanie szerokoœci, wysokoœci i g³êbi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;


	/*************************************************************************************/
	// Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pamiêci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


	/*************************************************************************************/

	// Alokacja pamiêci dla danych obrazu


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

// Teksturowanie bêdzie prowadzone tyko po jednej stronie œciany

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

	// Zwolnienie pamiêci

	free(pBytes);

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	/*************************************************************************************/

	// Okreœlenie sposobu nak³adania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	if (status == 1)                     // jeœli lewy klawisz myszy wciêniêty
	{
		theta_m_x += delta_x * pix2angleX;    // modyfikacja k¹ta obrotu o kat proporcjonalny
		theta_m_y += delta_y * pix2angleY;
	}                                  // do ró¿nicy po³o¿eñ kursora myszy

	glRotatef(theta_m_x, 0.0, 1.0, 0.0);  //obrót obiektu o nowy k¹t
	glRotatef(theta_m_y, 1.0, 0.0, 0.0);  //obrót obiektu o nowy k¹t
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
	// Przekazanie poleceñ rysuj¹cych do wykonania


	glutSwapBuffers();
	//
}

/*************************************************************************************/
 // Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
 // FileName, alokuje pamiêæ i zwraca wskaŸnik (pBits) do bufora w którym
 // umieszczone s¹ dane.
 // Ponadto udostêpnia szerokoœæ (ImWidth), wysokoœæ (ImHeight) obrazu
 // tekstury oraz dane opisuj¹ce format obrazu wed³ug specyfikacji OpenGL
 // (ImComponents) i (ImFormat).
 // Jest to bardzo uproszczona wersja funkcji wczytuj¹cej dane z pliku TGA.
 // Dzia³a tylko dla obrazów wykorzystuj¹cych 8, 24, or 32 bitowy kolor.
 // Nie obs³uguje plików w formacie TGA kodowanych z kompresj¹ RLE.
/*************************************************************************************/



/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania



void MyInit(void)
{
	// Disable color fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny

	srand(time(nullptr));

	createEggPointModel(50);

	// Tekstura
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	/*************************************************************************************/

// Teksturowanie bêdzie prowadzone tyko po jednej stronie œciany

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

	// Zwolnienie pamiêci

	free(pBytes);


	/*************************************************************************************/

	// W³¹czenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	/*************************************************************************************/

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*************************************************************************************/

	// Okreœlenie sposobu nak³adania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	// Œwiat³o
	/*************************************************************************************/

//  Definicja materia³u z jakiego zrobiony jest czajnik
//  i definicja Ÿród³a œwiat³a

/*************************************************************************************/


/*************************************************************************************/
// Definicja materia³u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego               

	GLfloat mat_shininess = { 70.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni


/*************************************************************************************/
// Definicja Ÿród³a œwiat³a


	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	// po³o¿enie Ÿród³a


	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_linear = { 0.05 };
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic = { 0.001 };
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

/*************************************************************************************/
// Ustawienie parametrów materia³u i Ÿród³a œwiat³a

/*************************************************************************************/
// Ustawienie patrametrów materia³u


	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	///*************************************************************************************/
	//// Ustawienie parametrów Ÿród³a

	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	///*************************************************************************************/
	//// Ustawienie opcji systemu oœwietlania sceny

	//glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	//glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
	//glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	//glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora

/*************************************************************************************/



}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	pix2angleX = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	pix2angleY = 360.0 / (float)vertical;  // przeliczenie pikseli na stopnie

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

	glutCreateWindow("Textures");

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

	glutMouseFunc(Mouse);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
	return 0;
}

/*************************************************************************************/