#include"cuda_header.cuh"
//#include"Pliki.h"


void init() {

	cudaDeviceProp prop;
	int dev;
	memset(&prop, 0, sizeof(cudaDeviceProp));
	prop.major = 1;
	prop.minor = 0;
	HANDLE_ERROR(cudaChooseDevice(&dev, &prop));
//	HANDLE_ERROR(cudaGLSetGLDevice(dev));
//	cudaStreamCreateWithFlags(&strumien, cudaStreamDefault);
}

void zaladujTeksture(pakiet_danych& p,unsigned long* tekstura,int szer,int wys) {

	
	//delete[] tekstura;
//	p.ptr = ptr;
//	p.rozmiar_x = szer;
//	p.rozmiar_y = wys;

}
