/*Kernel code for Nested Sampling Likelihood function */
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
#define NTH 32

__kernel void Likelihood2(__global float* B_Pg, __global float* angles, __global float* pols, __global float* LogL, const float Log2e, const int Asize, const int nunits) {
	int g_id=get_group_id(0);

	float B=B_Pg[0];
	float Pg=B_Pg[1];

	int numgrps = get_num_groups(0);
	 int gl_id = get_global_id(0);
	 int l_id = get_local_id(0);
	 float  delta_L      = 0.0;
	 float  costerm      = 0.0;
	 
	 float  A_tilde;
	 float  LogL_2       = 0.0;
	 float  probsum      = 0.0;
	 local  float chunkSums[NTH];

	 int mSize = Asize/(nunits*NTH);
	 int start = gl_id*mSize;
	 int stop = (gl_id==(nunits*NTH-1))?Asize:(gl_id+1)*mSize;
	 for (int idx = start; idx<stop; idx++) {
	    float angle = angles[idx];
	    float pol   = pols[idx];
	    float  prob         = 0.0;
	    costerm = Pg*B*cos(2*angle);
	 
	    A_tilde = (costerm + delta_L) / (1 + (costerm*delta_L));

	    if (pol < 0){
	      prob = 0.5*(1 + A_tilde); 
	    }

	    else if (pol >= 0){
	      prob = 0.5*(1 - A_tilde);
	    }

	    probsum += log2(prob);

	 }
	 chunkSums[l_id] = probsum;

	  // Barrier function goes here. 
	  barrier(CLK_LOCAL_MEM_FENCE);


	  for (int i = 0; i < NTH; i++){
	      LogL_2 += chunkSums[i];
	  }
	
	 LogL[g_id] = LogL_2 / Log2e;

	 //LogL[g_id] = Log2e*(g_id==0)+Asize*(g_id==1)+nunits*(g_id==2)+(B+Pg)*(g_id==3);
}
