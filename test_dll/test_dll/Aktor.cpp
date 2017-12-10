#include "Aktor.h"

void Aktor::InicjujBuforWerteksow() {

	//Vertex Array Object (VAO)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Vertex Buffer Object (VBO)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	Werteks* werteksy = NULL;
	liczbaWerteksow = TworzTabliceWerteksow(werteksy);
	glBufferData(GL_ARRAY_BUFFER, liczbaWerteksow * sizeof(Werteks), werteksy, GL_STATIC_DRAW);
	delete[] werteksy;

}

void Aktor::Inicjuj(GLuint atrybutPolozenie, GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania, GLuint atrybutKolor) {

	InicjujBuforWerteksow();

	glVertexAttribPointer(atrybutPolozenie, Werteks::liczbaWpolrzednychPolozenia, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, 0);
	glEnableVertexAttribArray(atrybutPolozenie);
//	glVertexAttribPointer(atrybutNormalna, Werteks::liczbaWspolrzednychNormalnej, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, (const GLvoid *)Werteks::rozmiarWektoraPolozenia);
//	glEnableVertexAttribArray(atrybutNormalna);
//	glVertexAttribPointer(atrybutWspolrzedneTeksturowania, Werteks::liczbaWspolrzednychTeksturowania, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, (const GLvoid*)(Werteks::rozmiarWektoraPolozenia + Werteks::rozmiarNormalnej));
	glVertexAttribPointer(atrybutWspolrzedneTeksturowania, Werteks::liczbaWspolrzednychTeksturowania, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, (const GLvoid*)(Werteks::rozmiarWektoraPolozenia));
	glEnableVertexAttribArray(atrybutWspolrzedneTeksturowania);
//	glVertexAttribPointer(atrybutKolor, Werteks::liczbaSkladowychKoloru, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, (const GLvoid*)(Werteks::rozmiarWektoraPolozenia+Werteks::rozmiarNormalnej+Werteks::rozmiarWspolrzednychTeksturowania));
	glVertexAttribPointer(atrybutKolor, Werteks::liczbaSkladowychKoloru, GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, (const GLvoid*)(Werteks::rozmiarWektoraPolozenia + Werteks::rozmiarWspolrzednychTeksturowania));
	glEnableVertexAttribArray(atrybutKolor);

}

Aktor::Aktor() 
	:	liczbaWerteksow(-1),MacierzSwiata(Macierz4::Jednostkowa), 
		MaterialSwiatloOtoczenia(Wektor4(0.1f,0.1f,0.1f,1)),MaterialSwiatloRozpraszane(Wektor4(1,1,1,1)),MaterialSwiatloRozblysku(Wektor4(0,1,0,1)),MaterialWykladnikRozblysku(100),przezroczystosc(1.0f),
		IndeksTekstury(-1) {

}


