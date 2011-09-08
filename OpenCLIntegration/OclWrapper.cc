/*
 * (c) 2011 Wim Vanderbauwhede <wim.vanderbauwhede@gmail.com>
 *
 * */

#include <OclWrapper.h>
// ----------------------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------------------
OclWrapper::OclWrapper (bool use_gpu) : useGPU(use_gpu), nPlatforms(0) {

	    // First check the Platform
		cl::Platform::get(&platformList);
		checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
#ifdef VERBOSE
		std::cerr << "Number of platforms is: " << platformList.size() << std::endl;
#endif
		nPlatforms=platformList.size();
#ifdef PLATINFO
		for (unsigned int i=0;i<platformList.size();i++) {
			platformInfo.show(platformList,i);
		}
#endif

		selectDevice();
    }

OclWrapper::OclWrapper (bool use_gpu, int devIdx) : useGPU(use_gpu), nPlatforms(0) {

	    // First check the Platform
		cl::Platform::get(&platformList);
		checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
#ifdef VERBOSE
		std::cerr << "Number of platforms is: " << platformList.size() << std::endl;
#endif
		nPlatforms=platformList.size();
#ifdef PLATINFO
		for (unsigned int i=0;i<platformList.size();i++) {
			platformInfo.show(platformList,i);
		}
#endif

		selectDevice(devIdx);
    }

OclWrapper::OclWrapper () : nPlatforms(0) {
	    // First check the Platform
		cl::Platform::get(&platformList);
		checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
#ifdef VERBOSE
		std::cerr << "Number of platforms is: " << platformList.size() << std::endl;
#endif
		nPlatforms=platformList.size();
#ifdef PLATINFO
		for (unsigned int i=0;i<platformList.size();i++) {
			std::cout << "Platform["<< i << "]\n";
			platformInfo.show(platformList,i);
		}
#endif

   }
// ----------------------------------------------------------------------------------------
// Other public methods
// ----------------------------------------------------------------------------------------
bool OclWrapper::hasGPU(int i) {

	const cl::Platform& platform=platformList[i];
//	cl::vector<cl::Device> gpus;
	cl_int err= platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	return (err!=CL_DEVICE_NOT_FOUND);
}

bool OclWrapper::hasCPU(int i) {
	const cl::Platform& platform=platformList[i];
	cl_int err= platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
	return (err!=CL_DEVICE_NOT_FOUND);
}

int OclWrapper::nDevices(int pIdx, std::string devt) {
	const cl::Platform& platform=platformList[pIdx];
	if (devt=="GPU") {
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	} else if (devt=="CPU") {
		platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
	} else {
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	}
	int nDevs=devices.size();
	std::cout << "Number of "<<devt<<" devices for platform "<<pIdx<<": "<< nDevs << "\n";
#ifdef DEVINFO
	for (int dIdx=0;dIdx<nDevs;dIdx++) {
		std::cout << devt<<" Device["<< dIdx << "]: ";
		deviceInfo.show(devices[dIdx]);
	}
#endif
	return nDevs;
}
void OclWrapper::selectGPU() {
	useGPU=true;
	selectDevice();
}

void OclWrapper::selectCPU() {
	useGPU=false;
	selectDevice();
}

void OclWrapper::selectDevice(int pIdx, int dIdx, bool gpu) {
    // Use the platform info as input for the Context
	platformIdx=pIdx;
	deviceIdx=dIdx;
	useGPU=gpu;
	getContextAndDevices();
#ifdef DEVINFO
    deviceInfo.show(devices[deviceIdx]);
#endif
}

// Here we set the attributes platformIdx and deviceIdx
void OclWrapper::selectDevice() {

	// So we must first select the platform that has a GPU, then the device that is a GPU
	bool useCPU = not useGPU;
	platformIdx=0;
	deviceIdx=0;

	for (unsigned int i=0; i<platformList.size();i++) {

		if ((useGPU && hasGPU(i)) || (useCPU && hasCPU(i))) {
			platformIdx=i;
#ifdef DEVINFO
			std::cout << "Found platform "<<platformIdx<< " for ";
			if (useGPU) {
				std::cout << "GPU" <<"\n";
			} else {
				std::cout << "CPU" << "\n";
			}
#endif
			break;
		}
	}

	getContextAndDevices();

	for (unsigned int i=0;i<devices.size();i++) {
		if ( (useGPU && deviceInfo.isGPU(devices[i])) || (useCPU && deviceInfo.isCPU(devices[i])) ) {
			deviceIdx=i;
			break;
		}
	}

#ifdef DEVINFO
	deviceInfo.show(devices[deviceIdx]);
#endif
}

