#g++ `root-config --libs` `root-config --cflags` -lMinuit -Wl,-rpath=`root-config --libdir` -o /home/gordonh/analysis_code/rootmacros/bsa/kinematics/tc/bsa_c0s1s2_vstc_uml.exe /home/gordonh/analysis_code/rootmacros/bsa/kinematics/tc/bsa_c0s1s2_vstc_uml.c
#g++ `root-config --libs` `root-config --cflags` -lMinuit -Wall -o fit_macro.exe fit_macro.c

g++ `root-config --libs` `root-config --cflags` -lMinuit -Wl,-rpath=`root-config --libdir` -g -o $1 $2
