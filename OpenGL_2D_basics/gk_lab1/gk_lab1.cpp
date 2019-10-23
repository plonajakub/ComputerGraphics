/*************************************************************************************/

// Trochê bardziej skomplikowany program wykorzystuj¹cy funkcje biblioteki OpenGL

/*************************************************************************************/

#include "pch.h"

#include <windows.h>

#include <gl/gl.h>

#include <gl/glut.h>

#include <stdlib.h>
#include <time.h>

/*************************************************************************************/

typedef float point2[2];

// Funkcaja okreœlaj¹ca, co ma byæ rysowane
// (zawsze wywo³ywana, gdy trzeba przerysowaæ scenê)

//kwadrat
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
//
//
//
//	glColor3f(0.0f, 1.0f, 0.0f);
//	// Ustawienie aktualnego koloru rysowania na zielony
//
//
//
//	glRectf(-50.0f, 50.0f, 50.0f, -50.0f);
//	// Narysowanie prostok¹ta
//
//
//
//	glFlush();
//	// Przekazanie poleceñ rysuj¹cych do wykonania
//
//}

//trójk¹t(2 kolory)
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
//
//	glColor3f(0.0f, 1.0f, 0.0f);
//
//	// Ustawienie aktualnego koloru rysowania na zielony
//
//
//
//	glBegin(GL_TRIANGLES);       // Narysowanie zielonego trójk¹ta
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
//	glBegin(GL_TRIANGLES); // Narysowanie czerwonego trójk¹ta
//
//	glVertex2f(0.0f, 0.0f);
//	glVertex2f(0.0f, 50.0f);
//	glVertex2f(-50.0f, 0.0f);
//
//	glEnd();
//
//	glFlush();
//	// Przekazanie poleceñ rysuj¹cych do wykonania
//
//}

// trójk¹t (3 kolory, gradient)
//void RenderScene(void)
//
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	glBegin(GL_TRIANGLES);
//
//	glColor3f(1.0f, 0.0f, 0.0f); // wierzcho³ek czerwony
//	glVertex2f(-50.0f, 0.0f);
//	glColor3f(0.0f, 1.0f, 0.0f); // wierzcho³ek zielony
//	glVertex2f(0.0f, 50.0f);
//	glColor3f(0.0f, 0.0f, 1.0f); // wierzcho³ek niebieski
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


// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)

{

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// Kolor okna wnêtrza okna - ustawiono na szary

}

/*************************************************************************************/

// Funkcja s³u¿¹ca do kontroli zachowania proporcji rysowanych obiektów
// niezale¿nie od rozmiarów okna graficznego



void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokoœæ i wysokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem, gdy zmieni siê rozmiar okna

{

	GLfloat AspectRatio;

	// Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna



	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0

		vertical = 1;


	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœciokna okna urz¹dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)


	glMatrixMode(GL_PROJECTION);
	// Okreœlenie uk³adu wspó³rzêdnych obserwatora

	glLoadIdentity();
	// Okreœlenie przestrzeni ograniczaj¹cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wspó³czynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// okreœlenie okna obserwatora.
	// Pozwala to zachowaæ w³aœciwe proporcje rysowanego obiektu
	// Do okreœlenia okna obserwatora s³u¿y funkcja glOrtho(...)



	if (horizontal <= vertical)

		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else

		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okreœlenie uk³adu wspó³rzêdnych    

	glLoadIdentity();

}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



void main(void)

{
	srand(time(0));

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyœwietlania
	// GLUT_SINGLE - pojedynczy bufor wyœwietlania
	// GLUT_RGBA - model kolorów RGB



	glutCreateWindow("Drugi program w OpenGL");
	// Utworzenie okna i okreœlenie treœci napisu w nag³ówku okna


	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback)
	// Biblioteka GLUT bêdzie wywo³ywa³a t¹ funkcjê za ka¿dym razem, gdy
	// trzeba bêdzie przerysowaæ okno


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy¿ej) wykonuje wszelkie 
	// inicjalizacje konieczneprzed przyst¹pieniem do renderowania


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/