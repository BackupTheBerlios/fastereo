
/****** Image support module ******/

/***
Sofware: stereomf
Version: 1.0
Author : Sebastien Roy (roys@iro.umontreal.ca)
Date   : 23 march 1999

               Copyright (c) 2000-2004, Universite de Montreal
               Copyright (c) 1999, NEC Research Institute Inc.
***/



/***** Define this for closest point instead of bi-linear interpolation *****/
// #define CLOSEST_POINT

/*** Support either pbm or sgi format ***/

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
/*#include <ieeefp.h>*/

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include "img.h"


/* Define to inverse all Y in image */
#define INVERSE_Y

/** Define to have verbose info **/
/*#define VERBOSE*/


/***** SUPPORT *****/

void DumpImgInfo(imginfo *II)
{
        printf("--- ImgInfo ---\n");
        printf("XSize,YSize = %d,%d\n",II->XSize,II->YSize);
	printf("ZSize = %d\n",II->ZSize);
	if( II->Data==NULL ) printf("NO Image Loaded\n");
	else printf("Image Loaded\n");
        printf("---------------\n");
}

/* init even without loading any image */
int InitImgInfo(imginfo *II,int xs,int ys)
{
        /* for now, only size info */
        II->XSize=xs;
        II->YSize=ys;
	II->ZSize=0;
	II->Data=NULL;
        return(0);
}



/***** MAIN ROUTINES *****/

/** Create a new image **/
int CreateImage(int XS,int YS,int ZS,imginfo *I)
{
int Sz,i;
unsigned char *Data,*P;

	Sz=XS*YS*ZS;
	Data=(unsigned char *)malloc(Sz);

	if( Data==NULL ) {
		I->XSize=I->YSize=I->ZSize=0;
		I->Data=NULL;
		return(-1);
	}

	I->XSize=XS;
	I->YSize=YS;
	I->ZSize=ZS;
	I->Data=Data;

	for(i=0,P=Data;i<Sz;i++) *P++=0;
	return(0);
}


void FreeImage(imginfo *I)
{
	if( I->Data!=NULL ) free(I->Data);
	I->XSize=I->YSize=I->ZSize=0;
	I->Data=NULL;
}




/* Bi-linear Interpolation */
/* Return 1 if (x,y) are inside the image, 0 otherwise */
int ImgCheck(float x,float y,imginfo *I)
/* x,y : 0..XSize-1,0..YSize-1 */
{
int fx,fy,cx,cy;
int r;

	/*if( isnanf(x) || isnanf(y) ) return(0);*/
	
	fx=(int)floor(x);cx=(int)ceil(x);
	fy=(int)floor(y);cy=(int)ceil(y);

	if( fx<0 || fy<0 || cx>=I->XSize || cy>=I->YSize ) r=0;
	else r=1;

	/*printf("Check (%f,%f) [%d,%d] -> %d\n",x,y,I->XSize,I->YSize,r);*/

	return(r);
}

#define OUTSIDE_VALUE	0.0

/* Bi-linear interpolation */
/* Use z=0 in monochrome case */
/* return OUTSIDE_VALUE if outside the image */
float InterpoleImg(float x,float y,int iz,imginfo *I)
/* x,y : 0..XSize-1,0,YSize-1 */
/* Img : Acces:[(Y*XSize+X)*ZS+z] */
{
int ix,iy,P;
float dx,dy;
float a,b,c,d;
	ix=(int)floor(x);iy=(int)floor(y);
#ifdef CLOSEST_POINT
	if( ix<0 || iy<0 || ix>=I->XSize || iy>=I->YSize ) return(OUTSIDE_VALUE);
	P=(iy*I->XSize+ix)*I->ZSize+iz;
	return(I->Data[P]); /* Closest point!! */
#endif
	dx=x-(float)ix;dy=y-(float)iy;
	/** Check for noise... **/
	if( dx<0.0001 ) dx=0.0;
	if( dy<0.0001 ) dy=0.0;
	/*printf("Interpole(%f,%f)->(%d,%d) dxy=(%f%s,%f%s)\n",x,y,ix,iy
		,dx,(dx==0.0)?"z":"",dy,(dy==0.0)?"z":"");*/
	if( ix<0 || iy<0 ) return(OUTSIDE_VALUE);
	if( ix+1>=I->XSize && dx!=0.0 ) return(OUTSIDE_VALUE);
	if( iy+1>=I->YSize && dy!=0.0 ) return(OUTSIDE_VALUE);

	P=(iy*I->XSize+ix)*I->ZSize+iz;

	a=I->Data[P];
	if( dx==0.0 ) b=a;
	else {
		b=I->Data[P+I->ZSize];
	}
	if( dy==0.0 ) c=a;
	else{
		c=I->Data[P+I->XSize*I->ZSize];
	}
	if( dx==0.0 ) d=c;
	else if( dy==0.0 ) d=b;
	else d=I->Data[P+(I->XSize+1)*I->ZSize];

	/*printf("Interpole %f,%f,%f,%f\n",a,b,c,d);*/

	/* bi-linear interpolation */
	a+=(c-a)*dy;
	b+=(d-b)*dy;
	a+=(b-a)*dx;
	return(a);
}


