module mat_param

  implicit none

  integer :: nra                   ! Number of rows for matrix A.
  integer :: nca                   ! Number of columns for matrix A.
  integer :: ncb                   ! Number of rows for matrix B. 

  real*8,allocatable :: a(:,:)     ! Local matrix A
  real*8,allocatable :: b(:,:)     ! Local matrix B
  real*8,allocatable :: c(:,:)     ! Local matrix C
  real*8,allocatable :: cfull(:,:) ! Full matrix C

  integer :: nsrow                 ! Number of rows in the sub-matrix.
  integer :: nscol                 ! Number of colums in the sub-matrix.

end module mat_param

module mpi_param
  
  use mpi

  implicit none

  integer :: nprocs                ! Number of processes
  integer :: myrank                ! Rank of current process
  integer :: mpi_err               ! MPI error flag
  integer :: errcode               ! MPI error code

  integer :: status(MPI_STATUS_SIZE)

  integer :: vtype                 ! MPI vector data type. 

  integer :: nprows                ! Number of rows in the process grid.
  integer :: npcols                ! Number of colums in the process grid.

  integer :: iprow                 ! Row index of current process in the process grid. 
  integer :: ipcol                 ! Column index of current process in the process grid.

end module mpi_param

program ser_matmul

  use mat_param
  use mpi_param

  implicit none

  integer :: argc                  ! Number of command line arguments.
  integer :: iargc                 ! External function returning argc.
  character*80 :: arg              ! Container for command line arguments.

  integer :: i,j,k                 ! Scratch variables.

  integer :: ipeek,jpeek           ! Indices for the element used for result validation.
  integer :: ispeek,jspeek

  integer*8, dimension(8) :: value ! Time variable.

  real*8 :: flops                  ! Number of floating point operations needed for the matrix multiplication.
  real*8 :: init_time              ! Time variable. 
  real*8 :: start_time             ! Time variable.
  real*8 :: end_time               ! Time variable.

  ! Initialize MPI environment.

  call mpi_init(mpi_err)

  ! Find out the total number of processes and the rank of current process.

  call mpi_comm_rank(mpi_comm_world, myrank, mpi_err)
  call mpi_comm_size(mpi_comm_world, nprocs, mpi_err)
  
  ! Read and validate command line parameters.

  argc=iargc()
  if (argc .ne. 5) then

     ! Incorrect number of arguments. Print the usage info.

     if (myrank .eq. 0) then
        write(*,*) "Usage: ser_mm nrows nprow npcol irow icol"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop

  else

     ! Process the arguments.

     call getarg(1, arg)
     read(arg, '(i)') nra
     call getarg(2, arg)
     read(arg, '(i)') nprows
     call getarg(3, arg)
     read(arg, '(i)') npcols
     call getarg(4, arg)
     read(arg, '(i)') ipeek
     call getarg(5, arg)
     read(arg, '(i)') jpeek

  endif

  ! We only deal with square matrices in this exercise.

  nca=nra
  ncb=nra

  ! Perform some sanity checks.

  if (nprocs .ne. nprows*npcols) then
     if (myrank .eq. 0) then
        write(*,*) "The nubmer of processes is not equal to nprows*npcols!"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop
  endif

  if (mod(nra,nprows) .ne. 0) then
     if (myrank .eq. 0) then
        write(*,*) "NRA is not a multiple of nprows!"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop
  endif

  if (mod(ncb,npcols) .ne. 0) then
     if (myrank .eq. 0) then
        write(*,*) "NCB is not a multiple of npcols!"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop   
  endif

  ! Calculate the number of floating point operations needed for the matrix multiplication

  flops = 2d0 * float(nra) * float(nca) * float(ncb)

  ! Find out current time.

  call date_and_time(VALUES=value)
  init_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Find out the row and column indcies of the current process in the process grid.
  ! It does not matter whether the index starts from 0 or 1 as long as the bookkeeping is tidy.
  ! We choose 0 in this example.

  iprow=myrank/npcols
  ipcol=mod(myrank,npcols)

  ! Calculate the dimensions of the sub-matrix.

  nsrow=nra/nprows
  nscol=ncb/npcols

  ! Allocate memory for sub-A, sub-B and sub-C.

  allocate(a(nsrow,nca), &
           b(nca,nscol), &
           c(nsrow,nscol))

  ! Allocate full matrix C at the root process.

  if (myrank.eq.0) then
     allocate(cfull(nra,ncb))
  endif

  ! Initialize sub-A, sub-B and sub-C.

  do j=1,nca
     do i=1,nsrow
        a(i,j)=iprow*nsrow+i+j
     enddo
  enddo
  do j=1,nscol
     do i=1,nca
        b(i,j)=i*(ipcol*nscol+j)
     enddo
  enddo
  c=0.d0

  ! Find out current time.

  call date_and_time(VALUES=value)
  start_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Matrix multiplication.

  do i=1,nsrow
     do j=1,nscol
        do k=1,nca
           c(i,j)=c(i,j)+a(i,k)*b(k,j)
        enddo
     enddo
  enddo

  ! Define a vector data type.

  call mpi_type_vector(nscol,nsrow,nra,mpi_real8,vtype,mpi_err)
  call mpi_type_commit(vtype,mpi_err)

  ! Send the results to the root process.

  if (myrank.eq.0) then

     ! Move the local copy.
     cfull(1:nsrow,1:nscol)=c

     ! Receive from other processes.
     do i=1,nprocs-1
        call mpi_recv(cfull(i/npcols*nsrow+1,mod(i,npcols)*nscol+1),1,vtype,i,1,mpi_comm_world,status,mpi_err)
     enddo

  else

     call mpi_send(c,nsrow*nscol,mpi_real8,0,1,mpi_comm_world,mpi_err)

  endif

  call mpi_type_free(vtype,mpi_err)

  ! Find out current time.

  call date_and_time(VALUES=value)
  end_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Print timing information to screen.

  if (myrank .eq. 0) then

     write(*,*)
     write(*,'(A,T12,f7.3)') 'Init Time: ',start_time - init_time
     write(*,'(A,T12,f7.3)') 'Calc Time: ',end_time - start_time
     write(*,'(A,T12,f7.3)') 'GFlops: ',1d-9 * flops/(end_time - start_time)
     write(*,*)

  endif

  ! Validate the result.

  if (myrank .eq. 0) then

     write(*,'(A)') 'Value calculated by the program:'
     write(*,'(A,es14.6)') 'C(i,j) =',cfull(ipeek,jpeek)
     write(*,'(A)') 'It should be:'
     write(*,'(A,es14.6)') 'C(i,j) =',float(ipeek)*float(jpeek)*float(nra)*(float(nra)+1)/2 + &
          float(jpeek)*float(nra)*(float(nra)+1)*(2*float(nra)+1)/6
     write(*,*)

  endif

  ! Deallocate A, B and C.
  deallocate(a,b,c)
  if (myrank.eq.0) then
     deallocate(cfull)
  endif
  
  call mpi_finalize(mpi_err)

  stop

end program ser_matmul
