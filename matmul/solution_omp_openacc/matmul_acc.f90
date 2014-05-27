program matrix_mul

  implicit none

  integer, parameter :: dp = selected_real_kind(14)
  integer :: i,j,k
  integer, parameter :: nra=1500, nca=2000, ncb=1000
  real(dp) :: a(nra,nca) , b(nca,ncb) , c(nra,ncb)
  real(dp) :: flops, sum
  real(dp) :: init_time, start_time, end_time
  integer :: c1, c2, c3, cr
  integer, dimension(8) :: value


  flops = 2d0 * float(nra) * float(nca) * float(ncb)

  !$acc data create(a,b,c)
  call date_and_time(VALUES=value)
  init_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0
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
  
  call date_and_time(VALUES=value)
  start_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0
  !$acc parallel loop private(sum)
  do j = 1, nca
     do k = 1, ncb
        sum = 0d0
        !$acc loop reduction(+:sum)
        do i = 1, nra
           sum = sum + a(i,j) * b(j,k) 
        end do
        c(i,k) = sum
     end do
  end do
  !$acc end parallel loop
  call date_and_time(VALUES=value)
  end_time = float(value(6)*60) + float(value(7)) + float(value(8))/1000d0
  !$acc end data
  print '(a,f6.3,a,f6.3,a,f7.3)', 'Init Time: ', start_time - init_time, &
       ' Calc Time: ', end_time - start_time, &
       ' GFlops: ', 1d-9 * flops/(end_time - start_time)
  
end program matrix_mul
