program saxpy

  use omp_lib 

  implicit none
  integer :: i,n
  real,dimension(:),allocatable :: x, y
  real :: a,start_time, end_time

  n=500000000
  allocate(x(n),y(n))
  a = 2.0
  !$acc data create(x), copyout(y)
  !$acc parallel
  x(:) = 1.0
  !$acc end parallel
  !$acc parallel
  y(:) = 1.0
  !$acc end parallel

  start_time = omp_get_wtime()
  !$acc parallel loop
  do i = 1, n
     y(i) = y(i) + a * x(i)
  end do
  !$acc end parallel loop
  end_time = omp_get_wtime()
  !$acc end data
  
  print '(a,f15.6,a)', 'SAXPY Time: ', end_time - start_time, 'in secs'
  print '(a,f15.6)', 'y(1) ', y(1)

  deallocate(x,y)
  
end program saxpy
    
