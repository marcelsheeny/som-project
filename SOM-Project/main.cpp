#include <opencv2\opencv.hpp>
#include <time.h>

#define PI 3.14159265

void preencheImagemRandomicamente (IplImage* img);
IplImage* mudaTamanho (IplImage *imgEntrada, double resize);
CvPoint achaVencedor (IplImage* img, CvPoint3D32f pt);
double distancia (CvPoint3D32f pt1, CvPoint3D32f pt2);
void kohonen (IplImage* som, IplImage* imgEntrada);
double max (double d1, double d2);

int main (void) {

	IplImage* imgSOM = cvCreateImage(cvSize(20,20),8,3);
	IplImage* imgEntrada = cvLoadImage("imagem.bmp");

	if (imgEntrada != NULL) {
		preencheImagemRandomicamente(imgSOM);

		kohonen(imgSOM,imgEntrada);

		IplImage* imgSOMResize = mudaTamanho (imgSOM,4.0);
		cvShowImage("SOM",imgSOMResize);
		cvWaitKey(0);
	}

	return 0;

}

void preencheImagemRandomicamente (IplImage* img) {
	unsigned char* colorData = (unsigned char*)img->imageData;
	srand ( time(NULL) );
	for (int i = 0; i < img->width*img->height * img->nChannels; i = i + img->nChannels) {
			colorData[i] = rand() % 256;
			colorData[i+1] = rand() % 256;
			colorData[i+2] = rand() % 256;
	}
}
IplImage* mudaTamanho (IplImage *imgEntrada, double size) {

	//Utiliza os valores de size para criar uma imagem onde será armazeada a imagem que será mostrada na Tela
	IplImage *imgResize = cvCreateImage(cvSize((int) (size*imgEntrada->width),(int) (size*imgEntrada->height )),  IPL_DEPTH_8U, 3);

	//Função que muda o valor para o tamanho da imagem do parâmetro 2
	cvResize (imgEntrada, imgResize);

	return imgResize;
	
}


void kohonen (IplImage* som, IplImage* imgEntrada) {

	unsigned char* colorDataEntrada = (unsigned char*)imgEntrada->imageData;
	unsigned char* colorDataSom = (unsigned char*)som->imageData;
	int tam = 10;
	double sigma = tam/5.0;
	double gaussian = 0;

	CvPoint3D32f color;
	CvPoint ptVencedor;


for(int a=0; a<10; a++)
	for (int i = 0; i < imgEntrada->width*imgEntrada->height * imgEntrada->nChannels; i = i + imgEntrada->nChannels) {

		color.x = colorDataEntrada[i];
		color.y = colorDataEntrada[i+1];
		color.z = colorDataEntrada[i+2];

		ptVencedor = achaVencedor(som, color);
		//cvCircle(som,ptVencedor,4,cvScalar(255,0,0),3);

		for (int y = -tam+ptVencedor.y; y <= tam+ptVencedor.y; y++) {
			for (int x = -tam+ptVencedor.x; x <= tam+ptVencedor.x; x++) {

				if (x < som->width && x >= 0 && y < som->height && y >= 0) {
					double dx = x-ptVencedor.x;
					double dy = y-ptVencedor.y;

					gaussian = ((1./(2.*PI*sigma)) * exp(-( ( (dx*dx) + (dy*dy) ) / (2.*sigma))));
					//printf("%d %d %lf\n", x-ptVencedor.x,y-ptVencedor.y,gaussian);
					CvPoint3D64f s = cvPoint3D64f( ((double)colorDataSom[x*3 + y*som->widthStep]),
												   ((double)colorDataSom[x*3+1 + y*som->widthStep]),
												   ((double)colorDataSom[x*3+2 + y*som->widthStep]) );

					colorDataSom[x*3 + y*som->widthStep] = (int)(((gaussian)*((double)color.x)) + ((1.-gaussian)*s.x));
					colorDataSom[x*3+1 + y*som->widthStep] = (int)( ((gaussian)*((double)color.y)) + ((1.-gaussian)*s.y));
					colorDataSom[x*3+2 + y*som->widthStep] = (int)( ((gaussian)*((double)color.z)) + ((1.-gaussian)*s.z));
				}
			}
		}
		IplImage* imgSOMResize = mudaTamanho (som,4.0);
				
		cvShowImage("SOM",imgSOMResize);
		cvWaitKey(1);
		cvReleaseImage (&imgSOMResize);


	}
	
}

double distancia (CvPoint3D32f pt1, CvPoint3D32f pt2) {
	return  (pt1.x - pt2.x) * (pt1.x - pt2.x) +
			(pt1.y - pt2.y) * (pt1.y - pt2.y) +
			(pt1.z - pt2.z) * (pt1.z - pt2.z);
}

CvPoint achaVencedor (IplImage* img, CvPoint3D32f pt) {
	unsigned char* colorData = (unsigned char*)img->imageData;
	CvPoint3D32f ptImg;
	CvPoint ptVencedor;
	double dist;
	double distVencedor = 10000000;
	CvPoint vencedor = cvPoint(0,0);
	for(int y = 0; y < img->height; y++) {
		for(int x = 0; x < img->width; x++) {
			ptImg.x = colorData[x + y*img->widthStep];
			ptImg.y = colorData[x+1 + y*img->widthStep];
			ptImg.z = colorData[x+2 + y*img->widthStep];

			dist = distancia(pt,ptImg);

			if (dist < distVencedor) {
				distVencedor = dist;
				ptVencedor = cvPoint(x,y);
			}
		}
	}

	return ptVencedor;
}


double max (double d1, double d2) {
	if (d1 > d2)
		return d1;
	else
		return d2;
}