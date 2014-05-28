module mat_param

  implicit none

  integer :: nra                   ! Number of rows for matrix A.
  integer :: nca                   ! Number of columns for matrix A.
  integer :: ncb                   ! Number of rows for matrix B. 

  real*8,allocatable :: a(:,:)     ! Matrix A
  real*8,allocatable :: b(:,:)     ! Matrix B
  real*8,allocatable :: c(:,:)     ! Matrix C

end module mat_param

module mpi_param
  
  use mpi

  implicit none

  integer :: nprocs                ! Number of processes
  integer :: myrank                ! Rank of current process
  integer :: mpi_err               ! MPI error flag
  integer :: errcode               ! MPI error code
  integer status(MPI_STATUS_SIZE)  ! MPI status

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

  integer*8, dimension(8) :: value ! Time variable.

  real*8 :: flops                  ! Number of floating point operations needed for the matrix multiplication.
  real*8 :: init_time              ! Time variable. 
  real*8 :: start_time             ! Time variable.
  real*8 :: end_time               ! Time variable.

  integer :: cols                  ! Number of columns processed by each process.
  integer :: jcb_start             ! Start index for current process. 
  integer :: jcb_end               ! End index for current process.

  ! Initialize MPI environment.

  call mpi_init(mpi_err)

  ! Find out the total number of processes and the rank of current process.

  call mpi_comm_rank(mpi_comm_world, myrank, mpi_err)
  call mpi_comm_size(mpi_comm_world, nprocs, mpi_err)
  
  ! Read and validate command line parameters.

  argc=iargc()
  if (argc .ne. 3) then

     ! Incorrect number of arguments. Print the usage info.

     if (myrank .eq. 0) then
        write(*,*) "Usage: ser_mm nrows irow icol"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop

  else

     ! Process the arguments.

     call getarg(1, arg)
     read(arg, '(i)') nra
     call getarg(2, arg)
     read(arg, '(i)') ipeek
     call getarg(3, arg)
     read(arg, '(i)') jpeek

  endif

  ! We only deal with square matrices in this exercise.

  nca=nra
  ncb=nra

  ! Check if the dimensions of matrices are multiples of nproces.

  if (mod(nra,nprocs) .ne. 0) then
     if (myrank.eq.0) then
        write(*,*) "Nrows is not a multiple of nprocs!"
     endif
     call mpi_abort(mpi_comm_world, errcode, mpi_err)
     stop
  endif

  ! Calculate the number of floating point operations needed for the matrix multiplication

  flops = 2d0 * float(nra) * float(nca) * float(ncb)

  ! Find out current time.

  call date_and_time(VALUES=value)
  init_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Allocate memory for A, B and C.

  allocate(a(nra,nca), &
           b(nca,ncb), &
           c(nra,ncb))

  ! Initialize A, B and C.

  do j=1,nca
     do i=1,nra
        a(i,j)=i+j
     enddo
  enddo
  do j=1,ncb
     do i=1,nca
        b(i,j)=i*j
     enddo
  enddo
  c=0.d0

  ! Find out current time.

  call date_and_time(VALUES=value)
  start_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Matrix multiplication.

  ! Decompose A in 1-D in a blocked manner.

  cols=ncb/nprocs

  ! blank 1: calculate the start and end indices for each process

  do i=1,nra
     do j=jcb_start,jcb_end
        do k=1,nca
           c(i,j)=c(i,j)+a(i,k)*b(k,j)
        enddo
     enddo
  enddo

  ! Send the results back to the root process.

  if (myrank.eq.0) then
     
     do i=1,nprocs-1
  !blank 2: receive data from other processes.
     enddo

  else
  ! blank 3: send data to the root process.

  endif

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

  ! Validate the result at the root process.

  if (myrank .eq. 0) then

     write(*,'(A)') 'Value calculated by the program:'
     write(*,'(A,es14.6)') 'C(i,j) =',c(ipeek,jpeek)
     write(*,'(A)') 'It should be:'
     write(*,'(A,es14.6)') 'C(i,j) =',float(ipeek)*float(jpeek)*float(nra)*(float(nra)+1)/2 + &
          float(jpeek)*float(nra)*(float(nra)+1)*(2*float(nra)+1)/6
     write(*,*)

  endif

  ! Deallocate A, B and C.
  deallocate(a,b,c)
  
  call mpi_finalize(mpi_err)

  stop

end program ser_matmul
