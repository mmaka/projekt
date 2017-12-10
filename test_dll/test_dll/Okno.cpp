#include"Okno.h"
#include "glew.h"
#include "wglew.h"
#include "Werteks.h"
#include "math.h"
#include "Wektor.h"
#include "Pliki.h"
#include<sstream>
#include<iostream>
#include<fstream>
#include<vector>

LRESULT CALLBACK __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


LRESULT __stdcall Okno::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		szerokoscObszaruUzytkownika = rect.right - rect.left;
		wysokoscObszaruUzytkownika = rect.bottom - rect.top;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0L;
}

bool Okno::ZmianaRozdzieloczosci(long szerokosc, long wysokosc, long glebiaKolorow) const {

	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = szerokosc;
	dmScreenSettings.dmPelsHeight = wysokosc;
	dmScreenSettings.dmBitsPerPel = glebiaKolorow;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;

}


bool Okno::Inicjuj(HINSTANCE uchwytAplikacji, POINT polozenieOkna, POINT rozmiarOkna, bool trybPelnoekranowy, bool zmianaRozdzielczosci) {

	char nazwaOkna[] = "Aplikacja OpenGL";

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = uchwytAplikacji;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = nazwaOkna;

	if (RegisterClassEx(&wc) == 0) return false;


	DWORD stylOkna = WS_OVERLAPPEDWINDOW;

	if (trybPelnoekranowy) {

		stylOkna = WS_POPUP;
		polozenieOkna.x = 0;
		polozenieOkna.y = 0;

		if (zmianaRozdzielczosci) {

			rozmiarOkna.x = 1024;
			rozmiarOkna.y = 768;
			if (!ZmianaRozdzieloczosci(rozmiarOkna.x, rozmiarOkna.y)) return false;

		}
		else {

			RECT rozmiarEkranu;
			GetWindowRect(GetDesktopWindow(), &rozmiarEkranu);
			rozmiarOkna.x = rozmiarEkranu.right - rozmiarEkranu.left;
			rozmiarOkna.y = rozmiarEkranu.bottom - rozmiarEkranu.top;

		}

	}
	uchwytOkna = CreateWindow(nazwaOkna, nazwaOkna, stylOkna, polozenieOkna.x, polozenieOkna.y, rozmiarOkna.x, rozmiarOkna.y, NULL, NULL, uchwytAplikacji, NULL);

	if (uchwytOkna == NULL) return false;

	ShowWindow(uchwytOkna, SW_SHOW);
	UpdateWindow(uchwytOkna);

	return true;
}

