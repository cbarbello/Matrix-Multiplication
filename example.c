#include <stdio.h>
int main(int argc, char **argv)
{
	double one_dim_array[6];
	int i;
	int j;

	one_dim_array[0] = 27;     /* (0,0) */
	one_dim_array[1] = -3.2;   /* (0,1) */
	one_dim_array[2] = 155.5;  /* (1,0) */
	one_dim_array[3] = 91;     /* (1,1) */
	one_dim_array[4] = 3;      /* (2,0) */
	one_dim_array[5] = 17.4;   /* (2,1) */

	for(i=0; i < 3; i++) {
		for(j=0; j < 2; j++) {
			printf("element (%d,%d): %f ",
				i,j,
				one_dim_array[i*2+j]);
		}
		printf("\n");
	}

	return(0);
}