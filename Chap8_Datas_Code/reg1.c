// reg1.c : Regions.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

/*----------------------------------------------------------------------
	Grey level include file
					J. R. Parker
					Digital Media Laboratory
					University of Calgary
					Calgary, Alberta, Canada
  ---------------------------------------------------------------------- */

#include <malloc.h>
#include <fcntl.h>
#include <io.h>

/* The image header data structure      */
struct header {
	int nr, nc;             /* Rows and columns in the image */
	int oi, oj;             /* Origin */
};

/*      The IMAGE data structure        */
struct image {
		struct header *info;            /* Pointer to header */
		unsigned char **data;           /* Pixel values */
};

#define SQRT2 1.414213562
#define BLACK 0
#define WHITE 1

long seed = 132531;
typedef struct image * IMAGE;
FILE *f;


IMAGE newimage (int nr, int nc)
{
	struct image  *x;                /* New image */
	int i;

	if (nr < 0 || nc < 0) 
	{
		printf ("Error: Bad image size (%d,%d)\n", nr, nc);
		return 0;
	}

/*      Allocate the image structure    */
	x = (struct image  *) malloc( sizeof (struct image) );
	if (!x) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

/*      Allocate and initialize the header      */

	x->info = (struct header *)malloc( sizeof(struct header) );
	if (!(x->info)) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}
	x->info->nr = nr;       x->info->nc = nc;
	x->info->oi = x->info->oj = 0;

/*      Allocate the pixel array        */

	x->data = (unsigned char **)malloc(sizeof(unsigned char *)*nr); 

/* Pointers to rows */
	if (!(x->data)) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	x->data[0] = (unsigned char *)malloc (nr*nc);
	if (x->data[0]==0)
	  {
		printf ("Out of storage. Newimage.\n");
		exit(1);
	  }

	for (i=1; i<nr; i++) 
	{
	  x->data[i] = (x->data[0]+nc*i);
	}

	return x;
}

void freeimage (struct image  *z)
{
/*      Free the storage associated with the image Z    */

	if (z != 0) 
	{
	   free (z->info);
	   free (z->data);
	   free (z);
	}
}

/* Otsu's method of 'grey level histograms' */
float nu (float *p, int k, float ut, float vt);
float u (float *p, int k);
void thr_glh (IMAGE im);


void thr_glh (IMAGE x)
{
/*	Threshold selection using grey level histograms. SMC-9 No 1 Jan 1979
		N. Otsu							*/

	int i,j,k,n,m, h[260], t;
	float y, z, p[260];
	unsigned char *pp;
	float ut, vt;

	n = x->info->nr*x->info->nc;
	for (i=0; i<260; i++) {		/* Zero the histograms	*/
		h[i] = 0;
		p[i] = 0.0;
	}

		 	/* Accumulate a histogram */
	for (i=0; i<x->info->nr; i++)
	   for (j=0; j<x->info->nc; j++) {
		   k = x->data[i][j];
		h[k+1] += 1;
	   }

	for (i=1; i<=256; i++)		/* Normalize into a distribution */
		p[i] = (float)h[i]/(float)n;

	ut = u(p, 256);		/* Global mean */
	vt = 0.0;		/* Global Variance */
	for (i=1; i<=256; i++)
		vt += (i-ut)*(i-ut)*p[i];

	j = -1; k = -1;
	for (i=1; i<=256; i++) {
		if ((j<0) && (p[i] > 0.0)) j = i;	/* First index */
		if (p[i] > 0.0) k = i;			/* Last index  */
	}
	z = -1.0;
	m = -1;
	for (i=j; i<=k; i++) {
		y = nu (p, i, ut, vt);		/* Compute NU */
		if (y>=z) {			/* Is it the biggest? */
			z = y;			/* Yes. Save value and i */
			m = i;
		}
	}

	t = m;
	printf("Threshold found is %d\n", t);

/* Threshold */
	pp = x->data[0];
	for (i=0; i<n; i++)
	  if (*pp < t)
	    *pp++ = 0;
	  else
	    *pp++ = 255;
}

float w (float *p, int k)
{
	int i;
	float x=0.0;

	for (i=1; i<=k; i++) x += p[i];
	return x;
}

float u (float *p, int k)
{
	int i;
	float x=0.0;

	for (i=1; i<=k; i++) x += (float)i*p[i];
	return x;
}

float nu (float *p, int k, float ut, float vt)
{
	float x, y;

	y = w(p,k);
	x = ut*y - u(p,k);
	x = x*x;
	y = y*(1.0F-y);
	if (y>0) x = x/y;
	 else x = 0.0;
	return x/vt;
}

void flood (IMAGE img, int i, int j, int target, int replace)
{
	if (img->data[i][j] == target)
	{	
		img->data[i][j] = 1;
		img->data[i][j] = replace;

		if (i+1 < img->info->nr) flood (img, i+1, j, target, replace);
		if (j-1 >= 0)            flood (img, i, j-1, target, replace);
		if (j+1 < img->info->nc) flood (img, i, j+1, target, replace);
		if (i-1 >= 0)            flood (img, i-1, j, target, replace);
	}
}