/*************************************************/
/**                                             **/
/**********   Bi-Cubic Interpolation   ***********/
/**                                             **/
/*************************************************/


/* Bi-Cubic Interpolation */
/* Return 1 if (x,y) are inside the image, 0 otherwise */
int ImgCheckCubic(float x,float y,imginfo *I)
/* x,y : 0..XSize-1,0..YSize-1 */
{
int fx,fy,cx,cy;
int r;
	/*if( isnanf(x) || isnanf(y) ) return(0);*/
	
	fx=(int)floor(x);cx=(int)ceil(x);
	fy=(int)floor(y);cy=(int)ceil(y);

	if( fx-1<0 || fy-1<0 || cx+1>=I->XSize || cy+1>=I->YSize ) r=0;
	else r=1;

	/*printf("Check (%f,%f) [%d,%d] -> %d\n",x,y,I->XSize,I->YSize,r);*/

	return(r);
}

/** Support **/

/** return the cubic parameters a,b,c,d (param[0..3]) from 4 points **/
/** ASSUME x[0..3]=[-1,0,1,2] and f(x) = f[0..3] **/
void SimpleCubicParam(float *f,float *param)
{
	param[0]=(-f[0] + 3*f[1] - 3*f[2] + f[3])/6;
	param[1]=(f[0] - 2*f[1] + f[2])/2;
	param[2]=(-2*f[0] - 3*f[1] + 6*f[2] - f[3])/6; 
   	param[3]=f[1];
}

/** Evaluate a x^3+b x^2+c x+d with {a,b,c,d}=q[0..3] **/
float EvalCube(float *q,float x)
{
	return((((q[0]*x)+q[1])*x+q[2])*x+q[3]);
}


/** return the cubic parameters a,b,c,d (param[0..3]) from 4 points **/
/** x[0..3] and f(x) = f[0..3] **/
void CubicParam(float *x,float *f,float *param)
{
float x11,x12,x13,x21,x22,x23,x31,x32,x33,x41,x42,x43;
float m21,m22,m31,m32,m41,m42;
float d2,d3,d4,p2,p3,p4,q3,q4,r3,r4,s3,s4,qs34,qr34;
float a,b,c,d;
        x11=x[0];
        x12=x11*x11;
        x13=x11*x12;
        x21=x[1];
        x22=x21*x21;
        x23=x21*x22;
        x31=x[2];
        x32=x31*x31;
        x33=x31*x32;
        x41=x[3];
        x42=x41*x41;
        x43=x41*x42;

        m21=(x13*x21 - x11*x23);
        m22=(x13*x22 - x12*x23);
        m31=(x13*x31 - x11*x33);
        m32=(x13*x32 - x12*x33);
        m41=(x13*x41 - x11*x43);
        m42=(x13*x42 - x12*x43);

        d2=(x13 - x23);
        d3=(x13 - x33);
        d4=(x13 - x43);

        p2=(x23*f[0] - x13*f[1]);
        p3=(x33*f[0] - x13*f[2]);
        p4=(x43*f[0] - x13*f[3]);

        q3=(m22*m31 - m21*m32);
        q4=(m22*m41 - m21*m42);

        r3=(d3*m22 - d2*m32);
        r4=(d4*m22 - d2*m42);

        s3=(m22*p3 - m32*p2);
        s4=(m22*p4 - m42*p2);

        qs34=(q3*s4 - q4*s3);
        qr34=(q3*r4 - q4*r3);

	a=(m22 * qs34 * q3 - m22 * qs34 * r3 * x11 + m22 * qr34 * s3 * x11
	 + p2 * qr34 * q3 * x12 - d2 * qs34 * q3 * x12
	 + m21 * qs34 * r3 * x12 - m21 * qr34 * s3 * x12
	 + m22 * qr34 * q3 * f[0]) / (m22 * qr34 * q3 * x13);

        b=(d2 * qs34 * q3 - m21 * qs34 * r3 + m21 * qr34 * s3
	 - p2 * qr34 * q3) / (m22 * qr34 * q3);

        c=(qs34 * r3 - qr34 * s3) / (qr34 * q3);

        d= - qs34 / qr34;

	param[0]=a;
	param[1]=b;
	param[2]=c;
	param[3]=d;

}


