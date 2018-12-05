#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
	int i;
	double urey;
	double ureymax=0.80;
	double ureymin=0.10;
	double ureyd=0.05;

	double beta;
	double betamax=0.50;
	double betamin=0.10;
	double betad=0.05;

	FILE* runme;
	runme=fopen("runme.sh", "w");
	fprintf(runme,"#!/bin/bash\n");
	fprintf(runme,"mkdir SCENARIOS\n");
	fprintf(runme,"rm registry.dat\n");
	for (urey=ureymin;urey<=ureymax;urey=urey+ureyd)
	{
		for (beta=betamin;beta<=betamax;beta=beta+betad)
		{
			fprintf(runme,"./thermev 0 %.2f %.2f\n", urey, beta);
			fprintf(runme,"mv timetemp_0_%.2f_%.2f.dat scenarios/timetemp_0_%.2f_%.2f.dat\n", urey, beta, urey, beta);
			fprintf(runme,"./thermev 1 %.2f %.2f\n", urey, beta);
			fprintf(runme,"mv timetemp_1_%.2f_%.2f.dat scenarios/timetemp_1_%.2f_%.2f.dat\n", urey, beta, urey, beta);
		}	
	}
fclose(runme);
return(0);
}


