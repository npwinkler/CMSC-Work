/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/* HW 3 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "checkit.h"

float mean(float m[], int len);
float stdDev(float m[], int len);
void zScores(float m[], float z[], int len);
int numOutliers(float m[], int len);
void whichOutliers(float m[], int len);

int main(void) {
	float a[7]={1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
	float b[8] = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
	float c[12] = {64.0f, 98.0f, 66.0f, 60.0f, 67.0f, 64.0f,
	70.0f, 73.0f, 59.0f, 75.0f, 35.0f, 75.0f};
	float cz[12];
	float d[8] = {1.0f, 2.0f, 0.0f, 2.0f, 3.0f, 1.0f, 3.0f, 4.0f};
	printf("The mean of array a is %f\n", mean(a, 7));
	checkit_float(4, mean(a,7));
	checkit_float(2, mean(b,8));
	printf("The std dev of array b is %f\n", stdDev(b, 8));
	checkit_float(0, stdDev(b,8));
	checkit_float(1.30931, stdDev(d,8));
	zScores(c, cz, 12);
	checkit_float(-0.219489, cz[0]);
	checkit_float(0.404322, cz[7]);
	checkit_float(-2.229545, cz[10]);
	printf("The # of outliers in array c is %d\n", numOutliers(c, 12));
	checkit_int(2, numOutliers(c,12));
	checkit_int(0, numOutliers(b,8));
	whichOutliers(c, 12);
	whichOutliers(b,8);
	return 0;
	}

float mean(float m[], int len) {
	float SumTerms = 0;
	int i = 0;
	for(i=0;i<len;i++)
		SumTerms += m[i];
	return SumTerms/(float)len;
	}

float stdDev(float m[], int len) {
	float Mean = mean(m, len);
	int i = 0;
	float sumSqrDev = 0; /* sum of squared deviations */
	for(i=0;i<len;i++) {
		sumSqrDev += pow( (m[i]-Mean), 2);
	}
	return sqrt(sumSqrDev/ (float)(len-1));
	}
	
void zScores(float m[], float z[], int len) {
	float Mean = mean(m, len);
	float StdDev = stdDev(m, len);
	int i = 0;
	for(i=0;i<len;i++)
		z[i] = (m[i]-Mean)/StdDev;
	}

int numOutliers(float m[], int len) {
	float zs[len];
	zScores(m, zs, len);
	int i = 0;
	int numOfOutliers = 0;
	for(i=0;i<len;i++) {
		if( fabs(zs[i]) > 2)
			numOfOutliers += 1;
		}
	return numOfOutliers;
	}
	
void whichOutliers(float m[], int len) {
	float zs[len];
	zScores(m, zs, len);
	int i = 0;
	if (numOutliers(m, len)==0)
		printf("There were no outliers.");
	else {
		printf("The outliers had values: ");
		for(i=0;i<len;i++) {
			if( fabs(zs[i]) > 2)
				printf("%f ", zs[i]);
			}
		}
	printf("\n");
	}