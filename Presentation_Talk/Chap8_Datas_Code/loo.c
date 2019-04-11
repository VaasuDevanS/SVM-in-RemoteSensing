#include <stdio.h>
#include <math.h>


/* Data partitions, training and testing */

/* All data in the set */
float alldata[160][4];
int allclass[160];

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
	int i,j,class,k,part;
	float x, d, dmin, sum=0, success;
	float confuse [4][4];

/*	Read all data */
	f = fopen ("iris-data.txt", "r");
	if (f == NULL)
	{
		printf ("ERROR: Can't open iris-data.txt. \n");
		return 0;
	}

	for (i=0; i<150; i++)
	{
		fscanf (f, "%f", &x); alldata[i][0] = x;
		fscanf (f, "%f", &x); alldata[i][1] = x;
		fscanf (f, "%f", &x); alldata[i][2] = x;
		fscanf (f, "%f", &x); alldata[i][3] = x;
		fscanf (f, "%d", &j); allclass[i] = j;
	}
	fclose (f);

	for (i=0; i<150; i++)		/* For each test data */
	{
		dmin = 1.0e12;
		class = 0;
		for (j=0; j<150; j++)	/* for each training data */
		  if (i != j)
		  {
			d = distance (alldata[i][0], alldata[i][1], alldata[i][2], alldata[i][3],
				alldata[j][0], alldata[j][1], alldata[j][2], alldata[j][3]);
			if (d < dmin)
			{
				dmin = d;
				class = allclass[j];
			}
		  }
		printf ("Test data %d is class %d (really class %d)  distance %f\n", 
				i, class, allclass[i], dmin);
		if (allclass[i] == class)
		{
			sum += 1;
			printf ("success at test=%d\n", i);
		} else printf ("Fails at %d\n", i);
	}

	printf ("Overall success rate is %f\n", sum/150.0);
	return 1;
}
