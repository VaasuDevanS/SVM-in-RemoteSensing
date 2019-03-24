#include <stdio.h>
#include <math.h>


/* Data partitions, training and testing */
float testdata[160][4];
int testclass[160], Ntest=30;
float traindata[160][4];
int trainclass[160], Ntrain=120;

/* All data in the set */
float alldata[160][4];
float allclass[160];

float distance (float a, float b, float c, float d, float w, float x, float y, float z)
{
	float dist;

	dist = (a-w)*(a-w) + (b-x)*(b-x) + (c-y)*(c-y) + (d-z)*(d-z);
	dist = (float)sqrt((double)dist);
	return dist;
}


/* This partition on;ly works for the Iris data set. There are 150 items, 50	*/
/* in each class. The first 50 are class 1, next 50 class 2, etc. So we can	*/
/* take 10  from each class into the testing set, with the remainder put into	*/
/* the training set.								*/
void partition (int k)
{
	int i,j,n;
	int testStart, testEnd;

	j = 0; n = 0;

/* Class 1 */
	testStart = (k)*10;
	testEnd = testStart+10;
	printf ("Partition %d: test from %d to %d\n", k, testStart, testEnd-1);
	for (i=0; i<50; i++)
	{
	  if ( (i>=testStart) && (i<testEnd) ) /* Test data */
	  {
		testdata[j][0] = alldata[i][0];
		testdata[j][1] = alldata[i][1];
		testdata[j][2] = alldata[i][2];
		testdata[j][3] = alldata[i][3];
		testclass[j++] = allclass[i];
	  } else
	  {
                traindata[n][0] = alldata[i][0];
                traindata[n][1] = alldata[i][1];
                traindata[n][2] = alldata[i][2];
                traindata[n][3] = alldata[i][3];
		trainclass[n++]  = allclass[i];
	  }
	}
	printf ("Ntest %d Ntrain %d so far ...\n", j, n);

/* Class 2 */
	testStart = (k)*10 + 50;
	testEnd = testStart+10;
	printf ("Partition %d: test from %d to %d\n", k, testStart, testEnd-1);
	for (i=50; i<100; i++)
	{
	  if ( (i>=testStart) && (i<testEnd) ) /* Test data */
	  {
		testdata[j][0] = alldata[i][0];
		testdata[j][1] = alldata[i][1];
		testdata[j][2] = alldata[i][2];
		testdata[j][3] = alldata[i][3];
		testclass[j++] = allclass[i];
	  } else
	  {
                traindata[n][0] = alldata[i][0];
                traindata[n][1] = alldata[i][1];
                traindata[n][2] = alldata[i][2];
                traindata[n][3] = alldata[i][3];
		trainclass[n++]  = allclass[i];
	  }
	}
	printf ("Ntest %d Ntrain %d so far ...\n", j, n);

/* Class 3 */
	testStart = (k)*10 + 100;
	testEnd = testStart+10;
	printf ("Partition %d: test from %d to %d\n", k, testStart, testEnd-1);
	for (i=100; i<150; i++)
	{
	  if ( (i>=testStart) && (i<testEnd) ) /* Test data */
	  {
		testdata[j][0] = alldata[i][0];
		testdata[j][1] = alldata[i][1];
		testdata[j][2] = alldata[i][2];
		testdata[j][3] = alldata[i][3];
		testclass[j++] = allclass[i];
	  } else
	  {
                traindata[n][0] = alldata[i][0];
                traindata[n][1] = alldata[i][1];
                traindata[n][2] = alldata[i][2];
                traindata[n][3] = alldata[i][3];
		trainclass[n++]  = allclass[i];
	  }
	}

	printf ("Test: %d  train: %d\n\n", j, n);
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

	printf ("All test and training data is read in.\n");

/* Partition into 5 sets, and iterate between them */
	for (part=0; part<5; part++)
	{
		partition (part);

/* Now perform the experiment; in this case a nearest neighbor classification */
/* of all test data, giving a confusion matrix and success rate.		*/
		confuse[0][0] = 0.0;	confuse[0][1] = 0.0;	confuse[0][2] = 0.0;
		confuse[1][0] = 0.0;	confuse[1][1] = 0.0;	confuse[1][2] = 0.0;
		confuse[2][0] = 0.0;	confuse[2][1] = 0.0;	confuse[2][2] = 0.0;

		for (i=0; i<Ntest; i++)		/* For each test data */
		{
			dmin = 1.0e12;
			class = 0;
			k = -1;
			for (j=0; j<Ntrain; j++)	/* for each training data */
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

		success = ((confuse[0][0]+confuse[1][1]+confuse[2][2])/Ntest) * 100.0F;
		printf ("Success rate for part %d is %f\n", part, success);
		sum += success;
	}

	printf ("Overall success rate is %f\n", sum/5.0);
	return 1;
}
