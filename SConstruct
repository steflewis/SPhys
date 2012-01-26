# For help on optionbs, scons -h
from init_env import init_environment
import OclBuilder 
from OclBuilder import initOcl
from OclBuilder import getOpt
import os

OclBuilder.opts=Variables()   
impl=getOpt('impl','Implementation','GPU')
print "Selected implementation: "+impl

env = init_environment("root")


LFLAGS=[]
IMPLFLAGS=[]
if impl=='CPP_CPU':
	LFLAGS= [ '-fopenmp' ]
	IMPLFLAGS=['-DCPP_CPU','-DWITHOMP']
else:
	# Initialise OpenCL-specific env values
	env = initOcl(env)

env.Append(CXXFLAGS = ['-Wall','-Wno-deprecated','-O3']+IMPLFLAGS, LINKFLAGS=LFLAGS)

# Macro for path to data
cwd=os.environ['PWD']
datapath=cwd+'/text_files/datatest.txt'
datapathmacro='DATAPATH="\\"'+datapath+'\\""'
env.Append(CXXFLAGS = '-D'+datapathmacro)

# GPU-accelerated or reference implementation?
if impl=='CPP_CPU':
  env.Program('usercode', ['src/TUserCode.cc',
		     'src/TNestedSample.cc', 
		     'src/TSimplePhysics.cc','src/TSimplePhysics_CPU.cc']) #,'src/TPlotter.cc'])  
else:
  env.Program('usercode', ['src/TUserCode.cc',
		     'src/TNestedSample.cc', 
		     'src/TSimplePhysics.cc','src/TSimplePhysics_GPU.cc']) #,'src/TPlotter.cc'])  
  

