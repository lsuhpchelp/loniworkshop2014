program saxpy

  use omp_lib 

  implicit none
  integer :: i,n
  real,dimension(:),allocatable :: x, y
  real :: a,start_time, end_time

  n=500000000
  allocate(x(n),y(n))
  a = 2.0
  !FIXME: complete the OpenACC directives here
  x(:) = 1.0
  y(:) = 1.0

  start_time = omp_get_wtime()
  do i = 1, n
     y(i) = y(i) + a * x(i)
  end do
  end_time = omp_get_wtime()
  deallocate(x,y)
  
  print '(a,f15.6,a)', 'SAXPY Time: ', end_time - start_time, 'in secs'
  
end program saxpy
    
