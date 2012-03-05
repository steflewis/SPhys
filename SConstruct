# For help on optionbs, scons -h
from init_env import init_environment
import OclBuilder 
from OclBuilder import initOcl
from OclBuilder import getOpt
import os

OclBuilder.opts=Variables()   
impl=getOpt('impl','Implementation','GPU')
print "Selected implementation: "+impl

iters=getOpt('iters','#Iterates','10000')
data=getOpt('data','Event data file (in ./text_files)','datatest_10000.txt')


env = init_environment("root")


LFLAGS=[]
IMPLFLAGS=['-DITERATES='+iters]
if impl=='CPP_CPU':
    nth=getOpt('nth','#Threads','1')
    LFLAGS= [ '-fopenmp' ]
    IMPLFLAGS+=LFLAGS+['-DCPP_CPU','-DWITH_OMP','-DNTH='+nth]
else:
	# Initialise OpenCL-specific env values
	env = initOcl(env)

env.Append(CXXFLAGS = ['-Wall','-Wno-deprecated','-O3']+IMPLFLAGS, LINKFLAGS = LFLAGS )

# Macro for path to data
cwd=os.environ['PWD']
datapath=cwd+'/text_files/'+data
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
  

