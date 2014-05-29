program matrixtrans

  implicit none

  include "mpif.h"

  integer :: i,j,k
  integer :: nprocs,myid,ierr
  integer :: status(mpi_status_size)
  integer :: ndim(2)  ! Dimension of the global matrix
  integer :: nldim(2) ! Dimension of the local sub-matrix
  integer :: npdim(2) ! Dimension of the process grid
  integer :: myrowid  ! Row ID in the process grid.
  integer :: mycolid  ! Column ID in the process grid.

  integer :: sizeofint    ! The extent of MPI_INTEGER
  integer :: row_type     ! The type for row data
  integer :: sub_mat_type ! The type for transposed matrix
  integer :: rowid,colid,ind_i,ind_j

  integer, allocatable :: a(:,:),a_t(:,:)           ! Global matrices
  integer, allocatable :: alocal(:,:),alocal_t(:,:) ! Local sub-matrices

  ! Global matrix dimension
  data ndim /16,12/
  character(len=*), parameter :: FMT1="(12(1x,i4))"
  character(len=*), parameter :: FMT2="(16(1x,i4))"

  call mpi_init(ierr)

  call mpi_comm_size(mpi_comm_world,nprocs,ierr)
  call mpi_comm_rank(mpi_comm_world,myid,ierr)

  ! Let the root process print out the first row of the original
  ! of the matrix, so we can check if it is correctly transposed
  ! Later.

  if (myid.eq.0) then
     allocate(a(ndim(1),ndim(2)))
     i=1
     do j=1,ndim(2)
        a(i,j)=i*100+j
     enddo
     write(*,*) "The first row before transposition:"
     write(*,'(<ndim(2)>(I4,1X))') a(i,:)
  endif

  ! Set up a 2-d process grid

  npdim(1)=2
  if (mod(nprocs,npdim(1)).ne.0) then
     if (myid.eq.0) then
        write(*,*) "Error: nprocs is not multiple of npdim(1)"
     endif
     stop
  endif
  npdim(2)=nprocs/npdim(1)
  myrowid=myid/npdim(2)
  mycolid=myid-npdim(2)*myrowid

  ! Find the dimensions of the sub-matrix.

  nldim(1)=ndim(1)/npdim(1)
  nldim(2)=ndim(2)/npdim(2)
  if ((mod(ndim(1),npdim(1)).ne.0).or.(mod(ndim(2),npdim(2)).ne.0)) then
     if (myid.eq.0) then
        write(*,*) "Error: dimensions of the matrix do not match those of the process grid"
     endif
     stop
  endif
  
  ! Initialize the sub-matrix at each process.

  allocate(alocal(nldim(1),nldim(2)),alocal_t(nldim(2),nldim(1)))
  do j=1,nldim(2)
     do i=1,nldim(1)
        alocal(i,j)=(myrowid*nldim(1)+i)*100+(mycolid*nldim(2)+j)
     enddo
  enddo

  ! Define new data types

  call mpi_type_extent(mpi_integer,sizeofint,ierr)
  call mpi_type_vector(nldim(1),1,ndim(2),mpi_integer,row_type,ierr)
  call mpi_type_hvector(nldim(2),1,sizeofint,row_type,sub_mat_type,ierr)
  call mpi_type_commit(sub_mat_type,ierr)

  ! Send the transposed sub-matrix to the root process to assemble

  if (myid.eq.0) then
     allocate(a_t(ndim(2),ndim(1)))
     ! Fill in the values for the sub-matrix owned by process 0.
     do i=1,nldim(1)
        do j=1,nldim(2)
           a_t(j,i)=i*100+j
        enddo
     enddo
     do k=1,nprocs-1
        ! Decide the location of the data in the global transposed matrix.
        rowid=k/npdim(2)
        colid=k-rowid*npdim(2)
        ind_i=colid*nldim(2)+1
        ind_j=rowid*nldim(1)+1
        call mpi_recv(a_t(ind_i,ind_j),1,sub_mat_type,k,100,mpi_comm_world,status,ierr)
     enddo
  else
     ! Send the data to the root process.
     call mpi_send(alocal,nldim(1)*nldim(2),mpi_integer,0,100,mpi_comm_world,ierr)
  endif

  ! Let the root process print out the first column of the transposed
  ! matrix. It should match the row printed at the beginning of the 
  ! program.

  if (myid.eq.0) then
     i=1
     write(*,*) "The first column after transposition:"
     write(*,'(<ndim(2)>(I4,1X))') a_t(:,i)
  endif

  ! Deallocate the arrays and terminate MPI.

  deallocate(alocal,alocal_t)
  if (myid.eq.0) then
     deallocate(a,a_t)
  endif

  call mpi_finalize(ierr)

  stop
end program matrixtrans
