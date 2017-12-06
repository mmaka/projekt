#pragma once
#include<cuda_runtime.h>
#include "device_launch_parameters.h"
#include "glew.h"
#include "wglew.h"
#include <stdlib.h>
#include <stdio.h>

static void HandlerError(cudaError_t err, const char *file, int line) {
	if (err != cudaSuccess) {
		printf("%s in %s at line %d\n", cudaGetErrorString(err),
			file, line);
		exit(EXIT_FAILURE);
	}
}

#define HANDLE_ERROR(err) (HandlerError(err,__FILE__,__LINE__))

class cuda_texture_interface {

	cudaGraphicsResource_t *r;
	cudaArray_t *tab;
	cudaStream_t stream;
	int rozmiar;
	cudaSurfaceObject_t *tex;
	int szerokosc;
	int wysokosc;

	cudaGraphicsResource_t *r2;
	cudaArray_t *tab2;
	cudaSurfaceObject_t *tex2;

public:
	explicit cuda_texture_interface(int liczba_tekstur) : rozmiar(liczba_tekstur) {
	r = new cudaGraphicsResource_t[liczba_tekstur]; 
	tab = new cudaArray_t[liczba_tekstur];
	tex = new cudaSurfaceObject_t[liczba_tekstur];
	r2 = new cudaGraphicsResource_t[liczba_tekstur]; 
	tab2 = new cudaArray_t[liczba_tekstur];
	tex2 = new cudaSurfaceObject_t[liczba_tekstur];
	}
	void zarejestrujTeksture(bool flaga,int indeks, GLuint image, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel, unsigned long* tekstura, int szer, int wys); //dla kazdej tekstury
	void odmapowanieTekstur();
	void inicjalizacja();
	void lalala(int i,unsigned long* tekstura,int szer,int wys,bool flaga);
	void akcja(bool flaga);
};
