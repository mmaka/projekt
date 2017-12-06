#include<Windows.h>
#include<stdio.h>
#include<iostream>


int main() {
	
	HMODULE const hello_dll = LoadLibraryEx("test_dll.dll", nullptr, 0);
	using Akcja_type = const char*(__cdecl*)();
	Akcja_type f_akcja = reinterpret_cast<Akcja_type>(GetProcAddress(hello_dll, "Akcja"));
	std::cout << "Przed uruchomieniem funkcji" << std::endl;
	std::cout << f_akcja() << std::endl;
	std::cout << "Po uruchomieniem funkcji" << std::endl;
//	Sleep(3000);
	FreeLibrary(hello_dll);

}