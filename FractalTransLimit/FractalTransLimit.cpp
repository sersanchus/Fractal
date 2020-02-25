#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc,char **argv)
{
	if (argc<4)
	{
		printf("Use mode: s-contrast, o-brightness, prec\n");
		return 1;
	}

	float s=(float)atof(argv[1]);
	float o=(float)atof(argv[2]);
	float prec=(float)atof(argv[3]);

	float aux;
	float sem=0.0f;
	unsigned int cont=0;
	do
	{
		aux=sem;
		sem=aux*s+o;
		cont++;
	}
	while(fabs(sem-aux)>prec);

	printf("%d passes: %f\n",cont,sem);

	return 0;
}