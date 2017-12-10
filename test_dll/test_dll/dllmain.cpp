#include<cstdlib>
#include<cstdio>
#include<thread>
#include"Okno.h"

HINSTANCE hInstance;
OknoGL okno;


BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpReserved){

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, "Process attached","MessageBox", MB_OK);
			
		hInstance = hinstDLL;

		break;
	case DLL_THREAD_ATTACH:
	//	MessageBox(NULL, "Thread attached", "MessageBox", MB_OK);
		
		break;
	case DLL_THREAD_DETACH:
	//	MessageBox(NULL, "Thread detached", "MessageBox", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, "Process detached", "MessageBox", MB_OK);
		break;
	}
	return TRUE;
}

LRESULT CALLBACK __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	return okno.WndProc(hWnd, message, wParam, lParam);
}

 int StworzOkno() {

	 POINT polozenieOkna = { 100,100 };
	 POINT rozmiarOkna = { 800,600 };
	 if (!okno.Inicjuj(hInstance, polozenieOkna, rozmiarOkna)) {

	 MessageBox(NULL, "Inicjacja okna nie powiodla sie", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
	 return EXIT_FAILURE;
	 }
	 else return okno.Uruchom();
}

std::thread *t;


extern "C" __declspec(dllexport) const char* Akcja() {

	t = new std::thread(StworzOkno);
	t->detach();
	//StworzOkno();
	return "Hello!";
}

extern "C" __declspec(dllexport) void Akcja2() {

	okno.PobierzDane();
}

extern "C" __declspec(dllexport) void Akcja3(pakiet_danych p) {

	okno.PrzekazWskaznik(p);
}