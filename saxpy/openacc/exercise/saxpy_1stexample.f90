subroutine saxpy(n, a, x, y)
    real :: x(n), y(n), a
    integer :: n, i
    !$acc kernels
    do i=1,n
    y(i) = a*x(i)+y(i)
    enddo
    !$acc end kernels
end subroutine saxpy

program saxpy_example
  use omp_lib 

  implicit none
  integer :: i,n
  real,dimension(:),allocatable :: x, y
  real :: a,start_time, end_time

  n=500000000
  allocate(x(n),y(n))
  a = 2.0
  x(:) = 1.0
  y(:) = 1.0

  start_time = omp_get_wtime()
  call saxpy(2**20, 2.0, x, y)
  end_time = omp_get_wtime()
  deallocate(x,y)
  
  print '(a,f15.6,a)', 'SAXPY Time: ', end_time - start_time, 'in secs'
  
end program saxpy_example
    
