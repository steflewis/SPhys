
#include "OclWrapperF.h"

//void* unpack(OclWrapper* ocl) {
//	void* pt = reinterpret_cast<void*>(ocl);
//	return pt;
//}
//
//OclWrapper* pack(void* res) {
//	OclWrapper* ocl=(OclWrapper*)res;
//	return ocl;
//}

//int64_t toWord(OclWrapper* ocl) {
//	void* vp=unpack(ocl);
//	int64_t ivp =(int64_t)vp;
//	return ivp;
//}
//
//OclWrapper* fromWord(int64_t ivp) {
//	void* vp=(void*)ivp;
//	OclWrapper* ocl=pack(vp);
//	return ocl;
//}


extern "C" {


void oclinit_(OclWrapperF ocl_ivp,const char* source,const char* kernel) {
//	std::cout <<"init\n";
 	bool use_gpu=true;
#ifdef CPU
	use_gpu=false;
#endif
//	std::cout <<"<"<<kernel<<">\n";
	std::string kstr(kernel);
	kstr.erase(std::remove_if(kstr.begin(), kstr.end(), isspace), kstr.end());
	kernel=kstr.c_str();
//	std::cout <<"<"<<source<<">\n";
	std::string sstr(source);
	sstr.erase(std::remove_if(sstr.begin(), sstr.end(), isspace), sstr.end());
//	sstr="matacc.cl"; FIXME!
	source=sstr.c_str();

	OclWrapper* ocl = new OclWrapper(use_gpu,source,kernel);
//	std::cout <<"cast\n";
	*ocl_ivp=toWord<OclWrapper*>(ocl);
//	std::cout <<ocl_ivp<<"\n";
//	std::cout <<(*ocl_ivp)<<"\n";
}

void oclgetmaxcomputeunits_(OclWrapperF ocl_ivp,int* nunits) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	*nunits = ocl->getMaxComputeUnits();
}
/*
void floatarraytoocl_(OclWrapperF ocl_ivp,int* argpos,int* sz,float* array) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);

	if (ocl->argStatus[*argpos]==0) {
		ocl->argStatus[*argpos]=1;
		// create the buffer; this method stores the buffer in OclWrapper::buf
		ocl->makeReadBufferPos(*argpos,*sz);
	}
	// write the array to the buffer; the buffer is accessed via argpos
	ocl->writeBufferPos(*argpos,*sz,(void*)array);
}

void floatconsttoocl_(OclWrapperF ocl_ivp,int* argpos,float* constarg) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	ocl->setArg(*argpos,*constarg);
}
// and similar for int, long and double
void floatarrayfromoclalloc_(OclWrapperF ocl_ivp,int* argpos,int* sz) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	if (ocl->argStatus[*argpos]==0) {
		ocl->argStatus[*argpos]=1;
		// create the buffer; this method stores the buffer in OclWrapper::buf
		ocl->makeWriteBufferPos(argpos,sz);
	}
}
void floatarraytooclalloc_(OclWrapperF ocl_ivp,int* argpos,int* sz) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	if (ocl->argStatus[*argpos]==0) {
		ocl->argStatus[*argpos]=1;
		// create the buffer; this method stores the buffer in OclWrapper::buf
		ocl->makeReadBufferPos(*argpos,*sz);
	}
}
void floatarrayfromocl_(OclWrapperF ocl_ivp,int* argpos,int* sz, float* array) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	// read the array from the buffer; the buffer is accessed via argpos
	ocl->readBufferPos(*argpos,*sz,(void*)array);

}
*/
//void oclenqueuendrange_(OclWrapperF ocl_ivp,int global , int local) {
//	OclWrapper* ocl = fromWord(ocl_ivp);
//};

void oclsetarrayarg_(OclWrapperF ocl_ivpa,int* pos, OclBufferF buf_ivpa) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivpa);
	cl::Buffer* buffer = fromWord<cl::Buffer*>(*buf_ivpa);
	ocl->setArg(*pos, *buffer);
};

