program picalc       
implicit none       
integer, parameter :: n=1000000       
integer :: i       
real(kind=8) :: t, pi       
pi = 0.0    
!$acc parallel loop       
do i=0, n-1         
    t = (i+0.5)/n         
    pi = pi + 4.0/(1.0 + t*t)       
end do    
!$acc end parallel loop       
print *, 'pi=', pi/n    
end program picalc 
