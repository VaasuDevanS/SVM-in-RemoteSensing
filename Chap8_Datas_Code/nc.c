#include <stdio.h>
#include <math.h>


float testdata[75][4];
int testclass[75];
float traindata[75][4];
int trainclass[75];
float cent1[4], cent2[4], cent3[4];	/* Centroids */

float distance (float a, float b, float c, float d, float w, float x, float y, float z)
{
	float dist, sum=0.0f;

	sum = (a-w)*(a-w) + (b-x)*(b-x) + (c-y)*(c-y) + (d-z)*(d-z);
	dist = (float)sqrt((double)sum);
	return dist;
}

int main ()
{
	FILE *f;
	int i,j,class,k;
	float x, d, dmin;
	float confuse [4][4];

/*	Read all training data */
	f = fopen ("iris-train1.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-train1.txt. \n");
		return 0;
	}
	cent1[0] = cent1[1] = cent1[2] = cent1[3] = 0.0f;
	for (i=0; i<25; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		cent1[0] += x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		cent1[1] += x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		cent1[2] += x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		cent1[3] += x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);
	cent1[0] /= 25.0f; cent1[1] /= 25.0f; cent1[2] /= 25.0f; cent1[3] /= 25.0f;
	printf ("Centroid 1 is (%6f, %6f, %6f, %6f)\n", cent1[0], cent1[1], cent1[2], cent1[3]);

	f = fopen ("iris-train2.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-train2.txt. \n");
		return 0;
	}
	cent2[0] = cent2[1] = cent2[2] = cent2[3] = 0.0f;
	for (i=25; i<50; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		cent2[0] += x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		cent2[1] += x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		cent2[2] += x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		cent2[3] += x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);
	cent2[0] /= 25.0f; cent2[1] /= 25.0f; cent2[2] /= 25.0f; cent2[3] /= 25.0f;
	printf ("Centroid 2 is (%6f, %6f, %6f, %6f)\n", cent2[0], cent2[1], cent2[2], cent2[3]);

	f = fopen ("iris-train3.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-train3.txt. \n");
		return 0;
	}
	cent3[0] = cent3[1] = cent3[2] = cent3[3] = 0.0f;
	for (i=50; i<75; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		cent3[0] += x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		cent3[1] += x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		cent3[2] += x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		cent3[3] += x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);
	cent3[0] /= 25.0f; cent3[1] /= 25.0f; cent3[2] /= 25.0f; cent3[3] /= 25.0f;
	printf ("Centroid 3 is (%6f, %6f, %6f, %6f)\n", cent3[0], cent3[1], cent3[2], cent3[3]);


/*	Read all test data */
	f = fopen ("iris-test1.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-test1.txt. \n");
		return 0;
	}
	for (i=0; i<25; i++)
	{
		fscanf (f, "%f", &x); testdata[i][0] = x;
		fscanf (f, "%f", &x); testdata[i][1] = x;
		fscanf (f, "%f", &x); testdata[i][2] = x;
		fscanf (f, "%f", &x); testdata[i][3] = x;
		fscanf (f, "%d", &j); testclass[i] = j;
	}
	fclose (f);

	f = fopen ("iris-test2.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-test2.txt. \n");
		return 0;
	}
	for (i=25; i<50; i++)
	{
		fscanf (f, "%f", &x); testdata[i][0] = x;
		fscanf (f, "%f", &x); testdata[i][1] = x;
		fscanf (f, "%f", &x); testdata[i][2] = x;
		fscanf (f, "%f", &x); testdata[i][3] = x;
		fscanf (f, "%d", &j); testclass[i] = j;
	}
	fclose (f);

	f = fopen ("iris-test3.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-test3.txt. \n");
		return 0;
	}
	for (i=50; i<75; i++)
	{
		fscanf (f, "%f", &x); testdata[i][0] = x;
		fscanf (f, "%f", &x); testdata[i][1] = x;
		fscanf (f, "%f", &x); testdata[i][2] = x;
		fscanf (f, "%f", &x); testdata[i][3] = x;
		fscanf (f, "%d", &j); testclass[i] = j;
	}
	fclose (f);

	printf ("All test and training data is read in.\n");

/* Computer the distance between each test data vector and all of	*/
/* the training data vectors. Keep the class of the nearest.		*/

	confuse[0][0] = 0.0;	confuse[0][1] = 0.0;	confuse[0][2] = 0.0;
	confuse[1][0] = 0.0;	confuse[1][1] = 0.0;	confuse[1][2] = 0.0;
	confuse[2][0] = 0.0;	confuse[2][1] = 0.0;	confuse[2][2] = 0.0;

	for (i=0; i<75; i++)		/* For each test data */
	{
		dmin = 1.0e12;

		dmin = distance (testdata[i][0], testdata[i][1], testdata[i][2], testdata[i][3],
			cent1[0], cent1[1], cent1[2], cent1[3]);
		class = 1;

		d = distance (testdata[i][0], testdata[i][1], testdata[i][2], testdata[i][3],
				cent2[0], cent2[1], cent2[2], cent2[3]);
		if (d < dmin)
		{
			dmin = d;
			class = 2;
		}

		d = distance (testdata[i][0], testdata[i][1], testdata[i][2], testdata[i][3],
				cent3[0], cent3[1], cent3[2], cent3[3]);
		if (d < dmin)
		{
			dmin = d;
			class = 3;
		}

		confuse[testclass[i]-1][class-1] += 1;
		printf ("Test data %d is class %d (really class %d)  distance %f\n", 
			i, class, testclass[i], dmin);
	}


	printf ("     Confusion Matrix\n");
	printf ("===========================\n");
	printf ("     %6f     %6f    %6f\n", confuse[0][0], confuse[1][0], confuse[2][0]);
	printf ("     %6f     %6f    %6f\n", confuse[0][1], confuse[1][1], confuse[2][1]);
	printf ("     %6f     %6f    %6f\n", confuse[0][2], confuse[1][2], confuse[2][2]);

	return 1;
}
