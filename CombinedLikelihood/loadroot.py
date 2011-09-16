import os

def loadroot(env) :
	OSENV  = os.environ
	ROOTSYS = OSENV['ROOTSYS']
	if env['SHOWENV'] == "1":
		print "Loading ROOT software from ", ROOTSYS

	inc_path = []

	if env['PLATFORM'] == 'posix':
		inc_path = os.popen('$ROOTSYS/bin/root-config --incdir').readline()
	if env['PLATFORM'] == 'darwin':
		inc_path = os.popen('$ROOTSYS/bin/root-config --incdir').readline()
	# on Windows there is no script
	# Have to do it by hand
	if env['PLATFORM'] == 'win32':
		inc_path = ROOTSYS + "\include"

	env.Append(CXXFLAGS = env['INCPREFIX'] + inc_path)

	rootlibs = []
	root_config_libs = []
	if env['PLATFORM'] == 'posix':
		root_config_libs = os.popen('$ROOTSYS/bin/root-config --glibs').readline()[:-1].split()
	if env['PLATFORM'] == 'darwin':
		root_config_libs = os.popen('$ROOTSYS/bin/root-config --glibs').readline()[:-1].split()
	# on Windows there is no script
	# Have to do it by hand
	# Do a showenv on Linux/Darwin and copy here
	if env['PLATFORM'] == 'win32':
		rootlibs = ['libCore', 'libCint', 'libRIO', 'libNet', 'libHist', 'libGraf', 'libGraf3d', 'libGpad', 'libTree', 'libRint', 'libPostscript', 'libMatrix', 'libPhysics', 'libMathCore', 'libThread', 'libGui']

	for l in root_config_libs:
		if l[:2] == '-l':
			rootlibs += [l[2:]]


	# no GQt in Windows
	if env['PLATFORM'] == 'posix':
#		rootlibs += ['-lGQt']
#		env.Append(LIBPATH = [ROOTSYS + '/lib/root'])
		env.Append(LIBPATH = [ROOTSYS + '/lib'])
	if env['PLATFORM'] == 'darwin':
#		rootlibs += ['-lGQt']
#		env.Append(LIBPATH = [ROOTSYS + '/lib/root'])
		env.Append(LIBPATH = [ROOTSYS + '/lib'])
	if env['PLATFORM'] == 'win32':
		env.Append(LIBPATH = [ROOTSYS + '\lib'])

	if env['SHOWENV'] == "1":
		print "ROOT include path: ", inc_path
		print "ROOT Libraries: ", rootlibs


	env.Append(LIBS = rootlibs)

