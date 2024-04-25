#set format xy "%3.0e"
#set size 1.0,1.0
set clip one
#
set nolabel
set logscale x
set logscale y
!cop -b2 3 gfitn468.gph gfitn468.gp_ c1 1/c2 1/c3
!cop 3 gnu0.dat gnu0.da_ c1 1/c2 1/c3
plot [1.000000e+01:1.000000e+08] [1.000000e-02:1.000000e+02] "gnu0.da_" using "%f%f" with points,"gfitn468.gp_" using "%f%f" with lines 2,"gfitn468.gp_" using "%f%*f%f" with lines 1
pause -1
#
