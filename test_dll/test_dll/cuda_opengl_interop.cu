#include"cuda_opengl_interop.cuh"
#include<cuda_gl_interop.h>


__global__ void kernel(cudaSurfaceObject_t surf1, cudaSurfaceObject_t surf2, int szer, int wys, bool flaga) {

	register int wsp_x = blockIdx.x * blockDim.x + threadIdx.x;
	register int wsp_y = blockIdx.y * blockDim.y + threadIdx.y;

	if (wsp_x < szer && wsp_y < wys) {

		//	register int i = wsp_y*szer + wsp_x;
		//	uchar4 voxel;
		//surf2Dread(&voxel, (flaga) ? surf1 : surf2, wsp_x * sizeof(uchar4), wsp_y);
		//	surf2Dwrite(voxel, (flaga) ? surf2 : surf1, wsp_x * sizeof(uchar4), wsp_y);
	}
}



void Cuda_OpenGL_Interop::zarejestrujTekstury(bool flaga, int indeks, GLuint image, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel, unsigned long* tekstura) {

	if (flaga) {

		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&r1[indeks], image, target, flags));
		HANDLE_ERROR(cudaGraphicsMapResources(1, &r1[indeks], strumien));
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&tab1[indeks], r1[indeks], arrayIndex, mipLevel));
	//	pobieranie_danych(indeks, tekstura, true);//pobieranie danych bêdzie zorganizowane zupe³nie inaczej - bêdzie niezale¿ne od rejestracji - póki co na potrzeby przyk³adu dzia³ania

	}
	else {

		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&r2[indeks], image, target, flags));
		HANDLE_ERROR(cudaGraphicsMapResources(1, &r2[indeks], strumien));
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&tab2[indeks], r2[indeks], arrayIndex, mipLevel));
	//	pobieranie_danych(indeks, tekstura, false);

	}
}

void Cuda_OpenGL_Interop::rejestrowanieTekstur(GLuint *indeksy, cudaGraphicsResource_t *res, cudaArray_t *arr, int ileTekstur,cudaStream_t strumien, GLenum target, unsigned int  flags, unsigned int arrayIndex, unsigned int mipLevel) {
/*
	for (int i = 0, end = ileTekstur; i != end; ++i)
		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&res[i], indeksy[i], target, flags));

	HANDLE_ERROR(cudaGraphicsMapResources(ileTekstur, res, strumien));

	for(int i = 0, end = ileTekstur;i!=end;++i)
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&arr[i], res[i], arrayIndex, mipLevel));
		*/
	for (int i = 0, end = ileTekstur; i != end; ++i) {

		HANDLE_ERROR(cudaGraphicsGLRegisterImage(&res[i], indeksy[i], target, flags));
		HANDLE_ERROR(cudaGraphicsMapResources(1, &res[i], strumien));
		HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&arr[i], res[i], arrayIndex, mipLevel));
	}

}

void Cuda_OpenGL_Interop::odmapowanieTekstur(cudaGraphicsResource_t *res, int ileTekstur, cudaStream_t stream) {

	HANDLE_ERROR(cudaGraphicsUnmapResources(ileTekstur, res, stream));

}

void Cuda_OpenGL_Interop::odmapowanieTekstur2() {

	HANDLE_ERROR(cudaGraphicsUnmapResources(rozmiar, r1, strumien));
	HANDLE_ERROR(cudaGraphicsUnmapResources(rozmiar, r2, strumien));
}


