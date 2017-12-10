#pragma once
#include<cuda_runtime.h>
#include "device_launch_parameters.h"
#include<stdio.h>
#include<cstdlib>
#include<Windows.h>


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


void zaladujTeksture(pakiet_danych& p,unsigned long* tekstura,int szer,int wys);
void init();