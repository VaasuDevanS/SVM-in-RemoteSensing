#include <stdio.h>
#include <math.h>


float testdata[75][4];
int testclass[75];
float traindata[75][4];
int trainclass[75];

float distance (float a, float b, float c, float d, float w, float x, float y, float z)
{
	float dist;

	dist = (a-w)*(a-w) + (b-x)*(b-x) + (c-y)*(c-y) + (d-z)*(d-z);
	dist = (float)sqrt((double)dist);
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
	for (i=0; i<25; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);

	f = fopen ("iris-train2.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-train2.txt. \n");
		return 0;
	}
	for (i=25; i<50; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);

	f = fopen ("iris-train3.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-train3.txt. \n");
		return 0;
	}
	for (i=50; i<75; i++)
	{
		fscanf (f, "%f", &x); traindata[i][0] = x;
		fscanf (f, "%f", &x); traindata[i][1] = x;
		fscanf (f, "%f", &x); traindata[i][2] = x;
		fscanf (f, "%f", &x); traindata[i][3] = x;
		fscanf (f, "%d", &j); trainclass[i] = j;
	}
	fclose (f);


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
		class = 0;
		k = -1;
		for (j=0; j<75; j++)	/* for each training data */
		{
			d = distance (testdata[i][0], testdata[i][1], testdata[i][2], testdata[i][3],
				traindata[j][0], traindata[j][1], traindata[j][2], traindata[j][3]);
			if (d < dmin)
			{
				dmin = d;
				class = trainclass[j];
				k = j;
			}
		}
		confuse[testclass[i]-1][class-1] += 1;
		printf ("Test data %d is class %d (really class %d) at %d distance %f\n", 
			i, class, testclass[i], k, dmin);
	}


	printf ("     Confusion Matrix\n");
	printf ("===========================\n");
	printf ("     %6f     %6f    %6f\n", confuse[0][0], confuse[1][0], confuse[2][0]);
	printf ("     %6f     %6f    %6f\n", confuse[0][1], confuse[1][1], confuse[2][1]);
	printf ("     %6f     %6f    %6f\n", confuse[0][2], confuse[1][2], confuse[2][2]);

	return 1;
}