void Cuda_OpenGL_Interop::init() {

	cudaDeviceProp prop;
	int dev;
	memset(&prop, 0, sizeof(cudaDeviceProp));
	prop.major = 1;
	prop.minor = 0;
	HANDLE_ERROR(cudaChooseDevice(&dev, &prop));
	HANDLE_ERROR(cudaGLSetGLDevice(dev));
	cudaStreamCreateWithFlags(&strumien, cudaStreamDefault);
}
/*
void Cuda_OpenGL_Interop::pobieranie_danych(int i, unsigned long* tekstura, bool flaga) {

	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8, 8, 8, 8, cudaChannelFormatKindUnsigned);// to powinno byæ ustalone podczas tworzenia instancji klasy, bo te ustawienia siê nie zmieniaj¹

	HANDLE_ERROR(cudaMemcpyToArray((flaga) ? tab1[i] : tab2[i], 0, 0, tekstura, szer_tekstury*wys_tekstury * sizeof(unsigned long), cudaMemcpyHostToDevice));

	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));

	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = (flaga) ? tab1[i] : tab2[i];

	HANDLE_ERROR(cudaCreateSurfaceObject(flaga ? &surf1[i] : &surf2[i], &resDesc));
}
*/

void Cuda_OpenGL_Interop::launch(bool flaga) {

	dim3 block(32, 32);
	dim3 grid((szer_tekstury - 1) / 32 + 1, (szer_tekstury - 1) / 32 + 1);

	for (int i = 0; i<rozmiar; ++i)
		kernel << <grid, block >> > (surf1[i], surf2[i], szer_tekstury, wys_tekstury, flaga);//to bêdzie realizowane przy u¿yciu strumieni - surface'y zapisane s¹ w tablicy, ale nie mo¿na dzia³aæ na nich równolegle - przynajmniej póki co mi siê nie uda³o

}


///////////////////////////////////////////////


void PojemnikNaTekstury::pobieranie_danych(int i, unsigned long* tekstura, bool flaga) {

	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8, 8, 8, 8, cudaChannelFormatKindUnsigned);// to powinno byæ ustalone podczas tworzenia instancji klasy, bo te ustawienia siê nie zmieniaj¹

	HANDLE_ERROR(cudaMemcpyToArray((flaga) ? tab1[i] : tab2[i], 0, 0, tekstura, szer_tekstury*wys_tekstury * sizeof(unsigned long), cudaMemcpyHostToDevice));

	cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));

	resDesc.resType = cudaResourceTypeArray;
	resDesc.res.array.array = (flaga) ? tab1[i] : tab2[i];

	HANDLE_ERROR(cudaCreateSurfaceObject(flaga ? &surf1[i] : &surf2[i], &resDesc));
}

void PojemnikNaTekstury::wpiszTekstureWCudaArray() {

	//int szer, wys;
	unsigned long *tekstura = WczytajObrazZPlikuBitmap(NULL, "kwadrat_czer.bmp", szer_tekstury, wys_tekstury, false, 255);
	for (int i = 0; i < liczbaTekstur; ++i) {

		pobieranie_danych(i, tekstura, true);
		pobieranie_danych(i, tekstura, false);
	}
		


}

void PojemnikNaTekstury::surf_init() {

	for (int i = 0, end = liczbaTekstur; i != end; ++i) {

		cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8, 8, 8, 8, cudaChannelFormatKindUnsigned);// to powinno byæ ustalone podczas tworzenia instancji klasy, bo te ustawienia siê nie zmieniaj¹
		cudaResourceDesc resDesc;
		memset(&resDesc, 0, sizeof(resDesc));
		resDesc.resType = cudaResourceTypeArray;
		resDesc.res.array.array = tab1[i];
		cudaSurfaceObject_t surf;
		HANDLE_ERROR(cudaCreateSurfaceObject(&surf, &resDesc));
		kolejka1.push(surf);

	}

	for (int i = 0, end = liczbaTekstur; i != end; ++i) {

		cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(8, 8, 8, 8, cudaChannelFormatKindUnsigned);// to powinno byæ ustalone podczas tworzenia instancji klasy, bo te ustawienia siê nie zmieniaj¹
		cudaResourceDesc resDesc;
		memset(&resDesc, 0, sizeof(resDesc));
		resDesc.resType = cudaResourceTypeArray;
		resDesc.res.array.array = tab2[i];
		cudaSurfaceObject_t surf;
		HANDLE_ERROR(cudaCreateSurfaceObject(&surf, &resDesc));
		kolejka2.push(surf);

	}
}

