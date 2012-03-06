import OclBuilder 
from OclBuilder import initOcl
from OclBuilder import getOpt
import os
#from optparse import OptionParser

OclBuilder.opts=Variables()   

impl=getOpt('impl','Implementation','GPU')

env = Environment( CC = 'g++', CXXFLAGS = ['-Wall','-O3'])
# Initialise OpenCL-specific env values
env = initOcl(env)

env.Append(CXXFLAGS = ['-Wall','-Wno-deprecated','-O3'])

env.Library('OclWrapperF', ['./OclWrapperF.cc'])