IplImage *toOpenCV (IMAGE x)
{
	IplImage *img;
	int i=0, j=0;
	CvScalar s;
	
	img = cvCreateImage (cvSize(x->info->nc, x->info->nr), 8 /*IPL_DEPTH_8U*/, 1);
	for (i=0; i<x->info->nr; i++)
	{
		for (j=0; j<x->info->nc; j++)
		{
			s.val[0] = x->data[i][j];
			cvSet2D (img, i,j,s);
		}
	}
	return img;
}

IMAGE fromOpenCV (IplImage *x)
{
	IMAGE img;
	int color=0, i=0;
	int k=0, j=0;
	CvScalar s;
	
	if ((x->depth==IPL_DEPTH_8U) &&(x->nChannels==1))	// 1 Pixel (grey) image
		img = newimage (x->height, x->width);
	else if ((x->depth==8) && (x->nChannels==3)) //Color
	{
		color = 1;
		img = newimage (x->height, x->width);
	}
	else return 0;

	for (i=0; i<x->height; i++)
	{
		for (j=0; j<x->width; j++)
		{
			s = cvGet2D (x, i, j);
			if (color) k = (unsigned char)((s.val[0] + s.val[1] + s.val[2])/3);
			else k = (unsigned char)(s.val[0]);
			img->data[i][j] = k;
		}
	}
	return img;
}

int area (IMAGE x, int c)
{
	int i,j,k=0;

	for (i=0; i<x->info->nr; i++)
		for (j=0; j<x->info->nc; j++)
			if (x->data[i][j] == c) k++;
	return k;
}

void clear (IMAGE x, int c)
{
	int i,j,k=0;

	for (i=0; i<x->info->nr; i++)
		for (j=0; j<x->info->nc; j++)
			if (x->data[i][j] == c) x->data[i][j] = 255;
}

void remark (IMAGE x, int a, int b)
{
	int i,j,k=0;

	for (i=0; i<x->info->nr; i++)
		for (j=0; j<x->info->nc; j++)
			if (x->data[i][j] == a) x->data[i][j] = b;
}

int main(int argc, char *argv[])
{
  IplImage* img=0; 
  IplImage* img2=0;
  IMAGE x;
  int height,width,step,channels;
  uchar *data;
  int found=1,count=0;
  int i,j,k,n=40;
  int smallMark= 90, largeMark= 230;

  // load an image  
    img=cvLoadImage("H:/Documents and Settings/jp/Desktop/EditionTwo/Chapter 8 Classifiers/Figure8.3data.jpg");

  if(!img)
  {
    printf("Could not load image file: %s\n",argv[1]);
    exit(0);
  }

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uchar *)img->imageData;
  printf("Processing a %dx%d image with %d channels\n",height,width,channels); 
  f = fopen ("H:/AIPCV/log.txt", "w");


  // MAKE CERTAIN THAT OPENCV and AIPCV images are identical!!!!
  // create a window
  cvNamedWindow("win1", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("win1", 100, 100);

  // show the image
  cvShowImage("win1", img );
  cvWaitKey(0);
 
  // Convert to AIPCV IMAGE type
   x = fromOpenCV (img);
	printf ("AIPCV image is %d rows by %d columns.\n", x->info->nr, x->info->nc);
    img2 = toOpenCV (x);
	cvSaveImage( "H:/AIPCV/thresholded.png", img2 );

  if (x)
  {
	  thr_glh (x);
	  img2 = toOpenCV (x);
	  cvNamedWindow( "thresh", CV_WINDOW_AUTOSIZE);
	  cvShowImage( "thresh", img2 );
	  cvSaveImage( "H:/AIPCV/thresholded.png", img2 );
  } else return 0;

  k=1;
  while (found)
  {
	  found = 0;
	  for (i=0; i<height; i++)					// Look for seed pixel, black.
		for (j=0; j<width; j++)
		{
		  if (x->data[i][j] == 0)				// Found one!
		  {
			  flood (x, i, j, 0, 128);			// Mark a region
			  k = area(x, 128);					// How big is it?
			  if (k < 900 || k > 3100) clear (x, 128);		// Too small/big
			  else
			  {
				  if (k > 2500) { remark (x, 128, largeMark); n = largeMark++; }
				  else { remark (x, 128, smallMark); n = smallMark++; }
				  printf ("Area is %d, marked as %d\n", k, n);
				  found = 1;
			  }
			  img2 = toOpenCV (x);
			  cvShowImage ("thresh", img2);
			  cvWaitKey (100);
		  }
	    }
  }

  // wait for a key
  cvWaitKey(0);

  // release the image
  cvReleaseImage(&img );
  return 0;
}