void Aktor::UsunBuforWerteksow() {

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

Aktor::~Aktor() {

	UsunBuforWerteksow();
}

void Aktor::Rysuj() {

	assert(liczbaWerteksow > 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, liczbaWerteksow);
	//glBindBuffer(GL_ARRAY_BUFFER,NULL);
	//glBindVertexArray(NULL);
}

Kwadrat::Kwadrat(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedzi) : Aktor(), dlugoscKrawedzi(dlugoscKrawedzi) {

	Inicjuj(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor);
}


unsigned int Kwadrat::TworzTabliceWerteksow(Werteks*& werteksy) {

	const float x0 = dlugoscKrawedzi / 2.0f;
	const float y0 = dlugoscKrawedzi / 2.0f;

	werteksy = new Werteks[4];
//	werteksy[0] = Werteks(-x0, -y0, 0.0f,0,0,1, 1, 1, 0, 1);
//	werteksy[1] = Werteks(x0, -y0, 0.0f, 0, 0, 1, 1, 0, 1, 1);
//	werteksy[2] = Werteks(-x0, y0, 0.0f, 0, 0, 1, 0, 1, 1, 1);
//	werteksy[3] = Werteks(x0, y0, 0.0f, 0, 0, 1, 1, 1, 1, 1);

	werteksy[0] = Werteks(-x0, -y0, 0.0f, 0, 0, 0, 1,1,1);
	werteksy[1] = Werteks(x0, -y0, 0.0f, 1, 0, 1, 1,1,1);
	werteksy[2] = Werteks(-x0, y0, 0.0f, 0, 1, 1, 1,1, 1);
	werteksy[3] = Werteks(x0, y0, 0.0f, 1, 1, 1, 1,1,1);

	return 4;
}

Prostopadloscian::Prostopadloscian(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedziX, float dlugoscKrawedziY, float dlugoscKrawedziZ, bool koloruj)
	:Aktor(), dlugoscKrawedziX(dlugoscKrawedziX), dlugoscKrawedziY(dlugoscKrawedziY), dlugoscKrawedziZ(dlugoscKrawedziZ), koloruj(koloruj) {

	Inicjuj(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor);
}

unsigned int Prostopadloscian::TworzTabliceWerteksow(Werteks*& werteksy) {

	const float x0 = dlugoscKrawedziX / 2.0f;
	const float y0 = dlugoscKrawedziY / 2.0f;
	const float z0 = dlugoscKrawedziZ / 2.0f;

	werteksy = new Werteks[24];

	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;

	for (int i = 0; i < 6; ++i) {

		pierwszeWerteksyPodciagow[i] = i * 4;
		liczbyWerteksowWPodciagach[i] = 4;
	}

	
	if (koloruj) {

		r = 1.0f;
		g = 0.0f;
		b = 0.0f;

	}
	//tylna
//	werteksy[0] = Werteks(x0, -y0, -z0,0.0f,0.0f,-1.0f,0.0f,0.0f, r, g, b);
//	werteksy[1] = Werteks(-x0, -y0, -z0, 0.0f, 0.0f, -1.0f,1.0f,0.0f, r, g, b);
//	werteksy[2] = Werteks(x0, y0, -z0, 0.0f, 0.0f, -1.0f, 0.0f,1.0f,r, g, b);
//	werteksy[3] = Werteks(-x0, y0, -z0, 0.0f, 0.0f, -1.0f, 1.0f,1.0f,r, g, b);
	//przednia
//	werteksy[4] = Werteks(-x0, -y0, z0, 0.0f, 0.0f, 1.0f,0.0f,0.0f, r, g, b);
//	werteksy[5] = Werteks(x0, -y0, z0, 0.0f, 0.0f, 1.0f,1.0f,0.0f, r, g, b);
//	werteksy[6] = Werteks(-x0, y0, z0, 0.0f, 0.0f, 1.0f,0.0f,1.0f, r, g, b);
//	werteksy[7] = Werteks(x0, y0, z0, 0.0f, 0.0f, 1.0f,1.0f,1.0f, r, g, b);

	werteksy[0] = Werteks(x0, -y0, -z0, 0.0f, 0.0f, r, g, b);
	werteksy[1] = Werteks(-x0, -y0, -z0,1.0f, 0.0f, r, g, b);
	werteksy[2] = Werteks(x0, y0, -z0,0.0f, 1.0f, r, g, b);
	werteksy[3] = Werteks(-x0, y0, -z0,1.0f, 1.0f, r, g, b);
	//przednia
	werteksy[4] = Werteks(-x0, -y0, z0,0.0f, 0.0f, r, g, b);
	werteksy[5] = Werteks(x0, -y0, z0,1.0f, 0.0f, r, g, b);
	werteksy[6] = Werteks(-x0, y0, z0,0.0f, 1.0f, r, g, b);
	werteksy[7] = Werteks(x0, y0, z0,1.0f, 1.0f, r, g, b);


	if (koloruj) {

		r = 0.0f;
		g = 1.0f;
		b = 0.0f;
	}

	//prawa
//	werteksy[8] = Werteks(x0, -y0, z0, 1.0f, 0.0f, 0.0f,0.0f,0.0f, r, g, b);
//	werteksy[9] = Werteks(x0, -y0, -z0, 1.0f, 0.0f, 0.0f,1.0f,0.0f, r, g, b);
//	werteksy[10] = Werteks(x0, y0, z0, 1.0f, 0.0f, 0.0f,0.0f,1.0f, r, g, b);
//	werteksy[11] = Werteks(x0, y0, -z0, 1.0f, 0.0f, 0.0f,1.0f,1.0f, r, g, b);
	//lewa
//	werteksy[12] = Werteks(-x0, -y0, -z0, -1.0f, 0.0f, 0.0f,0.0f,0.0f, r, g, b);
//	werteksy[13] = Werteks(-x0, -y0, z0, -1.0f, 0.0f, 0.0f,1.0f,0.0f, r, g, b);
//	werteksy[14] = Werteks(-x0, y0, -z0, -1.0f, 0.0f, 0.0f,0.0f,1.0f, r, g, b);
//	werteksy[15] = Werteks(-x0, y0, z0, -1.0f, 0.0f, 0.0f,1.0f,1.0f, r, g, b);
	
	werteksy[8] = Werteks(x0, -y0, z0,0.0f, 0.0f, r, g, b);
	werteksy[9] = Werteks(x0, -y0, -z0,1.0f, 0.0f, r, g, b);
	werteksy[10] = Werteks(x0, y0, z0,0.0f, 1.0f, r, g, b);
	werteksy[11] = Werteks(x0, y0, -z0, 1.0f, 1.0f, r, g, b);
	//lewa
	werteksy[12] = Werteks(-x0, -y0, -z0, 0.0f, 0.0f, r, g, b);
	werteksy[13] = Werteks(-x0, -y0, z0,  1.0f, 0.0f, r, g, b);
	werteksy[14] = Werteks(-x0, y0, -z0, 0.0f, 1.0f, r, g, b);
	werteksy[15] = Werteks(-x0, y0, z0, 1.0f, 1.0f, r, g, b);

	if (koloruj) {

		r = 0.0f;
		g = 0.0f;
		b = 1.0f;
	}

//	werteksy[16] = Werteks(-x0, y0, z0, 0.0f, 1.0f, 0.0f,0.0f,0.0f, r, g, b);
//	werteksy[17] = Werteks(x0, y0, z0, 0.0f, 1.0f, 0.0f, 1.0f,0.0f,r, g, b);
//	werteksy[18] = Werteks(-x0, y0, -z0, 0.0f, 1.0f, 0.0f,0.0f,1.0f, r, g, b);
//	werteksy[19] = Werteks(x0, y0, -z0, 0.0f, 1.0f, 0.0f,1.0f,1.0f, r, g, b);
	//dolna

//	werteksy[20] = Werteks(-x0, -y0, -z0, 0.0f, -1.0f, 0.0f,0.0f,0.0f, r, g, b);
//	werteksy[21] = Werteks(x0, -y0, -z0, 0.0f, -1.0f, 0.0f,1.0f,0.0f, r, g, b);
//	werteksy[22] = Werteks(-x0, -y0, z0, 0.0f, -1.0f, 0.0f,0.0f,1.0f, r, g, b);//tutaj s,t chyba powinny byc 0.0f,1.0f
//	werteksy[23] = Werteks(x0, -y0, z0, 0.0f, -1.0f, 0.0f,1.0f,1.0f, r, g, b);//tutaj s,t chyba powinny byc 1.0f,1.0f

	werteksy[16] = Werteks(-x0, y0, z0,  0.0f, 0.0f, r, g, b);
	werteksy[17] = Werteks(x0, y0, z0, 1.0f, 0.0f, r, g, b);
	werteksy[18] = Werteks(-x0, y0, -z0, 0.0f, 1.0f, r, g, b);
	werteksy[19] = Werteks(x0, y0, -z0, 1.0f, 1.0f, r, g, b);
	//dolna

	werteksy[20] = Werteks(-x0, -y0, -z0, 0.0f, 0.0f, r, g, b);
	werteksy[21] = Werteks(x0, -y0, -z0, 1.0f, 0.0f, r, g, b);
	werteksy[22] = Werteks(-x0, -y0, z0, 0.0f, 1.0f, r, g, b);
	werteksy[23] = Werteks(x0, -y0, z0, 1.0f, 1.0f, r, g, b);

	return 24;
}

void Prostopadloscian::Rysuj() {

	//bez bufora indeksow

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glMultiDrawArrays(GL_TRIANGLE_STRIP, pierwszeWerteksyPodciagow, liczbyWerteksowWPodciagach, 6);
	//glBindBuffer(GL_ARRAY_BUFFER,NULL);
	//glBindVertexArray(NULL);
}

AktorZBuforemIndeksow::AktorZBuforemIndeksow()
	:Aktor(), liczbaIndeksow(-1) {

}

AktorZBuforemIndeksow::~AktorZBuforemIndeksow() {

	Aktor::~Aktor();
	UsunBuforIndeksow();
}

void AktorZBuforemIndeksow::Inicjuj(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor) {

	Aktor::Inicjuj(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor);
	InicjujBuforIndeksow();
}

void AktorZBuforemIndeksow::InicjujBuforIndeksow() {

	glGenBuffers(1, &vbo_indeksy);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indeksy);

	GLuint* indeksy = NULL;
	liczbaIndeksow = TworzTabliceIndeksow(indeksy);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, liczbaIndeksow * sizeof(GLuint), indeksy, GL_STATIC_DRAW);
	delete[] indeksy;
}

