# Environment variables for OpenCL Integration

# modify as required
export NVSDKCUDA_ROOT="/Developer/GPU\ Computing/"
export AMDAPPSDKROOT=/usr/local/AMD-APP-SDK-v2.4-lnx64
export INTELOCLSDKROOT=/usr
export OPENCL_DIR= $HOME/OpenCL # dir containing OclWrapper!

export OPENCL_GPU=`$OPENCL_DIR/bin/test_gpu.pl`
export OPENCL_CPU=`$OPENCL_DIR/bin/test_cpu.pl`

export PYTHONPATH=$PYTHONPATH:$OPENCL_DIR
export PATH=$OPENCL_DIR/bin:$PATH
