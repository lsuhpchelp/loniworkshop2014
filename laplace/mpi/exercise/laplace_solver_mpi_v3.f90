module control_param

  use mpi

  implicit none

  ! Parameters to control the problem size

  integer niter         ! Current iteration number.
  integer maxiter       ! Maximum iterations to allow.
  integer nr            ! Number of rows.
  integer nc            ! Number of columns
  integer iprint        ! Iteration frequency for output.

  real*8 relerr

  parameter ( maxiter = 1000000 )

  ! MPI related parameters

  integer ierror,myid,nprocs
  integer ncl,nrl         ! Number of columns and rows per process
  integer nprocr,nprocc   ! Size of the process grid
  integer myrowid,mycolid ! Process row and column id in the process grid

end module control_param

program laplace_main

!***********************************************************************
!
!     Solve the Laplacian Heat Equation using a 4-point stencil.
!
!     Usage:  laplace nrows ncols npcols niter iprint relerr
!
!        nrows ... Number of rows in the matrix.
!        ncols ... Number of columns in the matrix.
!        niter ... Number of iterations to allow.
!        iprint .. Iterations between printed output.
!        relerr .. The max convergence relative error desired.
!        npcols .. Number of columns in the process grid.
!
!***********************************************************************
  use control_param

  implicit none

  integer argc          ! Number of command line arguments. 
  integer iargc         ! External function returning argc
  integer i             ! Scratch variable.

  integer time1(8),time2(8),time3(8)

  character*80 arg
  
  ! Now let's get into the program itself.

  call mpi_init(ierror)

  call mpi_comm_rank(mpi_comm_world,myid,ierror)
  call mpi_comm_size(mpi_comm_world,nprocs,ierror)

  write(*,'("This is process",I4," out of",I4," processes")') myid,nprocs

  call date_and_time(VALUES=time1)

  ! Since nr must be greater than 0 to have a valid matrix, we'll use
  ! it to flag any startup error conditions. The only one we'll
  ! actually check is the number of arguments passed to the
  ! program. Extra checks are left as an exercise.
      
  nr = 0

  ! The usage description requires 4 command line arguments. If that
  ! ever changes, then the logic used here must change to match.

  argc = iargc()

  if ( argc .ne. 6 ) then

     ! Do nothing but inform the user on the proper syntax.
            
     write(*,*) "Usage: laplace nrows ncols npcols niter iprint relerr"

  else

     ! Process the arguments. GETARG is a common extension function
     ! found on Unix-like systems. Check your compiler user docs if
     ! you run into trouble.
            
     call getarg( 1, arg )
     read( arg, '(i)' ) nr
     call getarg( 2, arg )
     read( arg, '(i)' ) nc
     call getarg( 3, arg )
     read( arg, '(i)' ) nprocc
     call getarg( 4, arg )
     read( arg, '(i)' ) niter
     call getarg( 5, arg )
     read( arg, '(i)' ) iprint
     call getarg( 6, arg )
     read( arg, '(f)' ) relerr

     ! Cap the number of iterations. 
            
     if ( niter .gt. maxiter ) then
        write(*,*) "Warning: the number of iterations exceeds",maxiter
        write(*,*) "Warning: the number of iterations is changed to",maxiter
        niter = maxiter
     end if

     if (mod(nprocs,nprocc).ne.0) then
        if (myid.eq.0) then 
           write(*,*) "Error: the number of processes is not multiple of the number of columns of the process grid!"
        endif
        call mpi_finalize(ierror)
        call exit(1)
     endif

     ! Set up a 2-D process grid.

     nprocr=nprocs/nprocc
     myrowid=myid/nprocc
     mycolid=myid-nprocc*myrowid

     if (mod(nc,nprocc).ne.0) then
        if (myid.eq.0) then 
           write(*,*) "Error: the number of columns is not multiple of the number of columns of the process grid!"
        endif
        call mpi_finalize(ierror)
        call exit(2)
     endif

     if (mod(nr,nprocr).ne.0) then
        if (myid.eq.0) then 
           write(*,*) "Error: the number of rows is not multiple of the number of rows of the process grid!"
        endif
        call mpi_finalize(ierror)
        call exit(3)
     endif

     ncl=nc/nprocc
     nrl=nr/nprocr

     ! Without further ado (like checking for negative values),
     ! execute the computation.
         
     call laplace

     call date_and_time(values=time2)
     time3=time2-time1

     if (myid.eq.0) then
        write(*,*)
        write( *, '("Total Time (sec): ", e10.3)' ) 3600.*time3(5)+60.*time3(6)+1.*time3(7)+0.001*time3(8)
     endif

     call mpi_finalize(ierror)

  end if

  ! Note that the IF block always falls through to this point. This
  ! follows the practice of having only 1 exit point per executable
  ! unit.
  
  ! Follow the style which places the name of the executable unit
  ! after the end statement. This help alert the reader as to the
  ! routine being entered if scrolling up from below.
  
