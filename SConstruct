#verbose: scons SHOWBUILD=1 
from init_env import init_environment
import os
#from optparse import OptionParser

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

opts=Variables()   
dev=getOpt('dev','Device','GPU')

print "Device selected: "+dev

env = init_environment("root")

#Include Path
cwd=os.environ['PWD']
incpath = ['.',cwd+'/OpenCLIntegration','/home/Stefanie/GPU_Computing/OpenCL']
env.Append(CPPPATH = incpath)
ocl_libpath=cwd+'/OpenCLIntegration'
env.Append(LIBPATH=['/opt/lib/root',ocl_libpath])
env.Append(LIBS=['OpenCL','OclWrapper'])
#env.Append(CXXFLAGS = '-g3')
datapath='DATAPATH="\\"'+cwd+'/text_files/datatest.txt\\""'
env.Append(CXXFLAGS = ' -Wall -g -O0 -D'+datapath)

#AddOption('--GPU',)
#Is it possible to add some sort of option to select CPU or GPU when compiling?
env.SConscript('OpenCLIntegration/SConstruct')
if dev=='GPU':
  env.Program('usercode', ['src/TUserCode.cc',
		     'src/TNestedSample.cc', 
		     'src/TSimplePhysics.cc','src/TSimplePhysics_GPU.cc']) #,'src/TPlotter.cc'])  
else:
  env.Program('usercode', ['src/TUserCode.cc',
		     'src/TNestedSample.cc', 
		     'src/TSimplePhysics.cc','src/TSimplePhysics_CPU.cc','src/TPlotter.cc'])  
  
#env.Library('usercode', ['src/TUserCode.cc',
#		     'src/TNestedSample.cc', 
#		     'src/TSimplePhysics.cc','src/TSimplePhysics_GPU.cc','src/TPlotter.cc'])

