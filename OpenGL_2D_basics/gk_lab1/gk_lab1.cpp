/*************************************************************************************/

// Troch� bardziej skomplikowany program wykorzystuj�cy funkcje biblioteki OpenGL

/*************************************************************************************/

#include "pch.h"

#include <windows.h>

#include <gl/gl.h>

#include <gl/glut.h>

#include <stdlib.h>
#include <time.h>

/*************************************************************************************/

typedef float point2[2];

// Funkcaja okre�laj�ca, co ma by� rysowane
// (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)

//kwadrat
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	// Czyszczenie okna aktualnym kolorem czyszcz�cym
//
//
//
//	glColor3f(0.0f, 1.0f, 0.0f);
//	// Ustawienie aktualnego koloru rysowania na zielony
//
//
//
//	glRectf(-50.0f, 50.0f, 50.0f, -50.0f);
//	// Narysowanie prostok�ta
//
//
//
//	glFlush();
//	// Przekazanie polece� rysuj�cych do wykonania
//
//}

//tr�jk�t(2 kolory)
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	// Czyszczenie okna aktualnym kolorem czyszcz�cym
//
//	glColor3f(0.0f, 1.0f, 0.0f);
//
//	// Ustawienie aktualnego koloru rysowania na zielony
//
//
//
//	glBegin(GL_TRIANGLES);       // Narysowanie zielonego tr�jk�ta
//
//	glVertex2f(0.0f, 0.0f);
//	glVertex2f(0.0f, 50.0f);
//	glVertex2f(50.0f, 0.0f);
//
//	glEnd();
//
//	glColor3f(1.0f, 0.0f, 0.0f);
//	// Ustawienie aktualnego koloru rysowania na czerwony
//
//	glBegin(GL_TRIANGLES); // Narysowanie czerwonego tr�jk�ta
//
//	glVertex2f(0.0f, 0.0f);
//	glVertex2f(0.0f, 50.0f);
//	glVertex2f(-50.0f, 0.0f);
//
//	glEnd();
//
//	glFlush();
//	// Przekazanie polece� rysuj�cych do wykonania
//
//}

// tr�jk�t (3 kolory, gradient)
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	glBegin(GL_TRIANGLES);
//
//	glColor3f(1.0f, 0.0f, 0.0f); // wierzcho�ek czerwony
//	glVertex2f(-50.0f, 0.0f);
//	glColor3f(0.0f, 1.0f, 0.0f); // wierzcho�ek zielony
//	glVertex2f(0.0f, 50.0f);
//	glColor3f(0.0f, 0.0f, 1.0f); // wierzcho�ek niebieski
//	glVertex2f(50.0f, 0.0f);
//	glEnd();
//
//	glFlush();
//
//}



/*************************************************************************************/

//[a, b)
int getRand(int a, int b) {
	return a + rand() % (b - a);
}

void drawRect(float x, float y, float a, float b) {
	
	glBegin(GL_TRIANGLES);
	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x, y);
	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x + a, y);
	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x, y + b);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x + a, y + b);
	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x + a, y);
	glColor3f(getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f, getRand(0, 256) / 255.0f);
	glVertex2f(x, y + b);

	glEnd();
}

void drawBlankRect(float x, float y, float a, float b) {
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_TRIANGLES);
	
	glVertex2f(x, y);
	glVertex2f(x + a, y);
	glVertex2f(x, y + b);

	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(x + a, y + b);
	glVertex2f(x + a, y);
	glVertex2f(x, y + b);

	glEnd();
}

void drawRandRect(float x, float y, float a, float b, float offset) {
	drawRect(x, y, a, b);
	drawRect(x, y, a + getRand(-offset, offset + 1), getRand(-offset, offset + 1));
	drawRect(x + a, y, getRand(-offset, offset + 1), b + getRand(-offset, offset + 1));
	drawRect(x, y + b, getRand(-offset, offset + 1), -b);
	drawRect(x + a, y + b, -a, getRand(-offset, offset + 1));
}

void drawBlankRandRect(float x, float y, float a, float b, float offset) {
	drawBlankRect(x, y, a, b);
	drawBlankRect(x, y, a + getRand(-offset, offset + 1), getRand(-offset, offset + 1));
	drawBlankRect(x + a, y, getRand(-offset, offset + 1), b + getRand(-offset, offset + 1));
	drawBlankRect(x, y + b, getRand(-offset, offset + 1), -b);
	drawBlankRect(x + a, y + b, -a, getRand(-offset, offset + 1));
}

void drawFractal(float x, float y, float a, float b) {
	drawRandRect(x, y, a, b, 5);
	drawBlankRandRect(x + a/3, y + b/3, a/3, b/3, 5);
}

void drawFractalRecursion(float x, float y, float a, float b, float offset, int lvl, int maxLvl) {
	if (lvl == 0) {
		drawRandRect(x, y, a, b, offset);
	}
	if (lvl == maxLvl) {
		return;
	}

	a /= 3;
	b /= 3;
	offset /= 1.5f;
	drawRandRect(x + a, y + b, a, b, offset);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (i == 2 && y == 2) {
				continue;
			}
			drawFractalRecursion(x + i * a , y + j * b, a, b , offset, lvl + 1, maxLvl);
		}
	}
}

void RenderScene(void)

{

	glClear(GL_COLOR_BUFFER_BIT);

	/*drawRect(0.0f, 0.0f, 20.0f, 70.0f);*/
	/*drawRandRect(0.0f, 0.0f, 50.0f, 50.0f, 10.0f);
	drawFractal(-50.0f, -50.0f, 100.0f, 100.0f);*/
	drawFractalRecursion(-50.0f, -50.0f, 75.0f, 100.0f, 5, 0, 4);

	glFlush();

}


// Funkcja ustalaj�ca stan renderowania
void MyInit(void)

{

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// Kolor okna wn�trza okna - ustawiono na szary

}

/*************************************************************************************/

// Funkcja s�u��ca do kontroli zachowania proporcji rysowanych obiekt�w
// niezale�nie od rozmiar�w okna graficznego



void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s�
// przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna

{

	GLfloat AspectRatio;

	// Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna



	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0

		vertical = 1;


	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna urz�dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)


	glMatrixMode(GL_PROJECTION);
	// Okre�lenie uk�adu wsp�rz�dnych obserwatora

	glLoadIdentity();
	// Okre�lenie przestrzeni ograniczaj�cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// okre�lenie okna obserwatora.
	// Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
	// Do okre�lenia okna obserwatora s�u�y funkcja glOrtho(...)



	if (horizontal <= vertical)

		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else

		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okre�lenie uk�adu wsp�rz�dnych    

	glLoadIdentity();

}

/*************************************************************************************/

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli



void main(void)

{
	srand(time(0));

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wy�wietlania
	// GLUT_SINGLE - pojedynczy bufor wy�wietlania
	// GLUT_RGBA - model kolor�w RGB



	glutCreateWindow("Drugi program w OpenGL");
	// Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna


	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
	// Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy
	// trzeba b�dzie przerysowa� okno


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie 
	// inicjalizacje konieczneprzed przyst�pieniem do renderowania


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/