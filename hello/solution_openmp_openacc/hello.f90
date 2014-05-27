program hello
  use omp_lib
  implicit none
  integer i
  !$omp parallel private(i)
  i = omp_get_thread_num()
  if (mod(i,2).eq.1) then
    print *,'Hello from thread',i,', I am odd!'
  else
    print *,'Hello from thread',i,', I am even!'
  endif
  !$omp end parallel
end program hello
