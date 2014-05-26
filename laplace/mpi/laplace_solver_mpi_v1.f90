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
  integer ncl           ! Number of columns per process

end module control_param

program laplace_main

!***********************************************************************
!
!     Solve the Laplacian Heat Equation using a 4-point stencil.
!
!     Usage:  laplace nrows ncols niter iprint relerr
!
!        nrows ... Number of rows in the matrix.
!        ncols ... Number of columns in the matrix.
!        niter ... Number of iterations to allow.
!        iprint .. Iterations between printed output.
!        relerr .. The max convergence relative error desired.
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

  if ( argc .ne. 5 ) then

     ! Do nothing but inform the user on the proper syntax.
            
     write(*,*) "Usage: laplace nrows ncols niter iprint relerr"

  else

     ! Process the arguments. GETARG is a common extension function
     ! found on Unix-like systems. Check your compiler user docs if
     ! you run into trouble.
            
     call getarg( 1, arg )
     read( arg, '(i)' ) nr
     call getarg( 2, arg )
     read( arg, '(i)' ) nc
     call getarg( 3, arg )
     read( arg, '(i)' ) niter
     call getarg( 4, arg )
     read( arg, '(i)' ) iprint
     call getarg( 5, arg )
     read( arg, '(f)' ) relerr

     ! Cap the number of iterations. 
            
     if ( niter .gt. maxiter ) then
        write(*,*) "Warning: the number of iterations exceeds",maxiter
        write(*,*) "Warning: the number of iterations is changed to",maxiter
        niter = maxiter
     end if

     if (mod(nc,nprocs).ne.0) then
        if (myid.eq.0) then 
           write(*,*) "Error: the number of columns is not multiple of the number of processes!"
        endif
        call mpi_finalize(ierror)
        call exit(1)
     endif

     ncl=nc/nprocs
     write(*,'("Process",I4," has column",I4," through",I4,".")') myid,myid*ncl+1,(myid+1)*ncl

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
  integer tagl,tagr
  integer reqidl,reqidr
  integer status(mpi_status_size)
  
  real*8 dt,dtg
  real*8, allocatable :: dta(:)
  real*8, allocatable :: t(:,:), told(:,:)

  parameter (tagl=100,tagr=101)

  allocate( t(0:nr+1,0:ncl+1), told(0:nr+1,0:ncl+1) )
  allocate(dta(0:nprocs-1))

  ! Initialize the data arrays.

  call initialize( t )
  call initialize( told )
  call set_bcs( t )
  call set_bcs( told )

  ! Set some timing variables for total time and iteration time. 

  do iter = 1, niter

     do j = 1, ncl
        do i = 1, nr
           t(i,j) = 0.25 * ( told(i+1,j) + told(i-1,j) &
                    + told(i,j+1) + told(i,j-1) )
        end do
     end do

     ! Update the boundary data. We'll send right and receive from
     ! left first, then send left and receive from right. In both
     ! cases, we'll post the receives first, and then do the sends.

     ! Send R - Recv L:
     
     if ( myid .ne. 0 ) then
        call mpi_irecv( t(1,0), nr, mpi_real8, myid - 1, tagr, mpi_comm_world, reqidr, ierror )
        if ( ierror .ne. 0 ) then
           write( *, * ) myid, iter, '- ERROR:', ierror
        end if
     end if

     if ( myid .lt. nprocs-1 ) then
        call mpi_send( t(1,ncl), nr, mpi_real8, myid + 1, tagr, mpi_comm_world, ierror )
     end if
         
     if ( myid .ne. 0 ) then
        call mpi_wait( reqidr, status, ierror )
     end if
         
     ! Send L - Recv R:

     if ( myid .ne. nprocs-1 ) then

        call mpi_irecv( t(1,ncl+1), nr, mpi_real8, myid + 1, tagl, mpi_comm_world, reqidl, ierror )
        if ( ierror .ne. 0 ) then
           write( *, * ) myid, iter, '- ERROR:', ierror
        end if
     end if

     if ( myid .ne. 0 ) then
!           Send data to the left
        call mpi_send( t(1,1), nr, mpi_real8, myid - 1, tagl, mpi_comm_world, ierror )
     end if
         
     if ( myid .ne. nprocs-1 ) then 
        call mpi_wait( reqidl, status, ierror )
     end if

     ! Check on convergence, and move current values to old.

     dt = 0.

     do j = 1, ncl
        do i = 1, nr
           dt = max( dabs(t(i,j) - told(i,j)), dt )
           told(i,j) = t(i,j)
        end do
     end do

     ! Find the global max convergence error.

     if (myid.eq.0) then
        do i=1,nprocs-1
           call mpi_recv(dta(i) , 1, mpi_real8, i, tagl, mpi_comm_world, status, ierror )
        enddo
        dta(0)=dt
        dtg=maxval(dta)
     else
        call mpi_send( dt, 1, mpi_real8, 0, tagl, mpi_comm_world, ierror )
     endif

     ! Send the global max convergence error to all processes.

     if (myid.eq.0) then
        do i=1,nprocs-1
           call mpi_send(dtg , 1, mpi_real8, i, tagl, mpi_comm_world, ierror )
        enddo
     else
        call mpi_recv( dtg, 1, mpi_real8, 0, tagl, mpi_comm_world, status, ierror )
     endif

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
      
  real*8 t(0:nr+1,0:ncl+1)

  do i = 0, nr + 1
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

  real*8 t(0:nr+1,0:ncl+1), tmin, tmax

  ! Left and Right Boundaries

  if (myid.eq.0) then
     do i = 0, nr + 1
        t(i,0) = 0.0
     end do
  endif

  if (myid.eq.nprocs-1) then
     do i = 0, nr + 1
        t(i,ncl+1) = (100.0/nr) * i
     end do
  endif

  ! Top and Bottom Boundaries

  tmin =  myid * 100.0/nprocs
  tmax = ( myid + 1 ) * 100.0/nprocs

  do j = 0, ncl + 1
     t(0,j) = 0.0
     t(nr+1,j) = tmin + ((tmax-tmin)/ncl) * j
  end do

  return

end subroutine set_bcs
