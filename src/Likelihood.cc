/*****************************************************************
 *                                                               *
 *         Likelihood Function (Nested Sampling Program)         *
 *                                                               *
 *         A first attempt at parallelising the code.            *
 *         Author: Stefanie Lewis                                *
 *         Based on code provided by Wim Vanderbauwhede          *
 *         Date: 28/06/2011                                      *
 *                                                               *
 *         s.lewis.glasgow@gmail.com                             *
 *****************************************************************/
#include <iostream>
#include <fstream>

#define __NO_STD_VECTOR // Use cl::vector instead of STL version

#ifdef OSX
#include <cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <stdio.h>

int main () {
  
  cl_int err;

  // Create platform:
  cl::vector<cl::Platform> platformList;
  cl::Platform::get(&platformList);
  checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");

  // Create the Context
  cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM,
				      (cl_context_properties)(platformList[0])(), 0 };
  cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err); // GPU-only
  checkErr(err, "Context::Context()");

  // Find the Devices
  cl::vector<cl::Device> devices;
  devices = context.getInfo<CL_CONTEXT_DEVICES>();
  checkErr( devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

  // Load the Kernel
  std::ifstream file("Fractal.cl");
  checkErr(file.is_open() ? CL_SUCCESS:-1, "Fractal.cl");

  std::string prog(
		   std::istreambuf_iterator<char>(file),
		   (std::istreambuf_iterator<char>())
		   );

  cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));

  // Build the Kernel
  cl::Program program(context, source);
  err = program.build(devices,"");
  checkErr(file.is_open() ? CL_SUCCESS : -1, "Program::build()");

  cl::Kernel kernel(program, "Likelihood", &err);
  checkErr(err, "Kernel::Kernel()");
  

  // Variable declarations go here.
  unsigned int num = 3000;


  size_t array_size = sizeof(double)*num;
  // where num is the number of elements in the array, in this case 3000.


  // Need to create OpenCL arrays:
  cl::Buffer cl_B(context, CL_MEM_READ_ONLY, array_size, NULL, &err);
  cl::Buffer cl_Pg(context, CL_MEM_READ_ONLY, array_size, NULL, &err);
  cl::Buffer cl_LogL(context, CL_MEM_WRITE_ONLY, array_size, NULL, &err);

  // Set arguments of kernel:
  err = kernel.setArg(0, cl_B);
  err = kernel.setArg(1, cl_Pg);
  err = kernel.setArg(2, cl_LogL);

  // Constant arguments:
  err = kernel.setArg(3, Log2e);

  cl::Event event;

  // Create command queue:
  cl::CommandQueue queue(context, devices[0], 0, &err);
  checkErr(err, "CommandQueue::CommandQueue()");

  // Push CPU arrays to GPU
  err = queue.enqueueWriteBuffer(cl_B, CL_TRUE, array_size, B, NULL, &event);
  err = queue.enqueueWriteBuffer(cl_Pg, CL_TRUE, array_size, Pg, NULL, &event);

  // Execute kernel:
  err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(), cl::NullRange, NULL, &event);
  // I don't know what to put in the parentheses at cl::NDRange(....)

  checkErr(err, "EnqueueNDRangeKernel()");
  queue.finish();
  

  err = queue.enqueueReadBuffer(cl_LogL, CL_TRUE, 0, sizeof(double)*num, LogL, NULL, &event);

  checkErr(err, "clEnqueueReadBuffer()");

  // Exit
  return EXIT_SUCCESS;

}


inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit( EXIT_FAILURE);
	}
}
