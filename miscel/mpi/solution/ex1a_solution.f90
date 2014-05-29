program pcolor 

implicit none

include "mpif.h"

integer :: nprocs,ierr,myid

! Initialize MPI
call mpi_init(ierr)

call mpi_comm_size(mpi_comm_world,nprocs,ierr)
call mpi_comm_rank(mpi_comm_world,myid,ierr)

if (mod(myid,2).eq.0) then
   write(*,'("Process",I3," has the color red.")') myid
else
   write(*,'("Process",I3," has the color green.")') myid
endif

call mpi_finalize(ierr)

stop
end program pcolor

