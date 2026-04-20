#include <stdio.h>                  /*  Marr-Hildreth.c  (or marrh.c) */
#include <stdlib.h>
#include <math.h>
#define  PICSIZE 256
#define  MAXMASK 100

         int    pic[PICSIZE][PICSIZE];
         double outpic1x[PICSIZE][PICSIZE];
         double outpic1y[PICSIZE][PICSIZE];
         double outpic2x[PICSIZE][PICSIZE];
         double outpic2y[PICSIZE][PICSIZE];
         int    edgeflag[PICSIZE][PICSIZE];
         double maskx[MAXMASK][MAXMASK];
         double masky[MAXMASK][MAXMASK];
         double convx[PICSIZE][PICSIZE];
         double convy[PICSIZE][PICSIZE];
         double final[PICSIZE][PICSIZE];
         double ival[PICSIZE][PICSIZE];

main(argc,argv)
int argc;
char **argv;
{
    int     i,j,p,q,s,t,mr,centx,centy;
    double  maskval,sumx, sumy, sig,maxival,minival,maxval,ZEROTOL, slope, xmaskval, ymaskval;
    FILE    *fo1, *fo2,*input, *fo3, *magFile, *fopen();
    char    *foobar;

    // File input
    argc--; argv++;
    foobar = *argv;
    input=fopen(foobar,"rb");

    // File output 1
    argc--; argv++;
    foobar = *argv;
    fo1=fopen(foobar,"wb");
    fprintf(magFile, "P5\n256 256\n255\n");

    // // File output 2
    // argc--; argv++;
    // foobar = *argv;
    // fo2=fopen(foobar,"wb");
    // fprintf(fo2, "P5\n256 256\n255\n");

    // // File output3
    // argc--; argv++;
    // foobar = *argv;
    // fo3=fopen(foobar,"wb");
    // fprintf(fo3, "P5\n256 256\n255\n");

    // Sigma input
    argc--; argv++;
    foobar = *argv;
    sig = atoi(foobar);

    // Zero tolerance input
    argc--; argv++;
    foobar = *argv;
    ZEROTOL = atoi(foobar);

    mr = (int)(sig * 3);
    centx = (MAXMASK / 2);
    centy = (MAXMASK / 2);


    // Skips the header
    for (i=0;i<256;i++){ 
        for (j=0;j<256;j++) {
            pic[i][j]  =  getc (input);
        }
    }

    // 1st derivatives for masks
    for (p=-mr;p<=mr;p++) {
        for (q=-mr;q<=mr;q++) {
            xmaskval = -(p*(exp(-(1/2)*(p/sig)*(p/sig))));
            ymaskval = -(q*(exp(-(1/2)*(q/sig)*(q/sig))));

            maskx[p+centy][q+centx] = xmaskval;
            masky[p+centy][q+centx] = ymaskval;
        }
        //   maskval = ((2-(((p*p)+(q*q))/(sig*sig)))*
        //           (exp(-1*(((p*p)+(q*q))/(2*(sig*sig))))));
        //   (mask[p+centy][q+centx]) = maskval;
    }

    // Convolution
    for (i=mr;i<=255-mr;i++) {
        for (j=mr;j<=255-mr;j++) {
            sumx = 0;
            sumy = 0;

            for (p=-mr;p<=mr;p++) {
                for (q=-mr;q<=mr;q++) {
                    sumx += pic[i+p][j+q] * maskx[p+centy][q+centx];
                    sumy += pic[i+p][j+q] * masky[p+centy][q+centx];
                }
            }
            outpic1x[i][j] = sumx;
            outpic1y[i][j] = sumy;
            convx[i][j] = sumx;
            convy[i][j] = sumy;
        }
    }

    // sqrt of squares from Sobel
    maxival = 0;
    for (i=mr;i<256-mr;i++) {
        for (j=mr;j<256-mr;j++) {
            ival[i][j]=sqrt((double)((outpic1x[i][j]*outpic1x[i][j]) +
            (outpic1y[i][j]*outpic1y[i][j])));

            if (ival[i][j] > maxival){
                maxival = ival[i][j];
            }

        }
    }

    for (i=0;i<256;i++) { 
        for (j=0;j<256;j++) {
            ival[i][j] = (ival[i][j] / maxival) * 255;   
         
            fprintf(magFile,"%c",(char)((int)(ival[i][j])));
        }
    }
}
