program hello
  ! Include/Use omp_lib.h/omp_lib ?
  implicit none
  integer i
  ! Add OMP Directive
  i = ! Get Thread ID
  if (mod(i,2).eq.1) then
    print *,'Hello from thread',i,', I am odd!'
  else
    print *,'Hello from thread',i,', I am even!'
  endif
  ! End OMP Directive ?
end program hello