/* Bi-Cubic interpolation */
/* Use z=0 in monochrome case */
/* return OUTSIDE_VALUE if outside the image */
float InterpoleImgCubic(float x,float y,int iz,imginfo *I)
/* x,y : 0..XSize-1,0,YSize-1 */
/* Img : Acces:[(Y*XSize+X)*ZS+z] */
{
int ix,iy,P;
float dx,dy;
float a;
float f[4],q[4],v[4];
	ix=(int)floor(x);iy=(int)floor(y);
#ifdef CLOSEST_POINT
	if( ix<0 || iy<0 || ix>=I->XSize || iy>=I->YSize ) return(OUTSIDE_VALUE);
	P=(iy*I->XSize+ix)*I->ZSize+iz;
	return(I->Data[P]); /* Closest point!! */
#endif
	dx=x-(float)ix;dy=y-(float)iy;
	/** Check for noise... **/
	/**
	if( dx<0.0001 ) dx=0.0;
	if( dy<0.0001 ) dy=0.0;
	**/
	/*printf("IC(%f,%f)->(%d,%d)\n",x,y,ix,iy);*/
	if( ix-1<0 || iy-1<0 ) return(OUTSIDE_VALUE);
	if( ix+2>=I->XSize && dx!=0.0 ) return(OUTSIDE_VALUE);
	if( iy+2>=I->YSize && dy!=0.0 ) return(OUTSIDE_VALUE);

	P=(iy*I->XSize+ix)*I->ZSize+iz;

	/** First horizontal row of four points **/
	P=((iy-1)*I->XSize+(ix-1))*I->ZSize+iz;
	f[0]=I->Data[P];P+=I->ZSize;
	f[1]=I->Data[P];P+=I->ZSize;
	f[2]=I->Data[P];P+=I->ZSize;
	f[3]=I->Data[P];
	SimpleCubicParam(f,q);
	v[0]=EvalCube(q,dx);

	/** Second horizontal row of four points **/
	P=((iy)*I->XSize+(ix-1))*I->ZSize+iz;
	f[0]=I->Data[P];P+=I->ZSize;
	f[1]=I->Data[P];P+=I->ZSize;
	f[2]=I->Data[P];P+=I->ZSize;
	f[3]=I->Data[P];
	SimpleCubicParam(f,q);
	v[1]=EvalCube(q,dx);

	/** Third horizontal row of four points **/
	P=((iy+1)*I->XSize+(ix-1))*I->ZSize+iz;
	f[0]=I->Data[P];P+=I->ZSize;
	f[1]=I->Data[P];P+=I->ZSize;
	f[2]=I->Data[P];P+=I->ZSize;
	f[3]=I->Data[P];
	SimpleCubicParam(f,q);
	v[2]=EvalCube(q,dx);

	/** Fourth horizontal row of four points **/
	P=((iy+2)*I->XSize+(ix-1))*I->ZSize+iz;
	f[0]=I->Data[P];P+=I->ZSize;
	f[1]=I->Data[P];P+=I->ZSize;
	f[2]=I->Data[P];P+=I->ZSize;
	f[3]=I->Data[P];
	SimpleCubicParam(f,q);
	v[3]=EvalCube(q,dx);

	/** Extract final value from row v **/
	SimpleCubicParam(v,q);
	a=EvalCube(q,dy);

	if( a<0.0 ) a=0.0;
	if( a>255.0 ) a=255.0;
	return(a);
}


