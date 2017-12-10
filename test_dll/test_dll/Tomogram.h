#pragma once
#include"glew.h"

enum TYP_PRZEKROJU
{
	BSKAN,PRZEKROJ_SZEROKOSC,PRZEKROJ_WYSOKOSC
};

class Tomogram {

	unsigned int b_skan_rozmiar_x;
	unsigned int b_skan_rozmiar_y;
	unsigned int liczba_B_skanow;
	unsigned int odstep;
	GLuint *indeksy_tekstur_ping;
	GLuint *indeksy_tekstur_pong;
	unsigned int liczba_Przekrojow_Szerokosc;
	unsigned int liczba_Przekrojow_Wysokosc;
	bool zainicjalizowane = false;

public:
	explicit Tomogram(unsigned int rozmiar_x, unsigned int rozmiar_y, unsigned int liczba, unsigned int przerwa = 0.5f) {

		b_skan_rozmiar_x = rozmiar_x;
		b_skan_rozmiar_y = rozmiar_y;
		liczba_B_skanow = liczba;
		odstep = przerwa;
	//	liczba_Przekrojow_Szerokosc = b_skan_rozmiar_x / odstep;//pewnie mo¿na by sprawdzaæ czy liczba jest podzielna...
	//	liczba_Przekrojow_Wysokosc = b_skan_rozmiar_y / odstep;
		liczba_Przekrojow_Szerokosc = 100;
		liczba_Przekrojow_Wysokosc = 100;
	}

	unsigned int liczbaTekstur() const { return liczba_B_skanow + liczba_Przekrojow_Szerokosc + liczba_Przekrojow_Wysokosc; }
	void inline parametryTekstur(unsigned int *indeksyTekstur, unsigned int ileTekstur,TYP_PRZEKROJU typ ) {

		unsigned int szerokoscTekstury = 0;
		unsigned int wysokoscTekstury = 0;

		switch (typ) {

		case BSKAN:
			szerokoscTekstury = b_skan_rozmiar_x;
			wysokoscTekstury = b_skan_rozmiar_y;
			break;
		case PRZEKROJ_SZEROKOSC:
			//szerokoscTekstury = (liczba_B_skanow - 1)*odstep;
			//wysokoscTekstury = b_skan_rozmiar_y;
			szerokoscTekstury = b_skan_rozmiar_x;
			wysokoscTekstury = b_skan_rozmiar_y;
			break;
		case PRZEKROJ_WYSOKOSC:
			//szerokoscTekstury = b_skan_rozmiar_x;
			//wysokoscTekstury = (liczba_B_skanow - 1)*odstep;
			szerokoscTekstury = b_skan_rozmiar_x;
			wysokoscTekstury = b_skan_rozmiar_y;
			break;
		}

		for (unsigned int i = 0, stop = ileTekstur; i != stop; ++i) {

			glBindTexture(GL_TEXTURE_2D, indeksyTekstur[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, szerokoscTekstury, wysokoscTekstury, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint* inicjalizacjaTekstur() {

		GLuint* indeksy = new GLuint[liczbaTekstur()];
		glGenTextures(liczbaTekstur(), indeksy);
		parametryTekstur(indeksy, liczba_B_skanow, BSKAN);
		parametryTekstur(indeksy + liczba_B_skanow, liczba_Przekrojow_Szerokosc, PRZEKROJ_SZEROKOSC);
		parametryTekstur(indeksy + liczba_B_skanow + liczba_Przekrojow_Szerokosc, liczba_Przekrojow_Wysokosc, PRZEKROJ_WYSOKOSC);
		return indeksy;
	}

	void init() {

		indeksy_tekstur_ping = inicjalizacjaTekstur();
		indeksy_tekstur_pong = inicjalizacjaTekstur();
		zainicjalizowane = true;
	}

	GLuint* indeksy_ping() { return indeksy_tekstur_ping; }
	GLuint* indeksy_pong() { return indeksy_tekstur_pong; }

	~Tomogram() {

		if (zainicjalizowane) {

			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(liczbaTekstur(), indeksy_tekstur_ping);
			glDeleteTextures(liczbaTekstur(), indeksy_tekstur_pong);
			delete[] indeksy_tekstur_ping;
			delete[] indeksy_tekstur_pong;
		}
	}
};