end program laplace_main

      
subroutine laplace
!***********************************************************************
!
! Laplace Equation Solver
!
! Arguments:
!
!   None
!        nprocs .. (IN) number of processes.
!
! T is initially 0.0 in the interior of the mesh, and the 
! boundaries are as indicated below:
!
!              T=0.   
!           _________              ____|____|____|____    Y
!           |       | 0           |    |    |    |    |   |
!           |       |             |    |    |    |    |   |
!     T=0.  | T=0.0 | T           | 1  | 2  | 3  | 4  |   |
!           |       |             |    |    |    |    |   |
!           |_______| 100         |    |    |    |    |   |_______X
!           0  T  100             |____|____|____|____|      
!                                      |    |    |
!
! The solution algorithm is the Central Differencing Method.
! Each process is assigned a subgrid that are adjacent to one
! another and uniformly span the mesh. The boundary cells
! adjacent to each subgrid must be exchanged after each iteration. 
!
! Revision History:
!
!     Jim Lupo, LSU ....... Jan 2012; Serial version and added comments.
!     Raghu Reddy, PSC .... Apr 1999; better verification
!     Susheel Chitre PSC .. Feb 1994
!
!***********************************************************************

  use control_param

  implicit none

  integer i, j, iter
  integer tagl,tagr,tagu,tagd
  integer reqid_rl,reqid_rr,reqid_ru,reqid_rd
  integer reqid_sl,reqid_sr,reqid_su,reqid_sd
  integer status(mpi_status_size)
  integer rowtype
  
  real*8 dt,dtg
  real*8, allocatable :: dta(:)
  real*8, allocatable :: t(:,:), told(:,:)

  parameter (tagl=100,tagr=101,tagu=102,tagd=103)

  allocate( t(0:nrl+1,0:ncl+1), told(0:nrl+1,0:ncl+1) )
  allocate(dta(0:nprocs-1))

  ! Initialize the data arrays.

  call initialize( t )
  call initialize( told )
  call set_bcs( t )
  call set_bcs( told )

  ! blank 1: Define a datatype to transfer row data

  call mpi_type_commit(rowtype,ierror)

  ! Set some timing variables for total time and iteration time. 

  do iter = 1, niter

     do j = 1, ncl
        do i = 1, nrl
           t(i,j) = 0.25 * ( told(i+1,j) + told(i-1,j) &
                    + told(i,j+1) + told(i,j-1) )
        end do
     end do

     ! Update the boundary data. We'll send right and receive from
     ! left first, then send left and receive from right. In both
     ! cases, we'll post the receives first, and then do the sends.

     ! blank 2: fill in arguement #1 and #4 for the MPI sends and receives below:

     ! Send R - Recv L:
     
     if ( mycolid .ne. 0 ) then
        call mpi_irecv( , nrl, mpi_real8, , tagr, mpi_comm_world, reqid_rl, ierror )
     end if

     if ( mycolid .ne. nprocc-1 ) then
        call mpi_send( , nrl, mpi_real8, , tagr, mpi_comm_world, reqid_sr,ierror )
     end if
         
     ! Send L - Recv R:

     if ( mycolid .ne. nprocc-1 ) then
        call mpi_irecv( , nrl, mpi_real8, , tagl, mpi_comm_world, reqid_rr, ierror )
     end if

     if ( mycolid .ne. 0 ) then
        call mpi_send( , nrl, mpi_real8, , tagl, mpi_comm_world, reqid_sl,ierror )
     end if

     ! Send U - Recv D:

     if ( myrowid .ne. nprocr-1 ) then
        call mpi_irecv( , 1, rowtype, , tagu, mpi_comm_world, reqid_rd, ierror )
     end if

     if ( myrowid .ne. 0 ) then
        call mpi_send( , 1, rowtype, , tagu, mpi_comm_world, reqid_su,ierror )
     end if

     ! Send D - Recv U:

     if ( myrowid .ne. 0 ) then
        call mpi_irecv( , 1, rowtype, , tagd, mpi_comm_world, reqid_ru, ierror )
     end if

     if ( myrowid .ne. nprocr-1 ) then
        call mpi_send( , 1, rowtype, , tagd, mpi_comm_world, reqid_sd, ierror )
     end if

     ! Send U - Recv D;

     if ( mycolid .ne. 0 ) then
        call mpi_wait( reqid_rl, status, ierror )
        call mpi_wait( reqid_sl, status, ierror )
     end if
     if ( mycolid .ne. nprocc-1 ) then 
        call mpi_wait( reqid_rr, status, ierror )
        call mpi_wait( reqid_sr, status, ierror )
     end if
     if ( myrowid .ne. nprocr-1 ) then 
        call mpi_wait( reqid_rd, status, ierror )
        call mpi_wait( reqid_sd, status, ierror )
     end if
     if ( myrowid .ne. 0 ) then 
        call mpi_wait( reqid_ru, status, ierror )
        call mpi_wait( reqid_su, status, ierror )
     end if

     ! Check on convergence, and move current values to old.

     dt = 0.

     do j = 1, ncl
        do i = 1, nrl
           dt = max( dabs(t(i,j) - told(i,j)), dt )
           told(i,j) = t(i,j)
        end do
     end do

     ! Find the global max convergence error.

     call mpi_allreduce( dt, dtg, 1, mpi_real8, mpi_max, mpi_comm_world, ierror)

     ! Check if output required.

     if (myid.eq.0) then
        if ( iprint .ne. 0 ) then
           if ( mod( iter, iprint ) .eq. 0 ) then
              write( *, 100) iter, dtg
           end if
        end if
     endif

     ! Check if convergence criteria meet.

     if ( dtg .lt. relerr ) then
        print *, 'Solution has converged.'
        goto 10
     end if

     ! Go do another iteration.

  end do