void oclsetfloatarrayarg_(OclWrapperF ocl_ivpa,int* pos, OclBufferF buf_ivpa) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivpa);
	cl::Buffer* buffer = fromWord<cl::Buffer*>(*buf_ivpa);
	ocl->setArg(*pos, *buffer);
};
void oclsetfloatconstarg_(OclWrapperF ocl_ivpa,int* pos,  float* constarg) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivpa);
	ocl->setArg(*pos, *constarg);
};
void oclsetintconstarg_(OclWrapperF ocl_ivpa,int* pos,  int* constarg) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivpa);
	ocl->setArg(*pos, *constarg);
};
// This works only for a 1-D range. For a 2-D or 3-D range, we can pass arrays
// but how do we know their size? We can use a 4-elt array, the 1st elt is the size
void runocl_(OclWrapperF ocl_ivp,int* global , int* local) {
//	std::cout <<"unwrap\n";
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
//	std::cout <<"create ranges "<<(*global)<<","<<(*local)<<"\n";
	cl::NDRange* globalrange=new cl::NDRange(*global);
	cl::NDRange* localrange=new cl::NDRange(*local);
//	std::cout <<"ocl->enqueueNDRangeRun\n";
	ocl->enqueueNDRangeRun(*globalrange,*localrange);
};

/*
 //    oclmakereadbuffer_(&ocl_ivp,&mA_buf_ivp,sizeof(cl_float) * mSize);

	OclWrapper* ocl_p = fromWord(ocl_ivp);
	int err;
	 cl::Buffer* buf_p= new cl::Buffer(
	            *(ocl_p->context_p),
	            CL_MEM_READ_ONLY,
	            sizeof(cl_float) * mSize,NULL,&err);
	 checkErr(err, "makeReadBuffer()");
	void* buf_vp=static_cast<void*>(buf_p);
	int64_t* mA_buf_ip=(int64_t*)buf_vp;

 * */

void oclwritebuffer_(OclWrapperF ocl_ivp,OclBufferF buf_ivpa, int* sz,void* array) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
//	int64_t buf_ivp=*buf_ivpa;
//	int64_t* buf_ip=(int64_t*)buf_ivp;
//	void* buf_vp=(void*)buf_ivp;
//	cl::Buffer* buffer = (cl::Buffer*)buf_vp;
	cl::Buffer* buffer = fromWord<cl::Buffer*>(*buf_ivpa);
	ocl->writeBuffer(*buffer,*sz,array);
}

void oclreadbuffer_(OclWrapperF ocl_ivp,OclBufferF buf_ivpa, int* sz,void* array) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);

//	int64_t buf_ivp=*buf_ivpa;
//	int64_t* buf_ip=(int64_t*)buf_ivp;
//	void* buf_vp=(void*)buf_ivp;
//	cl::Buffer* buffer = (cl::Buffer*)buf_vp;
	cl::Buffer* buffer = fromWord<cl::Buffer*>(*buf_ivpa);
	ocl->readBuffer(*buffer,*sz,array);
}

void oclmakereadbuffer_(OclWrapperF ocl_ivp,OclBufferF buf_ivp, int* sz) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	int err;
	cl::Buffer* buf_p= new cl::Buffer(
	            *(ocl->context_p),
	            CL_MEM_READ_ONLY,
	            *sz,NULL,&err);
	checkErr(err, "makeReadBuffer()");
//	void* buf_vp=reinterpret_cast<void*>(buf_p);
//	int64_t* buf_ip=(int64_t*)buf_vp;
//	*buf_ivp=(int64_t)buf_ip;
	*buf_ivp=toWord<cl::Buffer*>(buf_p);
}

void oclmakewritebuffer_(OclWrapperF ocl_ivp,OclBufferF buf_ivp, int* sz) {
	OclWrapper* ocl = fromWord<OclWrapper*>(*ocl_ivp);
	int err;
	cl::Buffer* buf_p= new cl::Buffer(
	            *(ocl->context_p),
	            CL_MEM_WRITE_ONLY,
	            *sz,NULL,&err);
	checkErr(err, "makeReadBuffer()");
//	void* buf_vp=reinterpret_cast<void*>(buf_p);
//	int64_t* buf_ip=(int64_t*)buf_vp;
//	*buf_ivp=(int64_t)buf_ip;
	*buf_ivp=toWord<cl::Buffer*>(buf_p);
}


} // extern "C"
