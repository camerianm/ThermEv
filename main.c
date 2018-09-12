// SFD THERMEV CODE

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main(void)
{

	double dt_Ga=0.01;		//timestep in Ga
	double gamma0=0.72;		//urey ratio, [H(t) / Q(t)]
	double B=0.30;			//temp dependence of viscosity; may be variable
	double E_kJmol=300.0;	//activation energy, kJ/mol; isoviscous=0, olivine=525
							//300 per Korenaga ; 525 per McNamara
	double Q0_TW=36.0;		//present day heat flux, TW (TJ/s)
	double T0_C=1350.0;		//mantle potential temp OR average temp
	double mass=1.00;		//planet mass (in earth masses); guides heat capacity
	double radius=1.00;		//planet radius (in earth radii) - no role yet except grav.
	int    forward=0;		//0 places t=0 at present, evolves back in time;
							//assumes Q0, H0, and T0 are present-day values.
							//1 places t=0 at formation, evolves forward.
							//note: possibly valid for stagnant-lid regimes;
							//future updates may represent other regimes
	double C_JK=7.0e27;		//heat capacity of whole earth in J/K
	double t_Ga=0.0;			//start time in Ga
	double t=t_Ga*3.1536e16;	//converts start time to seconds
	double dt=dt_Ga*3.1536e16; 	//converts dt to seconds
	double T0=T0_C+273;			//converts temp to K
	double Q0=Q0_TW*1e12;		//converts flux to J/s
	double H0=Q0*gamma0;		//initial heat production in J/s
	double E=E_kJmol*1e3;		//converts activation energy to J/mol
	double R=8.3145; 			//ideal gas constant in J/mol*K

	double G=6.67408e-11, Re=6.371e6, Me=5.9722e24;
   	double grav=G*(Me*mass)/(pow((Re*radius),2));	//calculates surface gravity
    double Cp=C_JK*mass;		//assumes heat capacity roughly scales with mass
 
	double h_n[4]={0.372, 0.0164, 0.430, 0.181}; // fraction of total heat production at t=0
    double L_n[4]={0.155, 0.985, 0.0495, 0.555}; // lambda - decay/growth constant in 1/Gyr
    double Qt, Ht, H_n1, H_n2, H_n3, H_n4 ; // total production; multiplier for each species

	FILE*TimeTemp; TimeTemp=fopen("TimeTemp.txt", "w");
	fprintf(TimeTemp, "%.2f, %.2f\n", t_Ga, T0_C);	//present time + temp in C
	int iter=0;		//iteration counter
	t_Ga=t_Ga+dt_Ga;				//setting time in Ga
	double dT=((H0 - Q0)/C_JK)*dt;	//temp chg

	if (forward==0)
	{
		double T=T0-dT; t=t+dt;	// applying present temp chg linearly back, 1st timestep in S
		fprintf(TimeTemp, "%.2f, %.2f\n", t_Ga, T-273);	//registers first timestep back
		while (t_Ga<4.6)
		{
			iter=iter+1;
			H_n1 = h_n[0]*exp(L_n[0]*t_Ga); // 238U	
	        H_n2 = h_n[1]*exp(L_n[1]*t_Ga); // 235U
	        H_n3 = h_n[2]*exp(L_n[2]*t_Ga); // 232Th
	        H_n4 = h_n[3]*exp(L_n[3]*t_Ga); // 40K
	        Ht = H0 * (H_n1 + H_n2 + H_n3 + H_n4);		//calculates H term
	        Qt = Q0*pow((T/T0), (1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);
	        dT = ((Ht - Qt)/C_JK)*dt; 	//finding temp difference
	        T = T - dT;					//applying temp difference
	        t_Ga=t_Ga+dt_Ga;	//note: decay rate at time t_Ga is one timestep off.
	        fprintf(TimeTemp, "%.2f, %.2f\n", t_Ga, T-273); 
		}
	}
	else
	{
		double T=T0+dT; t=t+dt;	// applying present temp chg linearly forward, 1st timestep in S
		fprintf(TimeTemp, "%.2f, %.2f\n", t_Ga, T-273);	//registers first timestep fwd
		while (t_Ga<4.6)
		{
			iter=iter+1;
			H_n1 = h_n[0]*exp(-1*L_n[0]*t_Ga); // 238U	
	        H_n2 = h_n[1]*exp(-1*L_n[1]*t_Ga); // 235U
	        H_n3 = h_n[2]*exp(-1*L_n[2]*t_Ga); // 232Th
	        H_n4 = h_n[3]*exp(-1*L_n[3]*t_Ga); // 40K
	        Ht = H0 * (H_n1 + H_n2 + H_n3 + H_n4);		//calculates H term
	        Qt = Q0*pow((T/T0), (1+B))*pow((exp((E/R)*((1/T0)-(1/T)))), B);
	        dT = ((Ht - Qt)/C_JK)*dt; 	//finding temp difference
	        T = T + dT;					//applying temp difference
	        t_Ga=t_Ga+dt_Ga;	//note: decay rate at time t_Ga is one timestep off.
	        fprintf(TimeTemp, "%.2f, %.2f\n", t_Ga, T-273); 
	    }
	}
	fprintf(stdout, "%d iterations displayed in TimeTemp.txt.\n", iter+1);
return 0;
}
