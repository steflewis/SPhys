root -b --no-splash


.x DoAll.C(1000,"gen.cfg",1)

.q

if (datatest.txt)
    rm datatest.txt

root para.C
.q

root perp.C
.q

scons

usercode

root BAnalyse.C

.q