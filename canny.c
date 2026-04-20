/*  Josie Goreczky
    CAP 4453 - Robot Vision
    Assignment 2
*/

#include <stdlib.h> /*  Marr-Hildreth.c  (or marrh.c) */
#include <stdio.h>
#include <math.h>
#define PICSIZE 256
#define MAXMASK 100

int pic[PICSIZE][PICSIZE];
int edgeflag[PICSIZE][PICSIZE];
int histogram[PICSIZE];
int final[PICSIZE][PICSIZE];
double maskx[MAXMASK][MAXMASK];
double masky[MAXMASK][MAXMASK];
double convx[PICSIZE][PICSIZE];
double convy[PICSIZE][PICSIZE];
double ival[PICSIZE][PICSIZE];

main(int argc, char **argv) {
    int i, j, p, q, mr, centx, centy;
    double area, cutOff, percent, slope, sumx, sumy, sig, maxival, xmask, ymask, hi, lo;
    char *foobar;
    FILE *input, *magFile, *peaksFile, *finalFile;

    /* cmd line: gcc canny.c -o canny -lm
                          ./canny inputFile.pgm magnitude.pgm peaks.pgm finaledges.pgm sig percent
                          
                          ./canny garb34.pgm magnitude.pgm peaks.pgm finaledges.pgm 1.0 0.05 */


    // input file
    argc--; argv++;
    foobar = *argv;
    input = fopen(foobar, "rb");

    // Magintude file
    argc--; argv++;
    foobar = *argv;
    magFile = fopen(foobar, "wb");

    // Peaks file
    argc--; argv++;
    foobar = *argv;
    peaksFile = fopen(foobar, "wb");

    // Final edges file
    argc--; argv++;
    foobar = *argv;
    finalFile = fopen(foobar, "wb");

    // Sig input
    argc--; argv++;
    foobar = *argv;
    sig = atof(foobar);

    // Percent input
    argc--; argv++;
    foobar = *argv;
    percent = atof(foobar);
    // percent = 0.05;

    mr = (int)(sig * 3);
    centx = (MAXMASK / 2);
    centy = (MAXMASK / 2);

    // Remove header from input
    char s[20];
    for (i = 1; i <= 4; i++) {
        fscanf(input, "%s", s);
    }

    // Read in File
    for (i = 0; i < PICSIZE; i++)
        for (j = 0; j < PICSIZE; j++)
            pic[i][j] = getc(input);

    // Derivatives
    for (p = -mr; p <= mr; p++) {
        for (q = -mr; q <= mr; q++) {
            xmask = (q * exp(-1 * ((p * p) + (q * q)) / (2 * (sig * sig))));
            ymask = (p * exp(-1 * ((p * p) + (q * q)) / (2 * (sig * sig))));

            maskx[p + centy][q + centx] = xmask;
            masky[p + centy][q + centx] = ymask;
        }
    }

    // Convolution
    for (i = mr; i <= 255 - mr; i++) {
        for (j = mr; j <= 255 - mr; j++) {
            sumx = 0;
            sumy = 0;
            for (p = -mr; p <= mr; p++) {
                for (q = -mr; q <= mr; q++) {
                    sumx += pic[i + p][j + q] * maskx[p + centy][q + centx];
                    sumy += pic[i + p][j + q] * masky[p + centy][q + centx];
                }
            }
            convx[i][j] = sumx;
            convy[i][j] = sumy;
        }
    }

    // Calculate magnitudes from two directions
    maxival = 0;
    for (i = mr; i < PICSIZE - mr; i++) {
        for (j = mr; j < PICSIZE - mr; j++) {
            ival[i][j] = sqrt((double)((convx[i][j] * convx[i][j]) +
                                       (convy[i][j] * convy[i][j])));

            if (ival[i][j] > maxival) {
                maxival = ival[i][j];
            }
        }
    }

    fprintf(magFile,"P5\n256 256\n255\n");

    for (i = 0; i < PICSIZE; i++) {
        for (j = 0; j < PICSIZE; j++) {
            ival[i][j] = (ival[i][j] / maxival) * 255;
            fprintf(magFile, "%c", (char)(int)ival[i][j]);
        }
    }

    // PART 2: RidgePeaks of Magnitude Images
    for (i = mr; i < PICSIZE - mr; i++){
        for (j = mr; j < PICSIZE - mr; j++) {
            if ((convx[i][j]) == 0.0) {
                convx[i][j] = .00001;
            }

            slope = convy[i][j] / convx[i][j];
            
            if ((slope <= .4142) && (slope > -.4142)) {
                if ((ival[i][j] > ival[i][j - 1]) && (ival[i][j] > ival[i][j + 1])) {
                    edgeflag[i][j] = 255;
                }
            } else if ((slope <= 2.4142) && (slope > .4142)) {
                if ((ival[i][j] > ival[i - 1][j - 1]) && (ival[i][j] > ival[i + 1][j + 1])) {
                    edgeflag[i][j] = 255;
                }
            } else if ((slope <= -.4142) && (slope > -2.4142)) {
                if ((ival[i][j] > ival[i + 1][j - 1]) && (ival[i][j] > ival[i - 1][j + 1])) {
                    edgeflag[i][j] = 255;
                }
            } else {
                if ((ival[i][j] > ival[i - 1][j]) && (ival[i][j] > ival[i + 1][j])) {
                    edgeflag[i][j] = 255;
                }
            }
        }
    }

    fprintf(peaksFile,"P5\n256 256\n255\n");

    for (i = 0; i < PICSIZE; i++)
        for (j = 0; j < PICSIZE; j++)
            fprintf(peaksFile, "%c", (char)(int)edgeflag[i][j]);


    // PART 4: Automatically pick thresholds
    for (i = 0; i < PICSIZE; i++)
        for (j = 0; j < PICSIZE; j++)
            histogram[(int)ival[i][j]]++;

    cutOff = percent * PICSIZE * PICSIZE ;
    area = 0;
    for (i = PICSIZE - 1; i >= 0; i--) {
        area += histogram[i];
        if (area > cutOff)
            break;
    }
    
    // Pick HI and LO 
    hi = i;
    lo = .35 * hi;

    printf("HI Threshold: %f\n", hi);
    printf("LO Threshold: %f\n", lo);

    // Part 3: Double thresholds
    for (i = 0; i < PICSIZE; i++) {
        for (j = 0; j < PICSIZE; j++) {
            final[i][j] = 0;
            if (edgeflag[i][j] == 255) {   
                if (ival[i][j] > hi) {
                    final[i][j] = 255;
                    edgeflag[i][j] = 0;
                } else if (ival[i][j] < lo) {
                    edgeflag[i][j] = final[i][j] = 0;
                }
            }
        }
    }

    int moretodo = 1;
    while (moretodo == 1) {
        moretodo = 0;
        for (i = 0; i < PICSIZE; i++) {
            for (j = 0; j < PICSIZE; j++) {
                if (edgeflag[i][j] == 255) {
                    for (p = -1; p <= 1; p++) {
                        for (q = -1; q <= 1; q++) {
                            if (final[i + p][j + q] == 255) {
                                final[i][j] = 255;
                                edgeflag[i][j] = 0;
                                moretodo = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    fprintf(finalFile,"P5\n256 256\n255\n");

    for (i = 0; i < PICSIZE; i++)
        for (j = 0; j < PICSIZE; j++)
            fprintf(finalFile, "%c", (char)((int)(final[i][j])));
}