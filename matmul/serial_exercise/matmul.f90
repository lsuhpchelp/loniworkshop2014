program matrix_mul

  implicit none

  integer, parameter :: dp = selected_real_kind(14)
  integer :: i,j,k
  integer, parameter :: nra=1500, nca=2000, ncb=1000
  real(dp) :: a(nra,nca) , b(nca,ncb) , c(nra,ncb)
  real(dp) :: flops, sum
  real(dp) :: init_time, start_time, end_time


  flops = 2d0 * float(nra) * float(nca) * float(ncb)

  call cpu_time(init_time)
  c = 0d0

  do i = 1,nra
     do j = 1,nca
        a(i,j) = i + j
     end do
  end do
  
  do i = 1,nca
     do j = 1,ncb
        b(i,j) = i * j
     end do
  end do
  
  call cpu_time(start_time)
  ! Parallelize the following block of code
  do j = 1, nca
     do k = 1, ncb
        sum = 0d0
        do i = 1, nra
           sum = sum + a(i,j) * b(j,k) 
        end do
        c(i,k) = sum
     end do
  end do
  call cpu_time(end_time)
  
  print '(a,f6.3,a,f6.3,a,f7.3)', 'Init Time: ', start_time - init_time, &
       ' Calc Time: ', end_time - start_time, &
       ' GFlops: ', 1d-9 * flops/(end_time - start_time)
  
  
end program matrix_mul
