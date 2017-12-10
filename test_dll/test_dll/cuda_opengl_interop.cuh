#pragma once
#include<cuda_runtime.h>
#include "device_launch_parameters.h"
#include<stdio.h>
#include<stdlib.h>
#include "glew.h"
#include "wglew.h"
#include "Pliki.h"
#include<queue>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<thread>


struct pakiet_danych {
	int rozmiar_x = 0;
	int rozmiar_y = 0;
	uchar4* ptr;
};


static void HandlerError(cudaError_t err, const char *file, int line) {
	if (err != cudaSuccess) {
		printf("%s in %s at line %d\n", cudaGetErrorString(err),
			file, line);
		exit(EXIT_FAILURE);
	}
}

#define HANDLE_ERROR(err) (HandlerError(err,__FILE__,__LINE__))


class Cuda_OpenGL_Interop {

	int rozmiar;
	int szer_tekstury;
	int wys_tekstury;

	cudaGraphicsResource_t *r1;
	cudaArray_t *tab1;
	cudaSurfaceObject_t *surf1;

	cudaGraphicsResource_t *r2;
	cudaArray_t *tab2;
	cudaSurfaceObject_t *surf2;

	cudaStream_t strumien;// byæ mo¿e strumienie te¿ bêd¹ w tablicy: https://devblogs.nvidia.com/parallelforall/gpu-pro-tip-cuda-7-streams-simplify-concurrency/

public:
	explicit Cuda_OpenGL_Interop(int liczba_tekstur, int szerokosc, int wysokosc) : rozmiar(liczba_tekstur), szer_tekstury(szerokosc), wys_tekstury(wysokosc) {

		r1 = new cudaGraphicsResource_t[liczba_tekstur];
		tab1 = new cudaArray_t[liczba_tekstur];
		surf1 = new cudaSurfaceObject_t[liczba_tekstur];
		r2 = new cudaGraphicsResource_t[liczba_tekstur];
		tab2 = new cudaArray_t[liczba_tekstur];
		surf2 = new cudaSurfaceObject_t[liczba_tekstur];
	}
	void zarejestrujTekstury(bool ping_pong, int indeks, GLuint image, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel, unsigned long* tekstura);
	void odmapowanieTekstur2();
	void init();
//	void pobieranie_danych(int i, unsigned long* tekstura, bool flaga);
	void launch(bool flaga);
	void rejestrowanieTekstur(GLuint *indeksy_OGL, cudaGraphicsResource_t *res, cudaArray_t *arr,int ileTekstur,cudaStream_t strumien, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel);
	void odmapowanieTekstur(cudaGraphicsResource_t *res,int ileTekstur,cudaStream_t stream);
};

class PojemnikNaTekstury {

	int liczbaTekstur;
	int szer_tekstury;
	int wys_tekstury;

	cudaGraphicsResource_t *r1;
	cudaArray_t *tab1;
	cudaSurfaceObject_t *surf1;

	cudaGraphicsResource_t *r2;
	cudaArray_t *tab2;
	cudaSurfaceObject_t *surf2;

	cudaStream_t strumien;
	std::queue<cudaSurfaceObject_t> kolejka1;
	std::queue<cudaSurfaceObject_t> kolejka2;
	std::queue<pakiet_danych> tasma_produkcyjna;
	unsigned int maxPojemnoscTasmy = 100;
	pakiet_danych p;

	unsigned int pierwszy;
	std::condition_variable cond;
	std::mutex mut;
	
public:
	explicit PojemnikNaTekstury(int ileTekstur,bool dodatkowePrzekroje = false) : liczbaTekstur(ileTekstur) {

		r1 = new cudaGraphicsResource_t[ileTekstur];
		tab1 = new cudaArray_t[ileTekstur];
		surf1 = new cudaSurfaceObject_t[ileTekstur];
		r2 = new cudaGraphicsResource_t[ileTekstur];
		tab2 = new cudaArray_t[ileTekstur];
		surf2 = new cudaSurfaceObject_t[ileTekstur];
		cudaStreamCreateWithFlags(&strumien, cudaStreamDefault);
	
		int szer = 0, wys = 0;
		unsigned long* tekstura = WczytajObrazZPlikuBitmap(NULL, "kwadrat_czer.bmp", szer, wys, false, 255);
		uchar4* ptr;
		HANDLE_ERROR(cudaMalloc(&ptr, szer*wys * sizeof(unsigned long)));
		HANDLE_ERROR(cudaMemcpy(ptr, tekstura, szer*wys * sizeof(unsigned long), cudaMemcpyHostToDevice));
		p.ptr = ptr;
		p.rozmiar_x = szer;
		p.rozmiar_y = wys;
		pierwszy = 0;
	
	}
	void pobieranie_danych(int i, unsigned long* tekstura, bool flaga);
	void wpiszTekstureWCudaArray();
	cudaGraphicsResource_t* res1() { return r1; }
	cudaGraphicsResource_t* res2() { return r2; }
	cudaArray_t *arr1() { return tab1; }
	cudaArray_t *arr2() { return tab2; }
	cudaStream_t str() { return strumien; }
	void surf_init();
	void dostawcaTekstur(const pakiet_danych& p);
	void old_dostawcaTekstur(const pakiet_danych& p,bool flaga);
	void producentTekstur();
	void launch(const cudaSurfaceObject_t& surf,const pakiet_danych& p);
	void przekazWskaznik(pakiet_danych& p);
	pakiet_danych& pakiet() { return p; }
	unsigned int indeks_pierwszego() const { return pierwszy; }
	void ustaw_indeks_pierwszego(unsigned int indeks) { pierwszy = indeks; }
	void generujIndeksy(std::vector<unsigned int>& v);
	std::mutex& mutex() { return mut; }
	std::condition_variable& cond_v(){ return cond; }
};