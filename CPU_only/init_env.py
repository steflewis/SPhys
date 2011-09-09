from util import cmloptions, loadoptions

from SCons.Environment import *
from SCons.Variables import *
from SCons.Script import *

def init_environment(reqlist) :
	opts = Variables()
	cmloptions(opts)

	# first check if there's qt4 in the requirement
	need_qt4 = 0
	list = reqlist.split()
	for l in list:
		if l == "qt4":
			env = Environment(tools=['default', 'qt4'], options = opts)
			need_qt4 = 1
			break

	# if not, build default
	if need_qt4 == 0:
		env = Environment(options = opts)

	# Now scanning the dependencies
	for l in list:
		if l == "qt4":
			from loadqt import loadqt
			loadqt(env)
		if l == "geant4":
			from loadgeant4 import loadgeant4
			loadgeant4(env)
		if l == "evio":
			from loadevio import loadevio
			loadevio(env)
		if l == "root":
			from loadroot import loadroot
			loadroot(env)
		if l == "clhep":
			from loadclhep import loadclhep
			loadclhep(env)
		if l == "mysql":
			from loadmysql import loadmysql
			loadmysql(env)
		if l == "clas":
			from loadclas import loadclas
			loadclas(env)
		if l == "jana":
			from loadjana import loadjana
			loadjana(env)
		if l == "clas12":
			from loadclas12 import loadclas12
			loadclas12(env)
		if l == "cmsg":
			from loadcmsg import loadcmsg
			loadcmsg(env)
		if l == "mu":
			from loadmu import loadmu
			loadmu(env)
		if l == "cuda":
			from loadcuda import loadcuda
			loadcuda(env)

	# generating help list		
	Help(opts.GenerateHelpText(env))
	# loading options
	loadoptions(env)
	return env