10 continue

  if ( iter .gt. niter ) then
     iter = niter
  end if

  ! And we're out'a here.

  deallocate(t,told,dta)

  return

100 format( "Iteration: ", i7, "; Convergence Error: ", e10.3 )

end subroutine laplace
      

subroutine initialize( t )
!***********************************************************************
!
! Initializes a 2-D data mesh.
!
! Arguments:
!
!   t .. (IN/OUT) 2-D array holding mesh data.
!
!***********************************************************************

  use control_param

  implicit none

  integer i, j
      
  real*8 t(0:nrl+1,0:ncl+1)

  do i = 0, nrl + 1
     do j = 0, ncl + 1
        t(i,j) = 0
     end do
  end do
      
  return

end subroutine initialize
      

subroutine set_bcs( t )
!***********************************************************************
!
! Routine which sets up the fixed boundary conditions.
!
! Arguments:
!
!   t ....... (IN/OUT) 2-D array holding mesh data.
!
!***********************************************************************

  use control_param

  implicit none

  integer i, j

  real*8 t(0:nrl+1,0:ncl+1), tmin, tmax

  ! Left and Right Boundaries

  if (mycolid.eq.nprocc-1) then

     tmin=myrowid*100.0/nprocr
     tmax=(myrowid+1)*100.0/nprocr

     do i = 0, nrl+1
        t(i,ncl+1) = tmin + ((tmax-tmin)/nrl)*i
     end do

  endif

  ! Top and Bottom Boundaries

  if (myrowid.eq.nprocr-1) then

     tmin=mycolid*100.0/nprocc
     tmax =(mycolid+1)*100.0/nprocc

     do j = 0, ncl + 1
        t(nrl+1,j) = tmin + ((tmax-tmin)/ncl)*j
     end do

endif

  return

end subroutine set_bcs
