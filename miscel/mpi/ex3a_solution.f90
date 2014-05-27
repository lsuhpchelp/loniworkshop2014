program findmax

implicit none

include "mpif.h"

integer :: nprocs,ierr,myid
integer :: status(mpi_status_size)

integer :: i,seed(1),clock
integer :: a(10)
integer :: max_local,max_global
real*8 :: xrnd

! Initialize MPI
call mpi_init(ierr)

call mpi_comm_size(mpi_comm_world,nprocs,ierr)
call mpi_comm_rank(mpi_comm_world,myid,ierr)

! Populate the array with random numbers
call system_clock(COUNT=clock)
seed=myid*10000+mod(clock,1000)
call random_seed(PUT=seed)
do i=1,10
   call random_number(xrnd)
   a(i)=1000*xrnd
enddo

! Print the array to screen
write(*,'(10(I4,1X))') a

! Find the local max
max_local=maxval(a)

! Send local max to the root process
! Distribute the global max to all processes
call mpi_allreduce(max_local,max_global,1,mpi_integer,MPI_MAX,mpi_comm_world,ierr)

write(*,'("Process",I3," has the global max as",I4)') myid,max_global

call mpi_finalize(ierr)

stop
end program findmax