void AktorZBuforemIndeksow::UsunBuforIndeksow() {

	glDeleteBuffers(1, &vbo_indeksy);
}

void AktorZBuforemIndeksow::Rysuj() {

	assert(liczbaIndeksow > 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indeksy);
	glDrawElements(GL_TRIANGLE_STRIP, liczbaIndeksow, GL_UNSIGNED_INT, 0);
	//glBindBuffer(GL_ARRAY_BUFFER,NULL);
	//glBindVertexArray(NULL);

}

KwadratZBuforemIndeksow::KwadratZBuforemIndeksow(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedzi)
	:AktorZBuforemIndeksow(),dlugoscKrawedzi(dlugoscKrawedzi){

	Inicjuj(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor);
}

unsigned int KwadratZBuforemIndeksow::TworzTabliceWerteksow(Werteks*& werteksy) {

	const float x0 = dlugoscKrawedzi / 2.0f;
	const float y0 = dlugoscKrawedzi / 2.0f;

	werteksy = new Werteks[5];
//	werteksy[0] = Werteks(-x0, -y0, 0.0f,0,0,1, 1, 0, 0, 1);
//	werteksy[1] = Werteks(x0, -y0, 0.0f,0,0,1, 0, 1, 0, 1);
//	werteksy[2] = Werteks(0, y0, 0.0f,0,0,1, 0, 0, 1, 1);
//	werteksy[3] = Werteks(-x0, y0, 0.0f,0,0,1, 0, 0, 1, 1);
//	werteksy[4] = Werteks(x0, y0, 0.0f,0,0,1, 0, 0, 1, 1);

	werteksy[0] = Werteks(-x0, -y0, 0.0f,0, 0, 1, 1,1,1);
	werteksy[1] = Werteks(x0, -y0, 0.0f, 0, 1, 1, 1, 1, 1);
	werteksy[2] = Werteks(0, y0, 0.0f, 0, 1, 1, 1, 1, 1);
	werteksy[3] = Werteks(-x0, y0, 0.0f, 1, 0, 1, 1, 1, 1);
	werteksy[4] = Werteks(x0, y0, 0.0f, 1, 1, 1, 1, 1, 1);

	return 5;
}

