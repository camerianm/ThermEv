//Description: Stages parameters for Thermev grid runs.
//To compile: gcc -lm -o stageparams stageparams.c
//To run: ./stageparams

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
	int i;
	double timeper=0.3		;//number of seconds taken for each run, on average. Update based on your experiences, if desired!
	
	double beta 			;//Coefficient for Nusselt-Rayleigh scaling; determines temperature-dependent viscosity assumptions
	double betamin=0.10		;//Positive values: Hi temps lower viscosity. More positive --> lowers it a LOT.
	double betamax=0.30		;//Negative values: High temps --> more melt generated and extruded --> more crust --> more viscous.
	double betad=0.02		;//size of steps between subsequent beta values tested. Start with 0.10 to 0.30, or see arXiv:1901.02136

	//BACKWARD INTEGRATION PARAMETERS - Values assumed to apply at present
	double urey  			;//Urey ratio = Radiogenic heat produced (H(t=present)) over heat loss from secular cooling (Q(t=present))
	double ureymin=0.04		;//minimum Urey ratio to include in grid; Korenaga (2008) suggests <=0.15, some crustal data suggests higher
	double ureymax=0.30		;//maximum Urey ratio to include in grid; Christensen (1984) suggests >=0.7 from assuming beta=0.33.
	double ureyd=0.02		;//size of steps between subsequent Urey ratios tested. Start with 0.10 to 0.30, or see DOI:10.1029/2007RG000241

	//FORWARD INTEGRATION PARAMETERS - Values assumed to apply ~4.54GA.
	double furey 			;//Urey ratio = Radiogenic heat produced (H(t=present)) over heat loss from secular cooling (Q(t=present))			
	double fureymin=0.84	;//Want to skip this step end up with a roughly plausible final Urey ratio?
	double fureymax=1.10	;//Comment this section out, and uncomment the below section.
	double fureyd=0.02		;//Don't want to run one mode (forward or backward)? Comment out the mode you don't want.


	FILE* runme;
	runme=fopen("runme.sh", "w");
	fprintf(runme,"#!/bin/bash\n");
	fprintf(runme,"mkdir SCENARIOS\n");
	fprintf(runme,"rm registry_fwd.dat\n");
	fprintf(runme,"rm registry_back.dat\n");
	fprintf(runme,"rm summary.dat\n");

	//Reverse mode
	for (beta=betamin;beta<=betamax;beta=beta+betad)
	{
		for (urey=ureymin;urey<=ureymax;urey=urey+ureyd)
		{
			fprintf(runme,"./thermev 0 %.2f %.2f\n", urey, beta);
			//fprintf(runme,"mv timetemp_0_%.2f_%.2f.dat SCENARIOS/timetemp_0_%.2f_%.2f.dat\n", urey, beta, urey, beta);
			i++;
		}	
	}

	//Forward mode
	for (beta=betamin;beta<=betamax;beta=beta+betad)
	{
		for (furey=fureymin;furey<=fureymax;furey=furey+fureyd)
		{
			fprintf(runme,"./thermev 1 %.2f %.2f\n", furey, beta);
			//fprintf(runme,"mv timetemp_1_%.2f_%.2f.dat SCENARIOS/timetemp_1_%.2f_%.2f.dat\n", furey, beta, furey, beta);
			i++;
		}
	}

fprintf(stderr,"\n%d iterations scheduled. Estimated time required: %.f minutes.\n",i,i*(timeper/60.));
fprintf(stderr,"To reduce time: Narrow maxes/mins or increase step size. Then recompile.\n");
fprintf(stderr,"To run, type: chmod +x runme.sh\nThen type: ./runme.sh\n");
fclose(runme);
return(0);
}
