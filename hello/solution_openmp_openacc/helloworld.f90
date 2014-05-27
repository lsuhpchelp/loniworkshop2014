program hello

  implicit none

  !$omp parallel
  print *, 'Hello World'
  !$omp end parallel

end program hello