unsigned int KwadratZBuforemIndeksow::TworzTabliceIndeksow(GLuint*& indeksy) {

	indeksy = new GLuint[4];
	indeksy[0] = 0;
	indeksy[1] = 1;
	indeksy[2] = 3;
	indeksy[3] = 4;

	return 4;
}

////////////////////
Prostokat::Prostokat(GLuint atrybutPolozenie, GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania, GLuint atrybutKolor, float dlugoscKrawedziX, float dlugoscKrawedziY) : Aktor(), dlugoscKrawedziX(dlugoscKrawedziX),dlugoscKrawedziY(dlugoscKrawedziY) {

	Inicjuj(atrybutPolozenie, atrybutNormalna, atrybutWspolrzedneTeksturowania, atrybutKolor);
}


unsigned int Prostokat::TworzTabliceWerteksow(Werteks*& werteksy) {

	const float x0 = dlugoscKrawedziX / 2.0f;
	const float y0 = dlugoscKrawedziY / 2.0f;

	werteksy = new Werteks[4];
//	werteksy[0] = Werteks(-x0, -y0, 0.0f, 0, 0,0, 0.0f, 0.0f);
//	werteksy[1] = Werteks(x0, -y0, 0.0f, 0, 0,0, 1.0f, 0.0f);
//	werteksy[2] = Werteks(-x0, y0, 0.0f, 0, 0,0, 0.0f, 1.0f);
//	werteksy[3] = Werteks(x0, y0, 0.0f, 0, 0,0, 1.0f, 1.0f);

	werteksy[0] = Werteks(-x0, -y0, 0.0f, 0.0f, 0.0f);
	werteksy[1] = Werteks(x0, -y0, 0.0f, 1.0f, 0.0f);
	werteksy[2] = Werteks(-x0, y0, 0.0f, 0.0f, 1.0f);
	werteksy[3] = Werteks(x0, y0, 0.0f, 1.0f, 1.0f);


	return 4;
}