// Here we set the attributes platformIdx and deviceIdx
void OclWrapper::selectDevice(int devIdx) {

	// So we must first select the platform that has a GPU, then the device that is a GPU
	bool useCPU = not useGPU;
	platformIdx=0;
    deviceIdx=0;

	for (unsigned int i=0; i<platformList.size();i++) {

		if ((useGPU && hasGPU(i)) || (useCPU && hasCPU(i))) {
			platformIdx=i;
			break;
		}
	}
	if (devIdx==-1) {
        //std::cout << "Automatic device selection: ";
		for (unsigned int i=0;i<devices.size();i++) {
			if ( (useGPU && deviceInfo.isGPU(devices[i])) || (useCPU && deviceInfo.isCPU(devices[i])) ) {
				deviceIdx=i;
#ifdef DEVINFO
				std::cout << "Found platform "<<platformIdx<< " for ";
				if (useGPU) {
					std::cout << "GPU " <<deviceIdx<<"\n";
				} else { 
					std::cout << "CPU " <<deviceIdx<< "\n";
				}
#endif
				break;
			}
		}
	} else {
        deviceIdx=devIdx;
    }
	getContextAndDevices();

#ifdef DEVINFO
	deviceInfo.show(devices[deviceIdx]);
#endif
}

void OclWrapper::buildProgram(const char* ksource, const char* opts) {
    std::ifstream file(ksource);
    checkErr(file.is_open() ? CL_SUCCESS:-1, ksource);

    std::string prog(
            std::istreambuf_iterator<char>(file),
            (std::istreambuf_iterator<char>())
            );
//    std::cout <<"Program from source\n";
    cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
//    std::cout <<"new Program\n";

    program_p = new cl::Program(*context_p, source);
//    program = *program_p;
//    std::cout <<"build Program\n";
    err = program_p->build(devices,opts);
    checkErr(file.is_open() ? CL_SUCCESS : -1, "Program::build()");
}

//void OclWrapper::buildProgram(const char* ksource) {
//    std::ifstream file(ksource);
//    checkErr(file.is_open() ? CL_SUCCESS:-1, ksource);
//
//    std::string prog(
//            std::istreambuf_iterator<char>(file),
//            (std::istreambuf_iterator<char>())
//            );
////    std::cout <<"Program from source\n";
//    cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
////    std::cout <<"new Program\n";
//
//    program_p = new cl::Program(*context_p, source);
////    program = *program_p;
////    std::cout <<"build Program\n";
//    err = program_p->build(devices,"");
//    checkErr(file.is_open() ? CL_SUCCESS : -1, "Program::build()");
//}


void OclWrapper::loadBinary(const char* ksource) {
    std::ifstream file(ksource);
    checkErr(file.is_open() ? CL_SUCCESS:-1, ksource);

    std::string prog(
            std::istreambuf_iterator<char>(file),
            (std::istreambuf_iterator<char>())
            );

    cl::Program::Binaries binaries(1, std::make_pair(prog.c_str(), prog.length()+1));
    cl::vector<cl_int> binaryStatus;
    program_p = new cl::Program(*context_p, devices, binaries, &binaryStatus, &err);
    checkErr(file.is_open() ? CL_SUCCESS : -1, "Program::Program() from Binary");
}

void OclWrapper::storeBinary(const char* ksource) {
	cl::vector<char*> binaries;

	err = program_p->getInfo(CL_PROGRAM_BINARIES,&binaries);
	checkErr(err, "Program::getInfo(CL_PROGRAM_BINARIES)");
	// Now write this binary to a file
	std::ofstream file(ksource);
// ...
}

void OclWrapper::loadKernel(const char* ksource, const char* kname,const char* opts) {
//    std::cout << "buildProgram\n";
	buildProgram(ksource,opts);
//    std::cout << "new Kernel\n";
    kernel_p= new cl::Kernel(*program_p, kname, &err);
    checkErr(err, "Kernel::Kernel()");
}


void OclWrapper::loadKernel(const char* ksource, const char* kname) {
//    std::cout << "buildProgram\n";
	buildProgram(ksource,"");
//    std::cout << "new Kernel\n";
    kernel_p= new cl::Kernel(*program_p, kname, &err);
    checkErr(err, "Kernel::Kernel()");
}

void OclWrapper::loadKernel(const char* kname) {
    kernel_p= new cl::Kernel(*program_p, kname, &err);
    checkErr(err, "Kernel::Kernel()");
}

