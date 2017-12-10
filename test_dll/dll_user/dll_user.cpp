#include<Windows.h>
#include<stdio.h>
#include<iostream>
#include"cuda_header.cuh"
#include"Pliki.h"

int main() {
	
	HMODULE const hello_dll = LoadLibraryEx("test_dll.dll", nullptr, 0);
	using Akcja_type = const char*(__cdecl*)();
	Akcja_type f_akcja = reinterpret_cast<Akcja_type>(GetProcAddress(hello_dll, "Akcja"));
	std::cout << "Przed uruchomieniem funkcji" << std::endl;
	std::cout << f_akcja() << std::endl;
//	Sleep(10000);
//	using Akcja_type3 = void(__cdecl*)(pakiet_danych);
//	Akcja_type3 f_akcja3 = reinterpret_cast<Akcja_type3>(GetProcAddress(hello_dll, "Akcja3"));
//	pakiet_danych p;
//	f_akcja3(p);
	std::cout << "Przed snem" << std::endl;
	Sleep(10000);
	std::cout << "Po œnie" << std::endl;
	using Akcja_type2 = void(__cdecl*)();
	Akcja_type2 f_akcja2 = reinterpret_cast<Akcja_type2>(GetProcAddress(hello_dll, "Akcja2"));
	std::cout << "Przed snem" << std::endl;
	Sleep(10000);
	std::cout << "Po œnie" << std::endl;
//	init();
//	int szer = 0, wys = 0;
//	unsigned long* tekstura = WczytajObrazZPlikuBitmap(NULL, "kwadrat_czer.bmp", szer, wys, false, 255);
//	uchar4* ptr;
//	HANDLE_ERROR(cudaMalloc(&ptr, szer*wys * sizeof(unsigned long)));
//	HANDLE_ERROR(cudaMemcpy(ptr, tekstura, szer*wys * sizeof(unsigned long), cudaMemcpyHostToDevice));
//	pakiet_danych p;
//	p.ptr = ptr;
//	p.rozmiar_x = szer;
//	p.rozmiar_y = wys;
//	zaladujTeksture(p, tekstura, szer, wys);

	while (1) {
	
		f_akcja2();
		Sleep(100);

	}


	FreeLibrary(hello_dll);

}