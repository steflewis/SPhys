import os
import glob
import sys

def recursiveDirs(root) :
	return filter( (lambda a : a.rfind( "CVS")==-1 ),  [ a[0] for a in os.walk(root)]  )

def unique(list) :
	return dict.fromkeys(list).keys()

def scanFiles(dir, accept=["include"], reject=[]) :
	sources = []
	paths = recursiveDirs(dir)
	for path in paths :
		for pattern in accept :
			sources+=glob.glob(path+"/"+pattern)
	for pattern in reject :
		sources = filter( (lambda a : a.rfind(pattern)==-1 ),  sources )
	return unique(sources)

def subdirsContaining(root, patterns):
	dirs = unique(map(os.path.dirname, scanFiles(root, patterns)))
	dirs.sort()
	return dirs

def print_cmd_line(s, target, src, env):
    """s is the original command line, target and src are lists of target
    and source nodes respectively, and env is the environment."""
    sys.stdout.write("Compiling %s...\n"%\
         (' and '.join([str(x) for x in target])))

def loadoptions(env) :
	# Adding fPIC to shared library linker
	# env.Append(SHLINKFLAGS = ['-fPIC'])
	# env.Append(SHCXXFLAGS = ['-fPIC'])


	# EHsc and MD only for win32 envs
	if  env['PLATFORM'] == 'win32':
		# 
		env.Append(CXXFLAGS='-Ox')     
		env.Append(CXXFLAGS='-MD')     # for multithread, dynamic link. Causes compiler to place msvcrt in object file
		env.Append(CXXFLAGS='-GR')     
		env.Append(CXXFLAGS='-EHsc')     
		env.Append(CXXFLAGS='-Zm200')     
		env.Append(CXXFLAGS='-nologo')     
		env.Append(CXXFLAGS='-D_CONSOLE')     
		env.Append(CXXFLAGS='-D_WIN32')     
		env.Append(CXXFLAGS='-DOS')     
		env.Append(CXXFLAGS='-DG4OPTIMISE')     
		env.Append(CXXFLAGS='-DWIN32')     
		env.Append(CXXFLAGS='-DXPNET')     
		env.Append(CXXFLAGS='-D_CRT_SECURE_NO_DEPRECATE')     
		env.Append(CXXFLAGS='-DG4_STORE_TRAJECTORY')     
	
		env.Append(LINKFLAGS='-FORCE')
		env.Append(LINKFLAGS='/NODEFAULTLIB:MSVCRT.dll')     

		env.Append(ENV = {'PATH': os.environ['PATH']})
		env.Append(ENV = {'INCLUDE': os.environ['INCLUDE']})
		env.AppendUnique(LIBPATH = [os.environ['MSLIBS']])
		env.AppendUnique(LIBPATH = [os.environ['SDKLIBS']])

			
	if env['SHOWBUILD'] != "1":
		env['PRINT_CMD_LINE_FUNC'] = print_cmd_line
		env['CXXCOM']   = "${TEMPFILE('%s')}" % env['CXXCOM'] 
	
	if env['SHOWENV'] == "1":
		print "  "
		print "Compiler: ", env['CC']
		print "Linker: ",   env.subst('$LINK')
		print "Platform: ", env['PLATFORM']
		print "  "
	
	if env['OPT'] == "1":
		if env['PLATFORM'] == 'posix':
			env.Append(CXXFLAGS = '-O2')
			print "Compiling with -O2 optimization."
		if env['PLATFORM'] == 'darwin':
			env.Append(CXXFLAGS = '-O2')
			print "Compiling with -O2 optimization."
		if env['PLATFORM'] == 'win32':
			env.Append(CXXFLAGS = '/O2 /Gs')
			print "Compiling with /O2 /Gs optimization."
	
	if env['DEBUG'] == "1":
		if env['PLATFORM'] == 'posix':
			env.Append(CXXFLAGS = '-g')
			print "Compiling with -g debug."
		if env['PLATFORM'] == 'darwin':
			env.Append(CXXFLAGS = '-g')
			print "Compiling with -g debug."
		if env['PLATFORM'] == 'win32':
			env.Append(CXXFLAGS = '/DEBUG')	
			print "Compiling with -/DEBUG debug."
	
	if env['ARCHI'] == 'x86':
		env.Append(CXXFLAGS  = '-m32')
		env.Append(CPPFLAGS  = '-m32')     
		env.Append(LINKFLAGS = '-m32')     
		

	if env['PROFILE'] == "1":
		if env['PLATFORM'] == 'posix':
			env.Append(CXXFLAGS = '-pg')
			env.Append(LINKFLAGS = '-pg')
			print "Compiling with -pg profiling."
	
	if env['PLATFORM'] == 'posix':
		env.Append(CXXFLAGS = '-fexceptions')
		env.Append(CXXFLAGS = '-fstack-protector')
		env.Append(CXXFLAGS = '-Wall')
		
	if env['PLATFORM'] == 'darwin':
		env.Append(CXXFLAGS = '-fexceptions')
		env.Append(CXXFLAGS = '-fstack-protector')
		env.Append(CXXFLAGS = '-Wall')



def cmloptions(opts) :
	opts.Add('ARCHI',     'Set to x86 to force 32 bit environment', 0)
	opts.Add('SHOWENV',   'Set to 1 to show environment used', 0)
	opts.Add('SHOWBUILD', 'Set to 1 to show build commands executed', 0)
	opts.Add('OPT',       'Set to 1 to optimize code', 0)
	opts.Add('DEBUG',     'Set to 1 to compile in debug mode', 0)
	opts.Add('PROFILE',   'Set to 1 to compile in profiling mode', 0)
	opts.Add('CUDA_EMU',  'Set to 1 to compile in video-card emulation mode', 0)
	
