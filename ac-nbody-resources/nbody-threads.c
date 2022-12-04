#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define CONST_GravConstant 0.01

void *aThread(void *arg)
{
	long tid = (long)arg;
	long square = tid * tid; // A
	// double square = tid*tid; // B

	printf("thread %ld: Hello World !\n", tid);
	pthread_exit((void *)square);
}

typedef struct
{
	double x, y, z;
} vector;

int GLOBAL_numBodies = 15;	 // default number of bodies
int GLOBAL_numSteps = 30000; // default number of time steps (1 million)

int GLOBAL_windowWidth = 800;	 // default window width is 800 pixels
int GLOBAL_windowHeight = 800; // default window height is 800 pixels

double *GLOBAL_masses;
vector *GLOBAL_positions;
vector *GLOBAL_velocities;
vector *GLOBAL_accelerations;
int NUM_THREADS = 1;

////////////////////////////////////////////////////////////////////////
vector addVectors(vector a, vector b)
{
	vector c = {a.x + b.x, a.y + b.y, a.z + b.z};
	return c;
}

////////////////////////////////////////////////////////////////////////
vector scaleVector(double b, vector a)
{
	vector c = {b * a.x, b * a.y, b * a.z};

	return c;
}

////////////////////////////////////////////////////////////////////////
vector subtractVectors(vector a, vector b)
{
	vector c = {a.x - b.x, a.y - b.y, a.z - b.z};

	return c;
}