/********** PBM FORMAT SECTION ***********/

/** return 0 if ok, -1 if error **/
/** return the data in PData, and the size in P[XYZ]Size **/
/** ZSize=1 -> monochrome, ZSize=3 or 4 -> RGB or ARGB **/
/** If RGB : rgbrgbrgbrgb... **/
int LoadImage(char *Name,imginfo *I)
{
FILE *img;
unsigned char *Data;
/* Data : [XSize*YSize*ZSize)], Acces=[(Y*XSize+X)(*Zsz)+(z)] */
int P,y;
int XSize,YSize,ZSize;
char TBuf[100];

	/* default values */
	I->XSize=I->YSize=I->ZSize=0;
	I->Data=NULL;

        img=fopen(Name,"r");
        if( img==NULL ) {
                fprintf(stderr,"LoadImage: Unable to open '%s'\n",Name);return(-1);
        }

	/** Read header **/
	if( fgets(TBuf,100,img)!=TBuf ) {
                fprintf(stderr,"LoadImage: Unable to read header of '%s'\n",Name);
		return(-1);
	}
	if( strncmp(TBuf,"P5",2)==0 ) ZSize=1;	/* pgm */
	else if( strncmp(TBuf,"P6",2)==0 ) ZSize=3;	/* ppm */
	else {
		fprintf(stderr,"LoadImage: Unsupported PBM format for '%s'\n",Name);
		return(-1);
	}
	/* Skip comments */
	do {
		if( fgets(TBuf,100,img)!=TBuf ) return(-1);
	} while( TBuf[0]=='#' );
	/* xres/yres */
	sscanf(TBuf,"%d %d",&XSize,&YSize);
	/* Number of gray , usually 255, Just skip it */
	if( fgets(TBuf,100,img)!=TBuf ) return(-1);

        /* print a little info about the image */
#ifdef VERBOSE
        printf("Image x and y size in pixels: %d %d\n"
                ,XSize,YSize);
        printf("Image zsize in channels: %d\n",ZSize);
        printf("Image pixel min and max: %d %d\n",0,255);
#endif

        /* Allocate data space , as char data... for now */
        Data=(unsigned char *)malloc(XSize*YSize*ZSize);
        if( Data==NULL ) { fprintf(stderr,"LoadImage: Out of mem\n");return(-1); }

        for(y=0;y<YSize;y++) {
#ifdef INVERSE_Y
		P=(YSize-1-y)*XSize*ZSize;
#else
		P=y*XSize*ZSize;
#endif
		fread(Data+P,1,XSize*ZSize,img);
        }

        I->Data=Data;
        I->XSize=XSize;
        I->YSize=YSize;
        I->ZSize=ZSize;

        fclose(img);
        return(0);
}


/*** Save in PBM Format , If color, Data is RGB RGB RGB ... ***/
int SaveImage(char *Name,imginfo *I)
{
FILE *img;
int P,y;
unsigned char *Data;
time_t tm;

	if( I->ZSize==0 || I->Data==NULL ) return(-1);

	if( I->ZSize!=1 && I->ZSize!=3 ) {
		fprintf(stderr,"SaveImage: ZSize=%d unsupported\n",I->ZSize);
		return(-1);
	}

#ifdef VERBOSE
        printf("Saving %d plane image '%s'\n",I->ZSize,Name);
#endif

	Data=I->Data;

        img=fopen(Name,"w");
        if( img==NULL ) {
                fprintf(stderr,"SaveImage: Unable to open '%s'\n",Name);return(-1);
        }

	if( I->ZSize==1 ) fprintf(img,"P5\n");	/* pgm */
	else if( I->ZSize==3 ) fprintf(img,"P6\n");	/* ppm */
	else return(-1); /* impossible */

	tm=time(NULL);
	fprintf(img,"# IMG Module, %s",ctime(&tm));

	fprintf(img,"%d %d\n255\n",I->XSize,I->YSize);


        for(y=0;y<I->YSize;y++) {
#ifdef INVERSE_Y
                P=(I->YSize-1-y)*I->XSize*I->ZSize; /* A L'ENVERS */
#else
                P=y*I->XSize*I->ZSize;
#endif
		fwrite(Data+P,1,I->XSize*I->ZSize,img);
        }
        fclose(img);

        return(0);
}


