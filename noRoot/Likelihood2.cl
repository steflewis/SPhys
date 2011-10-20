/* Kernel code for Nested Sampling Likelihood function */
/*
Inputs:
// Sampled
Bs: polarization observable, typ. 3000 elts, [-1,1] // could be done in a separate kernel
Pgs: beam polarization, typ. 3000 elts, around 0.7-0.8
// Measured
As: angles, typ. 10000, [0,2*PI]
Pms: polarization state, typ. 10000, [-1,1]

Output:
LogLs: log-likelihood, typ. 3000, usually -7000 log(L) with L in [0,1], so L= exp(LogL)
Consts:
Log2e: natural log constant

// Work items and work groups:

- 1 work group for each B
- N threads (e.g. 32 or 64), each calculates for 10000/N angles
=> total number of work items = 3000*N , access via get_global_id()
=> total number of groups = 3000, access via get_group_id()
=> total number of local work items ("threads") = N,  access via get_local_id()

*/
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define NTH 1
__kernel void Likelihood2(__global float* B, __global float* Pg, __global float* angles, __global float* pols, __global float* LogL, const float Log2e, const int Asize) {

	int g_id=get_group_id(0);
	/* double B=Bs[gid]; */
	/*double Pg=Pgs[ogid];*/
	
	 /*double angle        = get_global_id(0);*/
	 /*double polarisation = get_global_id(1);*/
	 int l_id = get_local_id(0);
	 /*float  angle        = angles[l_id];*/
	 /*float  pol          = pols[l_id];*/
	 float  delta_L      = 0.0;
	 //LogL[0]                = 0.0;
	 float  costerm      = 0.0;
	 
	 float  A_tilde;
	 float  LogL_2       = 0.0;
	 /*int    Asize        = 10075;  //Size of event arrays (angles and pols) */
	 int    M            = Asize / NTH; /* FIXME: deal with remainder! */
	 int    remainder    = Asize - (M*NTH); 
	 float  probsum      = 0.0;
	 local  float chunkSums[NTH];
	 //float  Total        = 0.0;
	int start = l_id*M;
	int idx;
	int stop = (l_id != NTH-1) ?  (l_id+1)*M  : Asize  ;
	 for (idx = start; idx<stop; idx++) {
	    float angle = angles[idx];
	    float pol   = pols[idx];
	    float  prob         = 0.0;
	    costerm = Pg[0]*B[0]*cos(2*angle);
	 
	    A_tilde = (costerm + delta_L) / (1 + (costerm*delta_L));
	    //A_tilde = angle*Pg[0];

	    if (pol < 0){
	      prob = 0.5*(1 + A_tilde); 
	    }

	    else if (pol >= 0){
	      prob = 0.5*(1 - A_tilde);
	    }

	    probsum += log2(prob);

	 }
	 chunkSums[l_id] = probsum;
/* How do these terms get added up? */
	  /* Barrier function goes here. */
	  barrier(CLK_LOCAL_MEM_FENCE);


	  /* Deal with remainder here? */
	  /* Not really sure what to do with it... Another for loop ranging from the last 'chunk'?  */
	  /* Does the array have to have size (NTH+1) to allow for this, or does it get tacked on to the last one? */

	  for (int i = 0; i < NTH; i++){
	      LogL_2 += chunkSums[i];
	  }

	 

/* Should I convert back to natural Log here? */

	 LogL[0] = LogL_2 / Log2e;

}
