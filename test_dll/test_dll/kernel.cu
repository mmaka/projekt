#include"cuda_kernel.cuh"
#include<cuda_gl_interop.h>
#include<iostream>
#include<fstream>


__global__ void kernel(cudaSurfaceObject_t surf1, cudaSurfaceObject_t surf2, int szer, int wys, bool flaga) {

	register int wsp_x = blockIdx.x * blockDim.x + threadIdx.x;
	register int wsp_y = blockIdx.y * blockDim.y + threadIdx.y;

	if (wsp_x < szer && wsp_y < wys) {

	//	register int i = wsp_y*szer + wsp_x;
	//	uchar4 voxel;// = make_uchar4(255, 255, 255, 255);

	//	voxel.x = (flaga) ? 255 : 1;
	//	voxel.y = (flaga) ? 255 : 1;
	//	voxel.z = (flaga) ? 255 : 1;
	//	voxel.w = (flaga) ? 255 : 1;
	//	surf2Dwrite(voxel, (flaga) ? surf2 : surf1, wsp_x * sizeof(uchar4), wsp_y);


		/*
		register int i = wsp_y*szer + wsp_x;
		uchar4 voxel;// = make_uchar4(255, 255, 255, 255);
		//	surf2Dread(&voxel, surf[blockIdx.x], wsp_x * sizeof(uchar4), wsp_y);
		surf2Dread(&voxel, (flaga)?surf1:surf2, wsp_x * sizeof(uchar4), wsp_y);
		//if (voxel.x == 255) {

		voxel.x = (flaga) ? 255 : 1;
		voxel.y = (flaga) ? 255 : 1;
		voxel.z = (flaga) ? 255 : 1;
		voxel.w = (flaga) ? 255 : 1;
		//surf2Dwrite(voxel, surf[blockIdx.x], wsp_x * sizeof(uchar4), wsp_y);
		surf2Dwrite(voxel, (flaga) ? surf2 : surf1, wsp_x * sizeof(uchar4), wsp_y);
		//}
		*/
	}
}

typedef unsigned int  uint;
typedef unsigned char uchar;

void cuda_texture_interface::zarejestrujTeksture(bool flaga, int indeks, GLuint image, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel, unsigned long* tekstura, int szer, int wys) {

	wysokosc = wys;
	szerokosc = szer;

	if (flaga) {

		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&r[indeks], image, target, flags));
		HANDLE_ERROR(cudaGraphicsMapResources(1, &r[indeks], stream));
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&tab[indeks], r[indeks], arrayIndex, mipLevel));

		lalala(indeks, tekstura, szer, wys, true);


	}
	else {

		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&r2[indeks], image, target, flags));
		HANDLE_ERROR(cudaGraphicsMapResources(1, &r2[indeks], stream));
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&tab2[indeks], r2[indeks], arrayIndex, mipLevel));
		lalala(indeks, tekstura, szer, wys, false);

	}
}


void cuda_texture_interface::odmapowanieTekstur() {

	HANDLE_ERROR(cudaGraphicsUnmapResources(rozmiar, r, stream));
	HANDLE_ERROR(cudaGraphicsUnmapResources(rozmiar, r2, stream));
}

void cuda_texture_interface::inicjalizacja() {

	cudaDeviceProp prop;
	int dev;
	memset(&prop, 0, sizeof(cudaDeviceProp));
	prop.major = 1;
	prop.minor = 0;
	HANDLE_ERROR(cudaChooseDevice(&dev, &prop));
	HANDLE_ERROR(cudaGLSetGLDevice(dev));
	cudaStreamCreateWithFlags(&stream, cudaStreamDefault);
}

void cuda_texture_interface::lalala(int i, unsigned long* tekstura, int szer, int wys, bool flaga) {

	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8, 8, 8, 8, cudaChannelFormatKindUnsigned);
	
	HANDLE_ERROR(cudaMemcpyToArray((flaga) ? tab[i] : tab2[i], 0, 0, tekstura, szer*wys * sizeof(unsigned long), cudaMemcpyHostToDevice));
	
	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));

	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = (flaga) ? tab[i] : tab2[i];
	
	HANDLE_ERROR(cudaCreateSurfaceObject(flaga ? &tex[i] : &tex2[i], &resDesc));
}

void cuda_texture_interface::akcja(bool flaga) {

	dim3 block(32, 32);
	dim3 grid((szerokosc - 1) / 32 + 1, (szerokosc - 1) / 32 + 1);

	for (int i = 0; i<rozmiar; ++i)
		kernel << <grid, block >> > (tex[i], tex2[i], szerokosc, wysokosc, flaga);

}