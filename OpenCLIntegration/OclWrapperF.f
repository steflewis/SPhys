! This is a very thin wrapper library for OpenCL
! I had hoped we could just pass the array but we need to pass its size as well. 
! Thanks to Fortran's shape() feature we can pass that instead so we can flatten anything 
      subroutine realArrayToOcl(ocl,pos,sz,array)
          integer(8) :: ocl
          integer :: pos, sz1d, cpos
          integer, dimension(3):: sz
          real,dimension(sz(1),sz(2),sz(3)) :: array
          real, dimension(size(array)):: array1d 
! The idea is that we:
!  - take the array, 
!  - find out its shape and size, 
!  - transform it into a 1-D array and 
!  - send it to C++
          array1d = reshape(array,shape(array1d))
          sz1d = size(array1d)*4 ! x4 because real is 4 bytes
          cpos = pos-1
          floatArrayToOcl(ocl,cpos,sz1d,array1d) 
      end