__global__ void wpisz_teksture(cudaSurfaceObject_t surf,uchar4 *tex, int szer, int wys) {

	register int wsp_x = blockIdx.x * blockDim.x + threadIdx.x;
	register int wsp_y = blockIdx.y * blockDim.y + threadIdx.y;

	if (wsp_x < szer && wsp_y < wys) {

		surf2Dwrite(tex[wsp_y*szer + wsp_x],surf, wsp_x * sizeof(uchar4), wsp_y);
		//	register int i = wsp_y*szer + wsp_x;
		//	uchar4 voxel;
		//surf2Dread(&voxel, (flaga) ? surf1 : surf2, wsp_x * sizeof(uchar4), wsp_y);
		//	surf2Dwrite(voxel, (flaga) ? surf2 : surf1, wsp_x * sizeof(uchar4), wsp_y);
	}
}

void PojemnikNaTekstury::launch(const cudaSurfaceObject_t& surf,const pakiet_danych& p) {

	dim3 block(32, 32);
	dim3 grid((p.rozmiar_x - 1) / 32 + 1, (p.rozmiar_y - 1) / 32 + 1);

	wpisz_teksture << <grid, block >> > (surf,p.ptr, p.rozmiar_x, p.rozmiar_y);//to bêdzie realizowane przy u¿yciu strumieni - surface'y zapisane s¹ w tablicy, ale nie mo¿na dzia³aæ na nich równolegle - przynajmniej póki co mi siê nie uda³o

}


void PojemnikNaTekstury::old_dostawcaTekstur(const pakiet_danych& p,bool flaga) {

	cudaSurfaceObject_t tmp = (flaga)?kolejka2.front():kolejka1.front();
	(flaga)?kolejka2.pop():kolejka1.pop();
	launch(tmp, p);
	(flaga)?kolejka2.push(tmp):kolejka1.push(tmp);
	pierwszy = (--pierwszy) % 300;
	//HANDLE_ERROR(cudaFree(p.ptr));

}


void PojemnikNaTekstury::dostawcaTekstur(const pakiet_danych& p) {

	mutex().lock();
	tasma_produkcyjna.push(p);
	mutex().unlock();
	//cudaSurfaceObject_t tmp = (flaga)?kolejka2.front():kolejka1.front();
	//(flaga)?kolejka2.pop():kolejka1.pop();
	//launch(tmp, p);
	//(flaga)?kolejka2.push(tmp):kolejka1.push(tmp);
	//HANDLE_ERROR(cudaFree(p.ptr));
	
}

void PojemnikNaTekstury::producentTekstur() {


	std::unique_lock<std::mutex> lock(mutex());
	cond_v().wait(lock);
	bool flaga = true;
	cudaSurfaceObject_t tmp = (flaga) ? kolejka2.front() : kolejka1.front();
	pakiet_danych pak = tasma_produkcyjna.front();
	tasma_produkcyjna.pop();
	launch(tmp, pak);
	(flaga) ? kolejka2.push(tmp) : kolejka1.push(tmp);
}

void PojemnikNaTekstury::przekazWskaznik(pakiet_danych& p) {

	int szer = 0, wys = 0;
	unsigned long* tekstura = WczytajObrazZPlikuBitmap(NULL, "kwadrat_czer.bmp", szer, wys, false, 255);
	uchar4* ptr;
	HANDLE_ERROR(cudaMalloc(&ptr, szer*wys * sizeof(unsigned long)));
	HANDLE_ERROR(cudaMemcpy(ptr, tekstura, szer*wys * sizeof(unsigned long), cudaMemcpyHostToDevice));
//	p.ptr = ptr;
	p.rozmiar_x = szer;
	p.rozmiar_y = wys;

}

void PojemnikNaTekstury::generujIndeksy(std::vector<unsigned int>& v) {

	v.reserve(300);
	
	for (unsigned int i = pierwszy; i < 300; ++i) v.push_back(i);

	for (unsigned int i = 0, end = 300 - v.size(); i != end; ++i) v.push_back(i);


}