! Learning: you can't mix combined declare/assign statements and write statements!
! set expandtab to get rid of ugly tabs!
! Don't know how to get free source format in gfortran
      program testOclWrapper
        implicit none
        real :: array1d(42)
        real::array(6,7) 
        integer i,j,k
        integer sz(3)
        integer ocl, pos
        ocl=0
        pos=1 ! using Fortran conventions here
        array1d = (/ (k*1.0, k=1,42) /)
        write(*,*) array1d
!        array = reshape( (/ (i*1.0, i=1,42) /) ,shape(array))
        array = reshape( array1d ,shape(array))
        sz(1:2)=shape(array)
        sz(3)=0
        do j=1,7          
          do i=1,6
            write (*,*) array(i,j)
          end do
          write (*,*) ""
        end do
        write(*,*) array
        call realArrayToOcl(ocl,pos,sz,array)
      end        