void OclWrapper::createQueue() {
    //std::cout << "Device: "<<deviceIdx<<"\n";
    // Create the CommandQueue
	queue_p = new cl::CommandQueue(*context_p, devices[deviceIdx], 0, &err);

    checkErr(err, "CommandQueue::CommandQueue()");
}

void OclWrapper::enqueueNDRange(const cl::NDRange& globalRange,const cl::NDRange& localRange) {
	// Create the CommandQueue
	queue_p = new cl::CommandQueue(*context_p, devices[deviceIdx], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");
	kernel_functor=kernel_p->bind(*queue_p,globalRange, localRange);
//	return kernel_functor;
//	return kernel_p->bind(*queue_p,globalRange, localRange);
}

cl::Buffer& OclWrapper::makeWriteBuffer(int bufSize) {
	 cl::Buffer* buf_p= new cl::Buffer(
	            *context_p,
	            CL_MEM_WRITE_ONLY,
	            bufSize,NULL,&err);
	 checkErr(err, "Buffer::Buffer()");
	 cl::Buffer& buf_r =*buf_p;
	return buf_r;
}

cl::Buffer& OclWrapper::makeReadBuffer(int bufSize,void* hostBuf, cl_mem_flags flags) {
	 cl::Buffer* buf_p= new cl::Buffer(
	            *context_p,
	            flags,
	            bufSize,hostBuf,&err);
	 checkErr(err, "Buffer::Buffer()");
	 cl::Buffer& buf_r =*buf_p;
	return buf_r;
}

void OclWrapper::readBuffer(const cl::Buffer& deviceBuf, int bufSize, void* hostBuf) {

	err = queue_p->enqueueReadBuffer(
	            deviceBuf,
	            CL_TRUE,
	            0,
	            bufSize,
	            hostBuf);
    checkErr(err, "CommandQueue::enqueueReadBuffer()");

}

void OclWrapper::readBuffer(const cl::Buffer& deviceBuf, bool blocking_read,
		::size_t offset, ::size_t bufSize, void * hostBuf,
		const VECTOR_CLASS<cl::Event> * events,
		cl::Event * event) {
			err = queue_p->enqueueReadBuffer(
					deviceBuf,
					blocking_read,
					offset,
					bufSize,
					hostBuf,
					events,
					event);

			checkErr(err, "CommandQueue::enqueueReadBuffer()");

		}
void OclWrapper::writeBuffer(const cl::Buffer& deviceBuf, int bufSize, void* hostBuf) {

	err = queue_p->enqueueWriteBuffer(
	            deviceBuf,
	            CL_TRUE,
	            0,
	            bufSize,
	            hostBuf);
    checkErr(err, "CommandQueue::enqueueWriteBuffer()");

}

void OclWrapper::writeBuffer(const cl::Buffer& deviceBuf, bool blocking_write,
		::size_t offset, ::size_t bufSize, void * hostBuf,
		const VECTOR_CLASS<cl::Event> * events,
		cl::Event * event) {
			err = queue_p->enqueueWriteBuffer(
					deviceBuf,
					blocking_write,
					offset,
					bufSize,
					hostBuf,
					events,
					event);

			checkErr(err, "CommandQueue::enqueueWriteBuffer()");

		}

void OclWrapper::doNothing() {}
// ----------------------------------------------------------------------------------------
// Private methods
// ----------------------------------------------------------------------------------------
void OclWrapper::getContextAndDevices() {

	const cl::Platform& platform = platformList[platformIdx];
	cl_platform_id platformId = platform();
    cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM,
    		(cl_context_properties)platformId, 0 };

	if (useGPU) {
		if (hasGPU(platformIdx) ) {
#ifdef VERBOSE
			std::cerr << "\nUsing GPU\n";
#endif
			context_p = new cl::Context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err); // GPU-only
			checkErr(err, "Context::Context()");
		} else  {
			checkErr(CL_FALSE, "Platform has no GPU");
		}
	} else	{
		if (hasCPU(platformIdx) ) {
#ifdef VERBOSE
			std::cerr << "\nUsing CPU\n";
#endif
			context_p = new cl::Context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err); // CPU-only
			checkErr(err, "Context::Context()");
		} else {
			checkErr(CL_FALSE, "Platform has no CPU");
		}
	}

//	getDevices();
}

// Unused, devices result from the getDevices call in hasGPU/hasCPU
void OclWrapper::getDevices() {
	devices = context_p->getInfo<CL_CONTEXT_DEVICES>();
	checkErr( devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");
}
// ----------------------------------------------------------------------------------------
// Functions, not part of the class
// ----------------------------------------------------------------------------------------
void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit( EXIT_FAILURE);
	}
}


