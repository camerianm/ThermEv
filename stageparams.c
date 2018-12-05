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

	FILE* flist;
	flist=fopen("flist.sh", "w");
	fprintf(flist,"#!/bin/bash\n");
	fprintf(flist,"mkdir SCENARIOS\n");
	fprintf(flist,"rm registry.dat\n");
	for (urey=ureymin;urey<=ureymax;urey=urey+ureyd)
	{
		for (beta=betamin;beta<=betamax;beta=beta+betad)
		{
			fprintf(flist,"./thermev 0 %.2f %.2f\n", urey, beta);
			fprintf(flist,"mv timetemp_0_%.2f_%.2f.dat scenarios/timetemp_0_%.2f_%.2f.dat\n", urey, beta, urey, beta);
			fprintf(flist,"./thermev 1 %.2f %.2f\n", urey, beta);
			fprintf(flist,"mv timetemp_1_%.2f_%.2f.dat scenarios/timetemp_1_%.2f_%.2f.dat\n", urey, beta, urey, beta);
		}	
	}
fclose(flist);
return(0);
}


