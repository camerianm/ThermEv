// SFD THERMEV CODE

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char**argv)
{
	if (argc<4)
	{
		fprintf(stderr,"\n\nPlease re-try with input as follows:\n");
		fprintf(stderr,"[executable] [int:Forward?0or1] [dbl:UreyRatio] [dbl:beta]\n");
		fprintf(stderr,"Example: thermev 0 0.72 0.30\n\n");
		exit(10);
	}

	int    	forward=atoi(argv[1]);		//0 places t=0 at present, evolves back in time; cuts off at T>8000K.
							//assumes Q0, H0, and T0 are present-day values.
							//1 places t=0 at formation, evolves forward.
							//note: possibly valid for stagnant-lid regimes;
							//future updates may represent other regimes
	double 	urey=atof(argv[2]);		//urey ratio, [H(t) / Q(t)]
	double 	B=atof(argv[3]);		//temp dependence of viscosity; may be variable

	char summary[50];
	char timetemp[50];
	sprintf(timetemp,"SCENARIOS/timetemp_%d_%.2f_%.2f.dat",forward,urey,B);
	sprintf(summary,"summary.dat");

	double Q0;					//initial heat flux, W (J/s)
	double H0;				//present heat production, W (J/s)
	double T0;				//mantle potential temp OR average temp, in K
//	double T0_C;
	double C_JK=7.0e27;		//heat capacity of whole earth in J/K
	double E=300.0*1.0E3;		//activation energy, J/mol; isoviscous=0. 300*1.0E3 Korenaga ; 525*1.0E3 MacNamara
	double R=8.3145; 		//ideal gas constant in J/mol*K

	double T;
	double dT;
	double GatoS=3.1536e16;		//converts from Ga to seconds
	double t_Ga=0.0;
	double dt_Ga=0.005;	//timestep in Ga
	double t=t_Ga*GatoS;
	double dt=dt_Ga*GatoS;

	int i;
	int iter=0;

/*	
	double mass=1.00;		//planet mass (in earth masses); guides heat capacity
	double radius=1.00;		//planet radius (in earth radii) - no role yet except grav.
	double G=6.67408e-11, Re=6.371e6, Me=5.9722e24;
   	double grav=G*(Me*mass)/(pow((Re*radius),2));	//calculates surface gravity
    double Cp=C_JK*mass;		//assumes heat capacity roughly scales with mass
*/
	
	double L_n[4]={0.155, 0.985, 0.0495, 0.555}; // lambda - decay/growth constant in 1/Gyr
    double H_n[4], h_n[3], Qt, Ht=0.0; // total production; multiplier for each species
	
	if(forward==0)		//backward in time
	{
		h_n[0]=0.372;
		h_n[1]=0.0164;
		h_n[2]=0.430;
		h_n[3]=0.181;	// fraction of total heat production at t=0
		Q0=36.e12;		//present day heat flux, W (J/s)
		H0=Q0*urey;
		T0=1350.0+273.15;	//kelvin
		T=T0;
	}
	if(forward==1)		//forward in time
	{
		h_n[0]=0.14715;
		h_n[1]=0.29649;
		h_n[2]=0.10464;
		h_n[3]=0.45172;		//extrapolated back via exp growth from present ratios, fraction of isoviscous urey-0.25 case
		Q0=104.92925e12;	//since this supposedly scales with beta... how treat? 104.---
		//urey=1.00;		//suggested urey: ~1.15
		H0=Q0*urey;			
		T0=1800.0+273.15;	//kelvin
		T=T0;
	}


	FILE*ky; ky=fopen(summary,"w");
	fprintf(ky,"EACH FILE:\n0=timeGa\n1=Urey\n2=Qt\n3=heat/lsH0\n4=heatTW\n5=1HPE\n6=2HPE\n7=3HPE\n8=4HPE\n9=TempC\n");
/*	fprintf(ky,"Col01: Time(GA)\nCol02: Temp(C)\nCol03: 238U\nCol04: 235U\nCol05: 232Th\nCol06: 40K\n");
/*	fprintf(ky,"Col07: 238U/H(t)\nCol08: 235U/H(t)\nCol09: 232Th/H(t)\nCol10: 40K/H(t)\nCol11: H(t)/H0\nCol12: H(t)\nCol13: UreyRatio\n");
	fprintf(ky,"\nregistry header info: #iterations filename\n");
*/

	FILE*fp; fp=fopen(timetemp,"w");
	fprintf(fp,"timeGa Urey Qt heat/H0 heatTW 1HPE 2HPE 3HPE 4HPE TempC\n");

//	fprintf(fp,"%.3f %.3f %.5f %.5f %.5f %.5f", t_Ga, T0_C, h_n[0], h_n[1], h_n[2], h_n[3]);
//	fprintf(fp," %.5f %.5f %.5f %.5f %.5f %.3e %.5f\n", h_n[0], h_n[1], h_n[2], h_n[3], H0/H0, H0, H0/Q0);
//	dT=((H0-Q0)/C_JK)*dt;	//Change in temp that should apply between timesteps 0 and 1
	
	if (forward==0)				//If we are running backward in time...
	{
		while ((t_Ga<4.6) && (T>1350) && (T<2100))	//less than 1800*C<<mantle melting temp	
		{ 	//Find out what mantle temp would be at timestep, to inform tdep visc.
			t_Ga=t_Ga+dt_Ga;
			t=t_Ga*GatoS;
			for (i=0;i<4;i++)
			{ //Calculate how heat production changes for each radionuclide, and total.
				H_n[i]=h_n[i]*exp(L_n[i]*t_Ga);	//exponential growth - i.e. decay in reverse
				Ht=Ht+H_n[i];
				//fprintf(fp,"%.5f ",H_n[i]);
			}

	        Qt = Q0*pow((T/T0),(1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);
			fprintf(fp,"%.3f %.5f %.5f %.5f %.5f ",t_Ga,Ht*H0/Qt,Qt/(1e12),Ht,Ht*H0/(1e12));

			for (i=0;i<4;i++)
			{ //Calculate each radionuclide's fraction of total heating at time t
				fprintf(fp,"%.5f ", H_n[i]/Ht); ///change here/??
			}

			Ht=Ht*H0;
       		dT=((Ht-Qt)/C_JK)*dt;
			T=T-dT; //calculate temp difference
			fprintf(fp,"%.3f\n", T-273.15);
			Ht=0.0;
			iter++;

		}
	}
	else
	{	
		while ((t_Ga<4.6) && (T>1350) && (T<2100))
		{ 	//Find out what mantle temp would be at timestep, to inform tdep visc.
			//T=T+dT;
			t_Ga=t_Ga+dt_Ga;
			t=t_Ga*GatoS;

			for (i=0;i<4;i++)
			{ //Calculate how heat production changes for each radionuclide, and total.
				H_n[i]=h_n[i]*exp(-1*L_n[i]*t_Ga);	//exponential decay
				Ht=Ht+H_n[i];
				//fprintf(fp,"%.5f ",H_n[i]);
			}

	        Qt = Q0*pow((T/T0),(1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);
			fprintf(fp,"%.3f %.5f %.5f %.5f %.5f ",t_Ga,Ht*H0/Qt,Qt/(1e12),Ht,Ht*H0/(1e12));

			for (i=0;i<4;i++)	//Calculate each radionuclide's fraction of total heating at time t
			{
				fprintf(fp,"%.5f ", H_n[i]/Ht);
			}

			Ht=Ht*H0;
       		dT=((Ht-Qt)/C_JK)*dt;
			T=T+dT; //calculate temp difference - is this right? or should be plus? exp decay
			fprintf(fp,"%.3f\n", T-273.15);
			Ht=0.0;
			iter++;
	    }
	}

FILE*reg;
FILE*ok;
if(forward==0)
{
 	reg=fopen("registry_back.dat","a");
	fprintf(reg,"%.2f %.3f %.2f %.2f\n", urey, B, t_Ga, T-273.15);
	if ((t_Ga>4.60) && (T>T0) && (T>(T+dT)))
	{
		ok=fopen("ok_back.dat","a");
		fprintf(ok,"%.3f %.3f %.3f %.3f %.3f %.2f\n", urey, B, Ht*H0/Qt, Qt/(1e12), Q0, T-273.15);
	}
}
if(forward==1)
{
	reg=fopen("registry_fwd.dat","a");
	fprintf(reg,"%.2f %.3f %.2f %.2f\n", urey, B, t_Ga, T-273.15);
	if ((t_Ga>4.60) && (T<T0) && (T>(T-dT)))
	{
		ok=fopen("ok_fwd.dat","a");
		fprintf(ok,"%.3f %.3f %.3f %.3f %.3f %.2f\n", urey, B, Ht*H0/Qt, Qt/(1e12), Q0, T-273.15);
	}
}
fprintf(stdout,"Done with Forward=%d\tUrey=%.3f\tBeta=%.3f\n", forward, urey, B);
fclose(ky);
fclose(fp);
fclose(reg);
return 0;
}