WPARAM Okno::Uruchom() {

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


bool OknoGL::UstalFomatPikseli(HDC uchwytDC) const {

	PIXELFORMATDESCRIPTOR opisFormatuPikseli;
	ZeroMemory(&opisFormatuPikseli, sizeof(opisFormatuPikseli));
	opisFormatuPikseli.nVersion = 1;
	opisFormatuPikseli.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	opisFormatuPikseli.iPixelType = PFD_TYPE_RGBA;
	opisFormatuPikseli.cColorBits = 32;
	opisFormatuPikseli.cDepthBits = 32;
	opisFormatuPikseli.iLayerType = PFD_MAIN_PLANE;
	int formatPikseli = ChoosePixelFormat(uchwytDC, &opisFormatuPikseli);
	if (formatPikseli == 0) return false;
	if (!SetPixelFormat(uchwytDC, formatPikseli, &opisFormatuPikseli)) return false;

	return true;
}

bool OknoGL::InicjujWGL(HWND uchwytOkna) {

	uchwytDC = ::GetDC(uchwytOkna);

	if (!UstalFomatPikseli(uchwytDC)) return false;

	HGLRC uchwytTymczasowegoRC = wglCreateContext(uchwytDC);
	if (uchwytTymczasowegoRC == NULL) return false;

	if (!wglMakeCurrent(uchwytDC, uchwytTymczasowegoRC)) return false;

	GLenum err = glewInit();
	if (GLEW_OK != err) {

		MessageBox(NULL, "Inicjacja biblioteki GLEW nie powiodla sie", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
		return false;
	}

	const int major_min = 4;
	const int minor_min = 3;
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (major < major_min || (major == major_min && minor < minor_min)) {

		MessageBox(NULL, "Wersja OpenGL jest niewystrczajaca", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
		return false;
	}

	int atrybuty[] = {

		WGL_CONTEXT_MAJOR_VERSION_ARB,major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB,minor_min,
		//	WGL_CONTEXT_FLAGS_ARB,WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,//WGL_CONTEXT_DEBUG_BIT_ARB,//WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, <-- dodane WGL_CONTEXT_DEBUG_BIT_ARG
		WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	uchwytRC = wglCreateContextAttribsARB(uchwytDC, 0, atrybuty);

	if (uchwytRC == NULL) return false;
	if (!wglMakeCurrent(uchwytDC, uchwytRC)) return false;

	wglDeleteContext(uchwytTymczasowegoRC);
	return true;
}


void OknoGL::UsunWGL() {

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(uchwytRC);
	::ReleaseDC(uchwytOkna, uchwytDC);
}

LRESULT __stdcall OknoGL::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	long wynik = Okno::WndProc(hWnd, message, wParam, lParam);

	static POINT poprzedniaPozycjaKursoraMyszyLMB = { -1,-1 };
	static POINT poprzedniaPozycjaKursoraMyszyRMB = { -1,-1 };

	const bool swobodneObrotyKameryMozliwe = true;
	const int identyfikatorTimeraSwobodnychObrotowKamery = 1;
	const int okresTimeraSwobodnychObrotowKamery = 50;

	const int identyfikatorTimeraRysowania = 2;
	const int okresTimeraRysowania = 1000;

	switch (message) {

	case WM_CREATE:
		if (!InicjujWGL(hWnd)) {

			MessageBox(NULL, "Pobranie kontekstu renderowania nie powodlo sie", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
		}

		idProgramuShaderow = PrzygotujShadery("Basic.vert", "Basic.frag", true);

		if (idProgramuShaderow == NULL) {

			MessageBox(NULL, "Przygotowanie shaderow nie powiodlo sie", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			exit(EXIT_FAILURE);
		}

		UmiescInformacjeNaPaskuTytulu(hWnd);

	//poni¿sze potrzebne do cudy
	//	cti = new cuda_texture_interface(300);
	//	cti->inicjalizacja();
		
//		if (teksturowanieWlaczone)
//			PrzygotujTekstury(300, new char*[2]{ "kwadrat_czer.bmp","kwadrat_czer2.bmp" });
//		else
//			glUniform1i(glGetUniformLocation(idProgramuShaderow, "Teksturowanie"), false);

		tom = new Tomogram(500, 500, 100);
		tom->init();
		cti = new Cuda_OpenGL_Interop(300,500,500);
		cti->init();
		poj = new PojemnikNaTekstury(300);
		cti->rejestrowanieTekstur(tom->indeksy_ping(), poj->res1(), poj->arr1(), 300, poj->str(), GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone, 0, 0);
		cti->rejestrowanieTekstur(tom->indeksy_pong(), poj->res2(), poj->arr2(), 300, poj->str(), GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone, 0, 0);
		poj->surf_init();
	//	poj->wpiszTekstureWCudaArray();
		MessageBox(NULL, "Przygotowanie tekstur", "", NULL);

		liczbaAktorow = PrzygotujAktorow();

		UstawienieSceny();
		if (swobodneObrotyKameryMozliwe) {

			if (SetTimer(hWnd, identyfikatorTimeraSwobodnychObrotowKamery, okresTimeraSwobodnychObrotowKamery, NULL) == 0)
				MessageBox(hWnd, "Nie udalo sie ustawic timera swobodnych obrotow kamery", "Blad", MB_OK | MB_ICONERROR);
		}

		if (SetTimer(hWnd, identyfikatorTimeraRysowania, okresTimeraRysowania, NULL) == 0)
			MessageBox(hWnd, "nie udalo sie ustawic timera rysowanie", "Blad", MB_OK | MB_ICONERROR);


		QueryPerformanceFrequency(&countPerSec);
		QueryPerformanceCounter(&tim1);

		break;
	case WM_SIZE:
		UstawienieSceny();
		break;
	case WM_PAINT:
		RysujScene();
		ValidateRect(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		ObslugaKlawiszy(wParam);
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
			ObliczaniePrzesunieciaMyszy(lParam, 3.0f, poprzedniaPozycjaKursoraMyszyLMB, &OknoGL::ObslugaMyszyZWcisnietymLewymPrzyciskiem);

		if (wParam & MK_RBUTTON)
			ObliczaniePrzesunieciaMyszy(lParam, 3.0f, poprzedniaPozycjaKursoraMyszyRMB, &OknoGL::ObslugaMyszyZWcisnietymPrawymPrzyciskiem);
		break;
	case WM_MOUSEWHEEL:
		ObslugaRolkiMyszy(wParam);
		break;
	case WM_LBUTTONUP:
		poprzedniaPozycjaKursoraMyszyLMB = { -1,-1 };
		break;
	case WM_RBUTTONUP:
		poprzedniaPozycjaKursoraMyszyRMB = { -1,-1 };
		break;
	case WM_TIMER:
		switch (wParam) {

		case identyfikatorTimeraSwobodnychObrotowKamery:
			if (swobodneObrotyKameryAktywne) SwobodneObrotyKamery(false);
			break;
		case identyfikatorTimeraRysowania:
			//cti->akcja(flaga);
		//	flaga = !flaga;
			RysujScene();
			break;
		}
		wynik = 0;
		break;
	case WM_DESTROY:
		UsunTekstury();
		UsunAktorow();
		UsunWGL();
		KillTimer(uchwytOkna, identyfikatorTimeraSwobodnychObrotowKamery);
		break;
	}

	return wynik;
}


void OknoGL::UmiescInformacjeNaPaskuTytulu(HWND uchwytOkna) {

	char bufor[256];
	GetWindowText(uchwytOkna, bufor, 256);
	const GLubyte* wersja = glGetString(GL_VERSION);
	strcat_s(bufor, " | OpenGL ");
	strcat_s(bufor, (char*)wersja);
	const GLubyte* dostawca = glGetString(GL_VENDOR);
	strcat_s(bufor, " | ");
	strcat_s(bufor, (char*)dostawca);
	const GLubyte* kartaGraficzna = glGetString(GL_RENDERER);
	strcat_s(bufor, " | ");
	strcat_s(bufor, (char*)kartaGraficzna);
	const GLubyte* wersjaGLEW = glewGetString(GLEW_VERSION);
	strcat_s(bufor, " | GLEW ");
	strcat_s(bufor, (char*)wersjaGLEW);
	const GLubyte* wersjaGLSL = glGetString(GL_SHADING_LANGUAGE_VERSION);
	strcat_s(bufor, " | GLSL ");
	strcat_s(bufor, (char*)wersjaGLSL);

	SetWindowText(uchwytOkna, bufor);
}

void OknoGL::UstawienieSceny(bool rzutowanieIzometryczne) {

	glViewport(0, 0, szerokoscObszaruUzytkownika, wysokoscObszaruUzytkownika);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	//Parametry shadera 
	GLint parametrUwzglednijKolorWerteksu = glGetUniformLocation(idProgramuShaderow, "UwzglednijKolorWerteksu");
	glUniform1i(parametrUwzglednijKolorWerteksu, true);
	GLint parametrKolor = glGetUniformLocation(idProgramuShaderow, "Kolor");
	float kolor[4] = { 1.0,1.0,0.0,1.0 };
	glUniform4fv(parametrKolor, 1, kolor);

	GLint parametrPrzezroczystosc = glGetUniformLocation(idProgramuShaderow, "przezroczystosc");
	float przezroczystosc = 0.005f;
	glUniform1f(parametrPrzezroczystosc, przezroczystosc);

	GLint parametrMacierzSwiata = glGetUniformLocation(idProgramuShaderow, "macierzSwiata");
	//macierzSwiata.ZwiazZIdentyfikatorem(parametrMacierzSwiata, true);

	GLint parametrMacierzWidoku = glGetUniformLocation(idProgramuShaderow, "macierzWidoku");
	macierzWidoku.UstawJednostkowa();
	macierzWidoku *= Macierz4::Przesuniecie(0, 0, -3);
//	macierzWidoku.ZwiazZIdentyfikatorem(parametrMacierzWidoku, true);

	GLint parametrMacierzRzutowania = glGetUniformLocation(idProgramuShaderow, "macierzRzutowania");
//	macierzRzutowania.ZwiazZIdentyfikatorem(parametrMacierzRzutowania, false);


	GLint parametrMVP = glGetUniformLocation(idProgramuShaderow, "mvp");
	MVP.ZwiazZIdentyfikatorem(parametrMVP, true);


	float wsp = wysokoscObszaruUzytkownika / (float)szerokoscObszaruUzytkownika;
	if (!rzutowanieIzometryczne)
		macierzRzutowania.UstawRzutPerspektywiczny(-1.0f, 1.0f, wsp*-1.0f, wsp*1.0f, 1.0f, 10.0f);
	else
		macierzRzutowania.UstawRzutIzometryczny(-1.0f, 1.0f, wsp*-1.0f, wsp*1.0f, 1.0f, 10.0f);

//	macierzRzutowania.PrzeslijWartosc();

	VP.Ustaw(macierzRzutowania);
	VP.PomnozZPrawej(macierzWidoku);
//	GLuint parametrMacierzNormalnych = glGetUniformLocation(idProgramuShaderow, "macierzNormalnych");
//	macierzNormalnych = macierzSwiata.Odwrotna().Transponowana();
//	macierzNormalnych.ZwiazZIdentyfikatorem(parametrMacierzNormalnych, true);
}


void OknoGL::RysujScene() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RysujAktorow();//uwaga na wiele watkow str 242 Nowoczesny OpenGL


	SwapBuffers(uchwytDC);
	glFinish();
	QueryPerformanceCounter(&tim2);
	double time = (double)(tim2.QuadPart - tim1.QuadPart) / countPerSec.QuadPart * 1000;
	tim1 = tim2;
	std::stringstream s;
	s << "Czas: " << time << " milisekund | FPS: " << 1 / (time / 1000) << std::endl;
	SetWindowText(uchwytOkna, s.str().c_str());

}

void PokazKomunikat(char* tekst, UINT typ = 0) {

	MessageBoxA(NULL, tekst, "Aplikacja OpenGL - Kompilacja shaderow", MB_OK | typ);
}

char*  __stdcall CzytajPlikTxt(const char* nazwaPliku, int rozmiarBufora, char* bufor) {

	std::ifstream plik_we(nazwaPliku);
	if (plik_we.fail()) return NULL;
	plik_we.read(bufor, rozmiarBufora);
	if (plik_we.eof()) {

		std::streamsize odczytane = plik_we.gcount();
		plik_we.close();
		//bufor[odczytane] = "\0";
		memmove(bufor + odczytane, "\0", 1);
		return bufor;

	}
	else {

		plik_we.close();
		return NULL;

	}
}

unsigned int OknoGL::KompilujShader(const char* nazwaPliku, GLenum typ, bool trybDebugowania) {

	const int maksymalnaWielkoscKodu = 65535;

	//Wczytanie kodu z pliku
	char kodShadera[maksymalnaWielkoscKodu] = "";
	if (CzytajPlikTxt(nazwaPliku, maksymalnaWielkoscKodu, kodShadera) == NULL) {
		PokazKomunikat("Brak lub niemozliwe odczyytanie pliku shadera", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) PokazKomunikat(kodShadera);

	//Tworzenie obiektu shadera
	GLuint idShadera = glCreateShader(typ);
	if (idShadera == NULL) return NULL;

	//Dostarczenie zrodla do obiektu shadera
	const GLchar* zrodlo[1];
	zrodlo[0] = kodShadera;
	glShaderSource(idShadera, 1, zrodlo, NULL);

	//Kompilacja shadera
	glCompileShader(idShadera);

	//Weryfikacja kompilacji
	GLint powodzenie;
	glGetShaderiv(idShadera, GL_COMPILE_STATUS, &powodzenie);
	if (!powodzenie) {
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetShaderInfoLog(idShadera, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwage! Kompilacja shadera nie powodla sie:\n";
		strcat_s(komunikat, (char*)infoLog);
		PokazKomunikat(komunikat, MB_ICONERROR);
		return NULL;

	}
	else if (trybDebugowania) PokazKomunikat("Kompilacja shadera zakonczyla sie sukcesem", MB_ICONINFORMATION);

	return idShadera;
}

unsigned int OknoGL::PrzygotujShadery(const char* vsNazwaPliku, const char* fsNazwaPliku, bool trybDebugowania) {

	//KomKompilacja shadera werteksow
	GLuint idShaderaWerteksow = KompilujShader(vsNazwaPliku, GL_VERTEX_SHADER, trybDebugowania);

	if (idShaderaWerteksow == NULL) {

		PokazKomunikat("Kompilacja shadera werteksow nie powiodla sie", MB_ICONERROR);
		return NULL;

	}
	else if (trybDebugowania) PokazKomunikat("Kompilacja shadera werteksow zakonczyla sie sukcesem", MB_ICONINFORMATION);

	//Kompilacja shadera fragmentow
	GLuint idShaderaFragmentow = KompilujShader(fsNazwaPliku, GL_FRAGMENT_SHADER, trybDebugowania);

	if (idShaderaFragmentow == NULL) {

		PokazKomunikat("Kompilacja shadera fragmentow nie powodila sie", MB_ICONERROR);
		return NULL;

	}
	else if (trybDebugowania) PokazKomunikat("Kompilacja shadera fragmentow zakonczyla sie sukcesem", MB_ICONINFORMATION);

	//Tworzenie obiektu programu
	GLuint idProgramu = glCreateProgram();

	//Przylaczenie shaderow
	glAttachShader(idProgramu, idShaderaWerteksow);
	glAttachShader(idProgramu, idShaderaFragmentow);

	//Linkowanie
	glLinkProgram(idProgramu);

	//Weryfikacja linkowania
	GLint powodzenie;
	glGetProgramiv(idProgramu, GL_LINK_STATUS, &powodzenie);

	if (!powodzenie) {

		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Linkowanie programu shaderow nie powiodlo sie:\n";
		strcat_s(komunikat, (char*)infoLog);
		PokazKomunikat(komunikat, MB_ICONERROR);
		return NULL;

	}
	else if (trybDebugowania) PokazKomunikat("Linkowanie programu shaderow powiodlo sie", MB_ICONINFORMATION);

	//Walidacja programu
	glGetProgramiv(idProgramu, GL_VALIDATE_STATUS, &powodzenie);

	if (!powodzenie) {

		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Walidacja programu shaderow nie powiodla sie:\n";
		strcat_s(komunikat, (char*)infoLog);
		PokazKomunikat(komunikat, MB_ICONERROR);
		return NULL;

	}
	else if (trybDebugowania) PokazKomunikat("Walidacja programu shaderow powiodla sie", MB_ICONINFORMATION);

	//Uzycie programu
	glUseProgram(idProgramu);
	//Usuwanie niepotrzebnych obiektow shadera
	glDeleteShader(idShaderaWerteksow);
	glDeleteShader(idShaderaFragmentow);

	return idProgramu;
}

void OknoGL::ObslugaKlawiszy(WPARAM wParam) {

	const float kat = 5.0f;
	Macierz4 m = Macierz4::Jednostkowa;
	switch (wParam) {

	case 27:
		SendMessage(uchwytOkna, WM_CLOSE, 0, 0);
		break;
	case VK_F6:
	
		break;
	case VK_F5:
	{
		bool komunikatNaPaskuTytulu = true;
		switch (trybKontroliKamery) {

		case tkkFPP:
			trybKontroliKamery = tkkTPP;
			if (komunikatNaPaskuTytulu)
				SetWindowText(uchwytOkna, "Tryb kontroli kamery: TPP");

			break;

		case tkkTPP:
			trybKontroliKamery = tkkArcBall;
			if (komunikatNaPaskuTytulu)
				SetWindowText(uchwytOkna, "Tryb kontroli kamery: ArcBall");

			break;

		case tkkArcBall:
			trybKontroliKamery = tkkModel;
			if (komunikatNaPaskuTytulu)
				SetWindowText(uchwytOkna, "Tryb kontroli kamery: Model");

			break;

		case tkkModel:
			trybKontroliKamery = tkkFPP;
			if (komunikatNaPaskuTytulu)
				SetWindowText(uchwytOkna, "tryb kontroli kamery: FPP");

			break;
		}
	}
	MessageBeep(MB_OK);
	break;
	case 'A':
	case VK_LEFT:
		m = Macierz4::ObrotY(kat);
		break;
	case 'D':
	case VK_RIGHT:
		m = Macierz4::ObrotY(-kat);
		break;
	case 'W':
	case VK_UP:
		m = Macierz4::ObrotX(kat);
		break;
	case 'S':
	case VK_DOWN:
		m = Macierz4::ObrotX(-kat);
		break;
	case VK_OEM_COMMA:
		m = Macierz4::ObrotZ(kat);
		break;
	case VK_OEM_PERIOD:
		m = Macierz4::ObrotZ(-kat);
		break;
	case VK_OEM_MINUS:
		m = Macierz4::Przesuniecie(0, 0, 0.1f);
		break;
	case VK_OEM_PLUS:
		m = Macierz4::Przesuniecie(0, 0, -0.1f);
		break;
	}

	ModyfikujPolozenieKamery(m);
	RysujScene();

}
//zwraca polozenie kamery w ukladzie sceny
Wektor3 OknoGL::PobierzPolozenieKamery(bool pominObroty) const {

	Wektor3 w3;
	if (pominObroty) {

		Wektor4 w4 = macierzWidoku.KopiaKolumny(3);
		for (int i = 0; i < 3; ++i) w3[i] = -w4[i];

	}
	else {

		Wektor4 w4 = macierzWidoku.Odwrotna().KopiaKolumny(3);
		for (int i = 0; i < 3; ++i) w3[i] = w4[i];

	}

	return w3;
}

float OknoGL::OdlegloscKamery() const {

	return PobierzPolozenieKamery().Dlugosc();
}

void OknoGL::ModyfikujPolozenieKamery(Macierz4 macierzPrzeksztalcenia) {

	switch (trybKontroliKamery) {

	case tkkFPP:
		macierzWidoku.PomnozZLewej(macierzPrzeksztalcenia);
		break;
	case tkkTPP:
	{
		const float odlegloscCentrumOdKamery = 3.0f;
		macierzWidoku.PomnozZLewej(Macierz4::Przesuniecie(0, 0, -odlegloscCentrumOdKamery)*macierzPrzeksztalcenia*Macierz4::Przesuniecie(0, 0, odlegloscCentrumOdKamery));

	}
	break;
	case tkkArcBall:
	{
		Wektor3 polozenieKamery = PobierzPolozenieKamery(true);
		macierzWidoku.PomnozZLewej(Macierz4::Przesuniecie(-polozenieKamery)*macierzPrzeksztalcenia*Macierz4::Przesuniecie(polozenieKamery));
	}
	break;
	case tkkModel:
		macierzWidoku.PomnozZPrawej(macierzPrzeksztalcenia);
	}
//	macierzWidoku.PrzeslijWartosc();

	VP.Ustaw(macierzRzutowania);
	VP.PomnozZPrawej(macierzWidoku);

//	float polozenieKamery[3];
//	PobierzPolozenieKamery(false).KopiujElementy(polozenieKamery);
//	GLuint parametrPolozenieKamery = glGetUniformLocation(idProgramuShaderow, "PolozenieKamery");
//	glUniform3fv(parametrPolozenieKamery, 1, polozenieKamery);
}

void OknoGL::ObliczaniePrzesunieciaMyszy(const LPARAM lParam, const float prog, POINT& poprzedniaPozycjaKursoraMyszy, TypMetodyObslugujacejPrzesuniecieMyszy MetodaObslugujacaPrzesuniecieMyszy) {

	if (poprzedniaPozycjaKursoraMyszy.x == -1 && poprzedniaPozycjaKursoraMyszy.y == -1) {
		poprzedniaPozycjaKursoraMyszy = { LOWORD(lParam), HIWORD(lParam) };
		return;
	}

	POINT biezacaPozycjaKursoraMyszy = { LOWORD(lParam),HIWORD(lParam) };
	POINT przesuniecie;
	przesuniecie.x = biezacaPozycjaKursoraMyszy.x - poprzedniaPozycjaKursoraMyszy.x;
	przesuniecie.y = biezacaPozycjaKursoraMyszy.y - poprzedniaPozycjaKursoraMyszy.y;

	float przesuniecieKursoraMyszy = (float)sqrt(przesuniecie.x*przesuniecie.x + przesuniecie.y*przesuniecie.y);

	if (przesuniecieKursoraMyszy > prog) {
		//prog w pikselach - zapobiega zbyt czestym zmianom - utrata dokladnosci

		(this->*MetodaObslugujacaPrzesuniecieMyszy)(biezacaPozycjaKursoraMyszy, przesuniecie);
		poprzedniaPozycjaKursoraMyszy = biezacaPozycjaKursoraMyszy;

	}
}

void OknoGL::ObslugaMyszyZWcisnietymLewymPrzyciskiem(POINT biezacaPozycjaKursoraMyszy, POINT przesuniecieKursoraMyszy) {

	const float wspX = 360.0f / szerokoscObszaruUzytkownika;
	const float wspY = 360.0f / wysokoscObszaruUzytkownika;
	float dx = przesuniecieKursoraMyszy.x *wspX;
	float dy = przesuniecieKursoraMyszy.y *wspY;
	Macierz4 m = Macierz4::ObrotXYZ(dy, dx, 0);
	ModyfikujPolozenieKamery(m);
	RysujScene();

	SwobodneObrotyKamery(true, dx, dy, 0.95f);
}

void OknoGL::ObslugaMyszyZWcisnietymPrawymPrzyciskiem(POINT biezacaPozycjaKursoraMyszy, POINT przesuniecieKursoraMyszy) {

	//skalowanie
	const float wspX = 5.0f / szerokoscObszaruUzytkownika;
	const float wspY = 5.0f / wysokoscObszaruUzytkownika;
	float dx = przesuniecieKursoraMyszy.x * wspX;
	float dy = przesuniecieKursoraMyszy.y * wspY;

	Macierz4 m = Macierz4::Przesuniecie(dx, -dy, 0);
	ModyfikujPolozenieKamery(m);
	RysujScene();

}

void OknoGL::ObslugaRolkiMyszy(WPARAM wParam) {

	const float czulosc = 10.0f;
	short zmianaPozycjiRolki = (short)HIWORD(wParam);
	float przesuniecie = zmianaPozycjiRolki / abs(zmianaPozycjiRolki) / czulosc;
	ModyfikujPolozenieKamery(Macierz4::Przesuniecie(0, 0, przesuniecie));
	RysujScene();
}

void OknoGL::SwobodneObrotyKamery(const bool inicjacja, const float poczatkowe_dx, const float poczatkowe_dy, const float wspolczynnikWygaszania) {

	static float dx = 0;
	static float dy = 0;
	static float wsp = 0;
	if (inicjacja) {

		swobodneObrotyKameryAktywne = true;
		dx = poczatkowe_dx;
		dy = poczatkowe_dy;
		wsp = wspolczynnikWygaszania;
		if (wsp < 0) wsp = 0;
		if (wsp > 1) wsp = 1;

		return;

	}
	else {

		dx *= wsp;
		dy *= wsp;
		if (fabs(dx) < 0.001f && fabs(dy) < 0.001f)
			swobodneObrotyKameryAktywne = false;

		Macierz4 m = Macierz4::ObrotXYZ(dy, dx, 0);
		ModyfikujPolozenieKamery(m);
		RysujScene();
	}

}

unsigned int OknoGL::PrzygotujAktorow() {

	GLuint atrybutPolozenie = glGetAttribLocation(idProgramuShaderow, "polozenie_in");
	if (atrybutPolozenie == (GLuint)-1) atrybutPolozenie = 0;

	GLuint atrybutNormalna = glGetAttribLocation(idProgramuShaderow, "normalna_in");
	if (atrybutNormalna == (GLuint)-1) atrybutNormalna = 1;

	GLuint atrybutWspolrzedneTeksturowania = glGetAttribLocation(idProgramuShaderow, "wspTekstur_in");
	if (atrybutWspolrzedneTeksturowania == (GLuint)-1) atrybutWspolrzedneTeksturowania = 2;

	GLuint atrybutKolor = glGetAttribLocation(idProgramuShaderow, "kolor_in");
	if (atrybutKolor == (GLuint)-1) atrybutKolor = 3;

	int liczbaAktorow = 300;
	aktorzy = new Aktor*[liczbaAktorow];
	//te dane bêd¹ wynika³y z liczby przekrojów do wyœwietlenia - póki co na oko
	float obszarB = 0.7;
	float obszarPSzer = 0.7;
	float obszarPWys = 0.7;
	int iloscBskanow = 100;
	float przezro = 0.005f;

	for (int i = 0; i < iloscBskanow; ++i) {

		float z = (((float)i * 2) / iloscBskanow - 1)*obszarB;
		aktorzy[i] = new Prostokat(atrybutPolozenie, atrybutNormalna, atrybutWspolrzedneTeksturowania, atrybutKolor);
		aktorzy[i]->MacierzSwiata = Macierz4::Przesuniecie(0.0f, 0.0f, -z);
	//	aktorzy[i]->IndeksTekstury = (teksturowanieWlaczone) ? indeksyTekstur[i] : -1;
		aktorzy[i]->IndeksTekstury = (teksturowanieWlaczone) ? *(tom->indeksy_ping()+i) : -1;
	//	aktorzy[i]->IndeksTekstury2 = (teksturowanieWlaczone) ? indeksyTekstur2[i] : -1;
		aktorzy[i]->IndeksTekstury2 = (teksturowanieWlaczone) ? *(tom->indeksy_pong() + i) : -1;
		aktorzy[i]->przezroczystosc = przezro;

	}

	int iloscPrzekrojowSzer = 100;

	for (int i = 0; i < iloscPrzekrojowSzer; ++i) {

		float x = (((float)i * 2) / iloscPrzekrojowSzer - 1)*obszarPSzer;
		aktorzy[iloscBskanow + i] = new Prostokat(atrybutPolozenie, atrybutNormalna, atrybutWspolrzedneTeksturowania, atrybutKolor);
		aktorzy[iloscBskanow + i]->MacierzSwiata = Macierz4::Przesuniecie(-x, 0.0f, 0.0f)*Macierz4::ObrotY(-90);
		//aktorzy[i + iloscBskanow]->IndeksTekstury = (teksturowanieWlaczone) ? indeksyTekstur[i + iloscBskanow] : -1;
		aktorzy[i + iloscBskanow]->IndeksTekstury = (teksturowanieWlaczone) ? *(tom->indeksy_ping()+i + iloscBskanow) : -1;
		//aktorzy[i + iloscBskanow]->IndeksTekstury2 = (teksturowanieWlaczone) ? indeksyTekstur2[i + iloscBskanow] : -1;
		aktorzy[i + iloscBskanow]->IndeksTekstury2 = (teksturowanieWlaczone) ? *(tom->indeksy_pong() + i + iloscBskanow) : -1;
		aktorzy[i + iloscBskanow]->przezroczystosc = przezro;

	}

	int iloscPrzekrojowWys = 100;

	for (int i = 0; i < iloscPrzekrojowWys; ++i) {

		float y = (((float)i * 2) / iloscPrzekrojowWys - 1)*obszarPWys;
		aktorzy[iloscBskanow + iloscPrzekrojowSzer + i] = new Prostokat(atrybutPolozenie, atrybutNormalna, atrybutWspolrzedneTeksturowania, atrybutKolor);
		aktorzy[iloscBskanow + iloscPrzekrojowSzer + i]->MacierzSwiata = Macierz4::Przesuniecie(0.0f, -y, 0.0f)*Macierz4::ObrotX(-270);
		//aktorzy[i + iloscBskanow + iloscPrzekrojowSzer]->IndeksTekstury = (teksturowanieWlaczone) ? indeksyTekstur[i + iloscBskanow + iloscPrzekrojowSzer] : -1;
		aktorzy[i + iloscBskanow + iloscPrzekrojowSzer]->IndeksTekstury = (teksturowanieWlaczone) ? *(tom->indeksy_ping()+i + iloscBskanow + iloscPrzekrojowSzer) : -1;
		//aktorzy[i + iloscBskanow + iloscPrzekrojowSzer]->IndeksTekstury2 = (teksturowanieWlaczone) ? indeksyTekstur2[i + iloscBskanow + iloscPrzekrojowSzer] : -1;
		aktorzy[i + iloscBskanow + iloscPrzekrojowSzer]->IndeksTekstury2 = (teksturowanieWlaczone) ? *(tom->indeksy_pong() + i + iloscBskanow + iloscPrzekrojowSzer) : -1;
		aktorzy[i + iloscBskanow + iloscPrzekrojowSzer]->przezroczystosc = przezro;

	}
	
	return liczbaAktorow;
}

void OknoGL::RysujAktorow() {
	
//	std::vector<unsigned int> v; 
//	poj->generujIndeksy(v);
	//v = poj->generujIndeksy();
	//v.swap(poj->generujIndeksy());

	for (unsigned int i = 0; i < liczbaAktorow; ++i) {

		//zachowuje zwiazek z macierza shaderow, z tego powodu nie nalezy uzywac operatora =
		macierzSwiata.Ustaw(aktorzy[i]->MacierzSwiata);
//		macierzSwiata.PrzeslijWartosc();
		MVP.Ustaw(VP);
		
		MVP.PomnozZPrawej(aktorzy[i]->MacierzSwiata);
		MVP.PrzeslijWartosc();
//		try {
//			macierzNormalnych.Ustaw(macierzSwiata.Odwrotna().Transponowana());
//		}
//		catch (std::exception exc) {

//			MessageBox(NULL, "Error", "Error", MB_OK | MB_ICONERROR);
//			macierzNormalnych.Ustaw(macierzSwiata);

//		}
//		macierzNormalnych.PrzeslijWartosc();
	
		if (teksturowanieWlaczone) {

			if (aktorzy[i]->IndeksTekstury != -1) {

				glUniform1i(glGetUniformLocation(idProgramuShaderow, "Teksturowanie"), true);
				glBindTexture(GL_TEXTURE_2D, (flaga) ? aktorzy[i]->IndeksTekstury : aktorzy[i]->IndeksTekstury2);
			//	glBindTexture(GL_TEXTURE_2D, *(tom->indeksy_ping()+v[i]));
			}
			else {

				glUniform1i(glGetUniformLocation(idProgramuShaderow, "Teksturowanie"), false);
			}

			glUniform1f(glGetUniformLocation(idProgramuShaderow, "przezroczystosc"), aktorzy[i]->przezroczystosc);

		}

		aktorzy[i]->Rysuj();
	}
}

void OknoGL::UsunAktorow() {

	for (unsigned int i = 0; i < liczbaAktorow; ++i)
		delete aktorzy[i];

	delete[] aktorzy;
	liczbaAktorow = 0;
}

void OknoGL::PrzygotujTekstury(unsigned int liczbaTekstur, char** nazwyPlikowTekstur) {

	indeksyTekstur = new GLuint[liczbaTekstur];
	indeksyTekstur2 = new GLuint[liczbaTekstur];
	glGenTextures(liczbaTekstur, indeksyTekstur);
	glGenTextures(liczbaTekstur, indeksyTekstur2);

	int szerokoscTekstury;
	int wysokoscTekstury;

	for (unsigned int i = 0; i < liczbaTekstur; ++i) {

		glBindTexture(GL_TEXTURE_2D, indeksyTekstur[i]);
		//kolejne polecenia beda dotyczyc tekstury o identyfikatorze przechowywanym w polu tabeli indeksyTekstur[i]

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//wczytywanie tekstury z pliku
		unsigned long* tekstura = WczytajObrazZPlikuBitmap(uchwytOkna, nazwyPlikowTekstur[0], szerokoscTekstury, wysokoscTekstury, false, 255);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, szerokoscTekstury, wysokoscTekstury, 0, GL_RGBA, GL_UNSIGNED_BYTE, tekstura);
		//gdy powy¿sze ma jako ostatni argument NULL, a poni¿sze jest odkomentowane to wyœwietlane dane pochodz¹ od Cudy
		//cti->zarejestrujTeksture(true, i, indeksyTekstur[i], GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone, 0, 0, tekstura, szerokoscTekstury, wysokoscTekstury);
		
		glBindTexture(GL_TEXTURE_2D, indeksyTekstur2[i]);
		//kolejne polecenia beda dotyczyc tekstury o identyfikatorze przechowywanym w polu tabeli indeksyTekstur[i]

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		unsigned long* tekstura2 = WczytajObrazZPlikuBitmap(uchwytOkna, nazwyPlikowTekstur[1], szerokoscTekstury, wysokoscTekstury, false, 255);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, szerokoscTekstury, wysokoscTekstury, 0, GL_RGBA, GL_UNSIGNED_BYTE, tekstura2);

		//cti->zarejestrujTeksture(false, i, indeksyTekstur2[i], GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone, 0, 0, tekstura2, szerokoscTekstury, wysokoscTekstury);
	
		delete[] tekstura;
		delete[] tekstura2;
	}

	glBindTexture(GL_TEXTURE_2D, NULL);
	//cti->odmapowanieTekstur();
}
/*
void OknoGL::PrzygotujTekstury() {

	unsigned int ileTekstur = 150;
	//unsigned int ileTekstur = 50;
	indeksyTekstur = new GLuint[ileTekstur];
	glGenTextures(ileTekstur, indeksyTekstur);

	int ileBskanow = daneOCT->iloscBskanowDoWyswietlenia;
	int glebokosc = daneOCT->glebokosc;
	float wsp = ((float)glebokosc) / ileBskanow;

	int nrBskanuDoWys;
	bool kolorowo = true;

	for (int i = 0; i < ileBskanow; ++i) {

		nrBskanuDoWys = floor(i*wsp);
		daneOCT->TworzTekstureBSkan(indeksyTekstur[i], nrBskanuDoWys);
		//daneOCT->TworzTeksture(indeksyTekstur[i], Bskan, nrBskanuDoWys,kolorowo);
	}

	int szerokosc = daneOCT->szer;
	int ilePrzekrojowDoWys = daneOCT->iloscPrzekrojowDoWyswietleniaSzerokosc;
	wsp = ((float)szerokosc) / ilePrzekrojowDoWys;
	
	int nrPrzekDoWys;

	for (int i = 0; i < ilePrzekrojowDoWys; ++i) {

	nrPrzekDoWys = floor(i*wsp);
	daneOCT->TworzTeksturePrzekPop(indeksyTekstur[ileBskanow + i], nrPrzekDoWys);
	//daneOCT->TworzTeksture(indeksyTekstur[ileBskanow + i], PrzekPop, nrPrzekDoWys, kolorowo);
	}

	int wysokosc = daneOCT->wys;
	int ilePrzekrojowPozDoWys = daneOCT->iloscPrzekrojowDoWyswietleniaWysokosc;

	wsp = ((float)wysokosc) / ilePrzekrojowPozDoWys;

	for (int i = 0; i < ilePrzekrojowPozDoWys; ++i) {

	int nrPrzekPozDoWys = floor(i*wsp);
	daneOCT->TworzTeksturePrzekPoz(indeksyTekstur[i + ileBskanow+ilePrzekrojowDoWys], nrPrzekPozDoWys);
	//daneOCT->TworzTeksture(indeksyTekstur[i + ileBskanow + ilePrzekrojowDoWys],PrzekPoz, nrPrzekPozDoWys,kolorowo);
	}

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glBindTexture(GL_TEXTURE_2D, NULL);
}
*/


void OknoGL::PobierzDane() {

	//wysy³am stan flagi
	//poj->producentTekstur(poj->pakiet(), !flaga);
	poj->old_dostawcaTekstur(poj->pakiet(), flaga);
	flaga = !flaga;
	poj->old_dostawcaTekstur(poj->pakiet(), flaga);
	flaga = !flaga;
}

void OknoGL::PrzekazWskaznik(pakiet_danych& p) {


//	poj->przekazWskaznik(poj->p);

}


void OknoGL::UsunTekstury() {

	glBindTexture(GL_TEXTURE_2D, NULL);
	glDeleteTextures(liczbaTekstur, indeksyTekstur);
	glDeleteTextures(liczbaTekstur, indeksyTekstur2);
	delete[] indeksyTekstur;
	delete[] indeksyTekstur2;
	liczbaTekstur = 0;
}