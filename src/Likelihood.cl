/* Kernel code for Nested Sampling Likelihood function */

__kernel void Fractal(__global double* B, __global double* Pg, __global double* LogL, const double Log2e) {

	 double angle        = get_global_id(0);
	 double polarisation = get_global_id(1);

	 double delta_L      = 0.0;
	 double LogL         = 0.0;
	 double costerm      = 0.0;
	 double prob         = 0.0;
	 double A_tilde;
	 double LogL_2       = 0.0;
	 
	 costerm = Pg * B * cos(2*angle);
	 
	 A_tilde = (costerm + delta_L) / (1 + (costerm*delta_L));

	 if (polarisation < 0){
	    prob = 0.5*(1 + A_tilde); 
	 }

	 else if (polarisation >= 0){
	    prob = 0.5*(1 - A_tilde);
	 }


/* How do these terms get added up? */


	 LogL_2 += log2(prob);

/* Should I convert back to natural Log here? */

	 LogL = LogL_2 / Log2e;

}