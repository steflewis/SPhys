#
# SCons build script for building OpenCL applications using OclWrapper
#
# (c) 2011 Wim Vanderbauwhede <wim.vanderbauwhede@gmail.com>
#

import os
import re
import commands
import sys
import os.path
from SCons.Variables import Variables
from SCons.Environment import Environment

def getOpt(optname,desc,default):

    global opts
    opts.Add(optname,desc,default)
    optionl = filter (lambda x: x.key==optname,opts.options)
    if optionl:
        option=optionl[0]
        if opts.args.has_key(optname) and opts.args[optname]!=option.default:
            return opts.args[option.key]
        else: 
            return option.default
    else:
        print "No such option: "+optname
    

def build(appname,sources):

    global opts
    
    help = """
    Options:
    lib=0|1 [0] build an OclWrapper library
    plat=AMD|NVIDIA|Intel [NVIDIA]
    dev=CPU|GPU [GPU] device
    kernel=<number> [1]   
    w=<number> [1024]   matrix width
    ref=0|1|2 [1]     reference 2=ref only
    v=0|1 [1]         verbose
    mode=0|1|2   COPY|COPY+ALLOC|USE 
    info=0|1
    gpu=-1|0|1 [-1, means automatic selection]
    dbg=0|1 [0]
    nruns= [1]
    D=[comma-sep list of macros]
    """
    
    if commands.getoutput("uname") == "Darwin":
        OSX=1
        OSFLAG='-DOSX'
    else:
        OSX=0
        OSFLAG='-D__LINUX__'

    OPENCL_DIR=os.environ['OPENCL_DIR']
    opts=Variables()        

    args=sys.argv[1:]
    for arg in args:
        if re.match("(\w+)=(\w+)",arg):
            (k,v)=arg.split('=')
            opts.args[k]=v
    buildLibrary=getOpt('lib','buildLibrary',0)
    dev=getOpt('dev','Device','GPU')
    plat=getOpt('plat','Platform','NVIDIA')
    if plat=='AMD':      
        AMD_SDK_PATH=os.environ['AMDAPPSDKROOT']
    elif plat=='Intel':      
        INTEL_SDK_PATH=os.environ['INTELOCLSDKROOT']
    else:    
        NVIDIA_SDK_PATH=os.environ['NVSDKCUDA_ROOT']
        if os.environ['OPENCL_GPU']!='NVIDIA':
            print 'No NVIDIA platform, defaulting to AMD CPU'
            if os.environ['OPENCL_CPU']=='AMD':
                AMD_SDK_PATH=os.environ['AMDAPPSDKROOT']
                plat='AMD'
            elif os.environ['OPENCL_CPU']=='Intel':
		INTEL_SDK_PATH=os.environ['INTELOCLSDKROOT']
		plat='Intel'
	    else:
		print 'No OpenCL-capable GPU found, giving up'
		exit
            dev='CPU'
#        else:
#            print 'NVIDIA'
    
    gpu=getOpt('gpu','GPU','-1')
    devidxflag='-DDEVIDX='+gpu
    if gpu!='-1':
        dev='GPU'
        
    kernel=getOpt('kernel','Kernel','1')
    nruns=getOpt('nruns','Number of runs','1')
    width=getOpt('w','Width','1024')
    ref=getOpt('ref','Reference','1')
    refflag='-DREF'
    verbose=getOpt('v','Verbose','0')
    vflag='-DVERBOSE'
    if verbose=='0':
        vflag=''
    mode=getOpt('mode','Mode','1')
    memreadflag='-DMRMODE='+mode
    devinfo=getOpt('info','DeviceInfo','0')
    devinfoflag=''
    platinfoflag=''
    if devinfo=='1':
        devinfoflag='-DDEVINFO'
        platinfoflag='-DPLATINFO'
        
    defs=getOpt('D','Defines',None)
    defflags=[]
    if defs!=None:
        deflist=defs.split(',')
        defflags=map (lambda s: '-D'+s, deflist)   
    
    DEVFLAGS=['-D'+dev,devidxflag,'-DKERNEL='+kernel,'-DNRUNS='+nruns,'-DWIDTH='+width,'-DREF='+ref,vflag, memreadflag,devinfoflag,platinfoflag]+defflags
    if commands.getoutput("uname") == "Darwin":
        DEVFLAGS+=['-DOSX']    
        
    oclsources=map (lambda s: OPENCL_DIR+'/'+s, ['Timing.cc','DeviceInfo.cc','PlatformInfo.cc','OclWrapper.cc'])
        
    env = Environment( CC = 'g++', CXXFLAGS = ['-Wall','-g',DEVFLAGS]) 
    env.Help(help)
    
    if OSX==1:
        env['FRAMEWORKS'] = ['OpenCL']
        env['CPPPATH']=[OPENCL_DIR]
    else:    
        env['LIBS']=['OpenCL']
        if plat=='AMD':
            env['CPPPATH']=[OPENCL_DIR,AMD_SDK_PATH+'/include','/usr/include/CL']
            env['LIBPATH']=[AMD_SDK_PATH+'/lib/x86_64']
        elif plat=='Intel':
            env['CPPPATH']=[OPENCL_DIR,INTEL_SDK_PATH+'/include/CL']
            env['LIBPATH']=[INTEL_SDK_PATH+'/lib64']
        else:
            env['CPPPATH']=[OPENCL_DIR,OPENCL_DIR+'/CL',NVIDIA_SDK_PATH+'/OpenCL/common/inc']
    if buildLibrary=='1':        
        env.Library('OclWrapper',oclsources)
    else:        
        env.Program(appname+'_'+dev+'_'+plat+'_'+kernel,oclsources+sources)