////////////////////////////////////////////////////////////////////////
double mod(vector a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

////////////////////////////////////////////////////////////////////////
void initSystemFromRandom()
{
	int i;

	GLOBAL_masses = (double *)malloc(GLOBAL_numBodies * sizeof(double));
	GLOBAL_positions = (vector *)malloc(GLOBAL_numBodies * sizeof(vector));
	GLOBAL_velocities = (vector *)malloc(GLOBAL_numBodies * sizeof(vector));
	GLOBAL_accelerations = (vector *)malloc(GLOBAL_numBodies * sizeof(vector));

	for (i = 0; i < GLOBAL_numBodies; i++)
	{
		GLOBAL_masses[i] = rand() % GLOBAL_numBodies;
		GLOBAL_positions[i].x = rand() % GLOBAL_windowWidth;
		GLOBAL_positions[i].y = rand() % GLOBAL_windowHeight;
		GLOBAL_positions[i].z = 0;
		GLOBAL_velocities[i].x = GLOBAL_velocities[i].y = GLOBAL_velocities[i].z = 0;
	}
}

////////////////////////////////////////////////////////////////////////
void showSystem()
{
	int i;

	for (i = 0; i < GLOBAL_numBodies; i++)
	{
		fprintf(stderr, "Body %d : %lf\t%lf\t%lf\t|\t%lf\t%lf\t%lf\n", i,
						GLOBAL_positions[i].x, GLOBAL_positions[i].y, GLOBAL_positions[i].z,
						GLOBAL_velocities[i].x, GLOBAL_velocities[i].y, GLOBAL_velocities[i].z);
	}
}

////////////////////////////////////////////////////////////////////////
void *parallel_computeAccelerations(void *arg)
{
	long tid = (long)arg;
	int i, j;

	for (i = tid; i < GLOBAL_numBodies; i += NUM_THREADS)
	{
		GLOBAL_accelerations[i].x = 0;
		GLOBAL_accelerations[i].y = 0;
		GLOBAL_accelerations[i].z = 0;
		for (j = 0; j < GLOBAL_numBodies; j++)
		{
			if (i != j)
			{
				GLOBAL_accelerations[i] = addVectors(GLOBAL_accelerations[i], scaleVector(CONST_GravConstant * GLOBAL_masses[j] / pow(mod(subtractVectors(GLOBAL_positions[i], GLOBAL_positions[j])), 3), subtractVectors(GLOBAL_positions[j], GLOBAL_positions[i])));
			}
		}
	}

	pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////
void *parallel_computePositions(void *arg)
{
	long tid = (long)arg;
	int i;

	for (i = tid; i < GLOBAL_numBodies; i += NUM_THREADS)
	{
		GLOBAL_positions[i] = addVectors(GLOBAL_positions[i], addVectors(GLOBAL_velocities[i], scaleVector(0.5, GLOBAL_accelerations[i])));
	}

	pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////
void *parallel_computeVelocities(void *arg)
{
	long tid = (long)arg;
	int i;

	for (i = tid; i < GLOBAL_numBodies; i += NUM_THREADS)
	{
		GLOBAL_velocities[i] = addVectors(GLOBAL_velocities[i], GLOBAL_accelerations[i]);
	}

	pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////
void *parallel_resolveCollisions(void *arg)
{
	long tid = (long)arg;
	int i, j;

	for (i = tid; i < GLOBAL_numBodies; i += NUM_THREADS)
		for (j = i + 1; j < GLOBAL_numBodies; j++)
		{
			if (GLOBAL_positions[i].x == GLOBAL_positions[j].x && GLOBAL_positions[i].y == GLOBAL_positions[j].y && GLOBAL_positions[i].z == GLOBAL_positions[j].z)
			{
				vector temp = GLOBAL_velocities[i];
				GLOBAL_velocities[i] = GLOBAL_velocities[j];
				GLOBAL_velocities[j] = temp;
			}
		}
	pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////
void validateSystem()
{
	int i;

	for (i = 0; i < GLOBAL_numBodies; i++)
	{
		if (isnan(GLOBAL_positions[i].x) || isnan(GLOBAL_positions[i].y) || isnan(GLOBAL_positions[i].z) ||
				isnan(GLOBAL_velocities[i].x) || isnan(GLOBAL_velocities[i].y) || isnan(GLOBAL_velocities[i].z))
		{
			fprintf(stderr, "NAN Body %d : %lf\t%lf\t%lf\t|\t%lf\t%lf\t%lf\n", i,
							GLOBAL_positions[i].x, GLOBAL_positions[i].y, GLOBAL_positions[i].z,
							GLOBAL_velocities[i].x, GLOBAL_velocities[i].y, GLOBAL_velocities[i].z);
			exit(1);
		}
	}
}

void parallel_simulate()
{
	pthread_t threads[NUM_THREADS];
	int t;

	// computeAccelerations
	for (t = 0; t < NUM_THREADS; t++)
		pthread_create(&threads[t], NULL, parallel_computeAccelerations, (void *)t);
	for (t = 0; t < NUM_THREADS; t++)
		pthread_join(threads[t], NULL);

	// computePositions
	for (t = 0; t < NUM_THREADS; t++)
		pthread_create(&threads[t], NULL, parallel_computePositions, (void *)t);
	for (t = 0; t < NUM_THREADS; t++)
		pthread_join(threads[t], NULL);

	// computeVelocities
	for (t = 0; t < NUM_THREADS; t++)
		pthread_create(&threads[t], NULL, parallel_computeVelocities, (void *)t);
	for (t = 0; t < NUM_THREADS; t++)
		pthread_join(threads[t], NULL);

	// resolveCollisions
	for (t = 0; t < NUM_THREADS; t++)
		pthread_create(&threads[t], NULL, parallel_resolveCollisions, (void *)t);
	for (t = 0; t < NUM_THREADS; t++)
		pthread_join(threads[t], NULL);

	validateSystem();
}

void freeAll()
{
	free(GLOBAL_masses);
	free(GLOBAL_positions);
	free(GLOBAL_velocities);
	free(GLOBAL_accelerations);
}

////////////////////////////////////////////////////////////////////////
int main(int argC, char *argV[])
{
	int i;
	const unsigned int randSeed = 10; // default value; do not change

	if (argC >= 3)
	{
		GLOBAL_numBodies = atoi(argV[1]);
		GLOBAL_numSteps = atoi(argV[2]);
		NUM_THREADS = atoi(argV[3]);

		if (argC >= 5)
		{
			GLOBAL_windowWidth = atoi(argV[4]);
			GLOBAL_windowHeight = atoi(argV[5]);
		}
	}
	else if (argC != 1)
	{
		printf("Usage : %s [numBodies numSteps [windowWidth windowHeight]]\n", argV[0]);
		exit(1);
	}

	srand(randSeed);
	initSystemFromRandom();

	for (i = 0; i < GLOBAL_numSteps; i++)
	{
		parallel_simulate();
	}
	showSystem();

	freeAll();

	return 0;
}