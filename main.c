// SFD THERMEV CODE

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char**argv)
{
	if (argc<4)
	{
		fprintf(stderr,"\n\nPlease re-try with arguments as follows:\n");
		fprintf(stderr,"[executable] [int:Forward? 0or1] [dbl:UreyRatio] [dbl:beta]\n");
		fprintf(stderr,"Example: thermev 0 0.72 0.30\n\n");
		exit(10);
	}

	double 	urey=atof(argv[2]);		//urey ratio, [H(t) / Q(t)]
	double 	B=atof(argv[3]);			//temp dependence of viscosity; may be variable
	int    	forward=atoi(argv[1]);		//0 places t=0 at present, evolves back in time; cuts off at T>8000K.
							//assumes Q0, H0, and T0 are present-day values.
							//1 places t=0 at formation, evolves forward.
							//note: possibly valid for stagnant-lid regimes;
							//future updates may represent other regimes
	char summary[50];
	char timetemp[50];
//	sprintf(summary,"summary_%d_%.2f_%.2f.dat",forward,urey,B);
	sprintf(timetemp,"timetemp_%d_%.2f_%.2f.dat",forward,urey,B);
	sprintf(summary,"summary.dat");

	double E=300.0*1.0E3;		//activation energy, J/mol; isoviscous=0. 300*1.0E3 Korenaga ; 525*1.0E3 MacNamara
	double Q0=3.6e13;		//present day heat flux, W (J/s)

	double H0=Q0*urey;		//present heat production, W (J/s)
	double T0_C=1350.0;		//mantle potential temp OR average temp
	double C_JK=7.0e27;		//heat capacity of whole earth in J/K
	double T0=T0_C+273.15;		//converts temp to K
	double R=8.3145; 		//ideal gas constant in J/mol*K

	double t_Ga=0.0, dt_Ga=0.005;	//timestep in Ga
	double GatoS=3.1536e16;		//converts from Ga to seconds
	double t=0.0, dt=dt_Ga*GatoS;
	double T=T0, dT;

	int i, iter=0;

	double mass=1.00;		//planet mass (in earth masses); guides heat capacity
	double radius=1.00;		//planet radius (in earth radii) - no role yet except grav.
	double G=6.67408e-11, Re=6.371e6, Me=5.9722e24;
   	double grav=G*(Me*mass)/(pow((Re*radius),2));	//calculates surface gravity
    	double Cp=C_JK*mass;		//assumes heat capacity roughly scales with mass
 
	double h_n[4]={0.372, 0.0164, 0.430, 0.181}; // fraction of total heat production at t=0
	double L_n[4]={0.155, 0.985, 0.0495, 0.555}; // lambda - decay/growth constant in 1/Gyr
    	double H_n[4], Qt, Ht=0.0; // total production; multiplier for each species

	FILE*ky; ky=fopen(summary,"w");
	fprintf(ky,"timetemp.dat header info:\nCol.3-6 show fraction of H(t=0)\nCol.7-10 show fraction of H(t=time)\n\n");
	fprintf(ky,"Col01: Time(GA)\nCol02: Temp(C)\nCol03: 238U\nCol04: 235U\nCol05: 232Th\nCol06: 40K\n");
	fprintf(ky,"Col07: 238U/H(t)\nCol08: 235U/H(t)\nCol09: 232Th/H(t)\nCol10: 40K/H(t)\nCol11: H(t)/H0\nCol12: H(t)\nCol13: UreyRatio\n");
	fprintf(ky,"\nregistry header info: #iterations filename\n");

	FILE*fp; fp=fopen(timetemp,"w");
	fprintf(fp,"%.3f %.3f %.5f %.5f %.5f %.5f", t_Ga, T0_C, h_n[0], h_n[1], h_n[2], h_n[3]);
	fprintf(fp,"%.5f %.5f %.5f %.5f %.5f %.3e %.5f\n", h_n[0], h_n[1], h_n[2], h_n[3], H0/H0, H0, H0/Q0);


	dT=((H0-Q0)/C_JK)*dt;	//Change in temp that should apply between timesteps 0 and 1
	

	if (forward==0)				//If we are running backward in time...
	{
		while ((t_Ga<4.6) && (T<8000))	
		{
			T=T-dT;				//Find out what mantle temp would be at timestep, to inform tdep visc.
			t_Ga=t_Ga+dt_Ga;
			t=t_Ga*GatoS;
			fprintf(fp,"%.3f %.3f ", t_Ga, T-273.15); 	//Starts to log info @ timestep...

	        Qt = Q0*pow((T/T0), (1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);

			for (i=0;i<4;i++)	//Calculate how heat production changes for each radionuclide, and total.
			{
				H_n[i]=h_n[i]*exp(L_n[i]*t_Ga);		//exponential growth - i.e. decay in reverse
				Ht=Ht+H_n[i];
				fprintf(fp,"%.5f ",H_n[i]);
			}

			for (i=0;i<4;i++)	//Calculate each radionuclide's fraction of total heating at time t
			{
				fprintf(fp,"%.5f ", H_n[i]/Ht);
			}

			fprintf(fp,"%.5f ",Ht);
			Ht=Ht*H0;
			fprintf(fp,"%.3e",Ht);
			fprintf(fp,"%.5f\n",Ht/Qt);
	        	dT=((Ht-Qt)/C_JK)*dt;		//find temp difference for next time step
			iter++;
			Ht=0.0;
		}
	}
	else
	{
		//h_n[4]={0.19447,0.21200,0.14951,0.44402};
		while (t_Ga<4.6)
		{
			T=T+dT;				//Find out what mantle temp would be at timestep, to inform tdep visc.
			t_Ga=t_Ga+dt_Ga;
			t=t_Ga*GatoS;
			fprintf(fp,"%.3f %.3f ", t_Ga, T-273.15); 	//Starts to log info @ timestep...

	        Qt = Q0*pow((T/T0), (1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);

			for (i=0;i<4;i++)	//Calculate how heat production changes for each radionuclide, and total.
			{
				H_n[i]=h_n[i]*exp(-1*L_n[i]*t_Ga);	//exponential decay
				Ht=Ht+H_n[i];
				fprintf(fp,"%.5f ",H_n[i]);
			}

			for (i=0;i<4;i++)	//Calculate each radionuclide's fraction of total heating at time t
			{
				fprintf(fp,"%.5f ", H_n[i]/Ht);
			}

			fprintf(fp,"%.5f ",Ht);
			Ht=Ht*H0;
			fprintf(fp,"%.3e ",Ht);
			fprintf(fp,"%.5f\n", Ht/Qt);
	        	dT=((Ht-Qt)/C_JK)*dt;		//find temp difference for next time step
			iter++;
			Ht=0.0;
	    }
	}

FILE*reg; reg=fopen("registry.dat","a");
fprintf(reg,"%.2f %d %.3f %.3f timetemp_%d_%.2f_%.2f.dat\n", t_Ga,forward, urey, B, forward,urey,B);
fprintf(stdout,"Done with Forward=%d\tUrey=%.3f\tBeta=%.3f\n", forward, urey, B);
fclose(ky);
fclose(fp);
fclose(reg);
return 0;
}
