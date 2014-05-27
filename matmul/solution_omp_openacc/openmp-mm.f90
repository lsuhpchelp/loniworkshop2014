program matmul

  implicit none
  integer, parameter :: dp = selected_real_kind(14)
  integer, parameter :: nra=1500
  integer, parameter :: nca=2000
  integer, parameter :: ncb=1000
  integer :: i,j,k
  real(dp) :: a(nra,nca), b(nca,ncb), c(nra,ncb),sum
  integer, dimension(8) :: value
  real(dp) :: init_time, start_time, end_time, flops

  sum = 0d0
  flops = float(nra) * float(nca) * float(ncb)

  !  Start recording matrix initialization time (Init)
  call date_and_time(VALUES=value)
  init_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Initialize the multiplicand matrix
  do i = 1, nra
     do j = 1, nca
        a(i,j) = float(i+j)
     end do
  end do

  ! Initialize the multiplier matrix
  do i = 1, nca
     do j = 1, ncb
        b(i,j) = float(i*j)
     end do
  end do

  ! Start recording matrix multiplication (computation) time (Calc)
  call date_and_time(VALUES=value)
  start_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0
  
  ! Create OpenMP threads and multiply matrix A and B
  !$omp parallel do private(sum) shared(a,b,c)
  do j = 1, nca
     do k = 1, ncb
        sum = 0d0
        do i = 1, nra
           sum = sum + a(i,j) * b(j,k)
        end do
        c(i,k) = sum
     end do
  end do
  !$omp end parallel do

  ! Stop recording computation time
  call date_and_time(VALUES=value)
  end_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0

  ! Print timing results
  print '(a,f6.3,a,f6.3,a,f6.3)', 'OPENMP-MM: Init time: ',start_time - init_time, &
       '   Calc time: ', end_time - start_time, &
       '   GFlop/s: ', 1e-9*2.d0*flops/(end_time - start_time)

end program matmul
  
