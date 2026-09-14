! Copyright LANL/UC/DOE - see file COPYRIGHT_INFO
       subroutine source
! dummy subroutine. aborts job if source subroutine is missing.
! if nsr=0, subroutine source must be furnished by the user.
! at entrance, a random set of uuu,vvv,www has been defined. the
! following variables must be defined within the subroutine:
! xxx,yyy,zzz,icl,jsu,erg,wgt,tme and possibly ipt,uuu,vvv,www.
! subroutine srcdx may also be needed.

       use global_data







       implicit real(kindr) (a-h,o-z)




       INTEGER*8 HANDLE
       LOGICAL SUCCESS,RSLOADCONFIG, FIRST, RSSOURCECONFIG
       REAL*8 RSGETPHOTON,etmp

       EXTERNAL RSLOADCONFIG, RSGETPHOTON, RSADDCONFIG, RSSOURCECONFIG

       EXTERNAL RANG


       common /rscommon/ HANDLE, FIRST
       data first /.true./

       if(first)then
         print *, " in setup"
         first=.false.
         CALL RSNEWSOURCE(HANDLE)
         SUCCESS = RSLOADCONFIG(HANDLE,'./problem.in')
! CALL RSADDCONFIG(HANDLE,'U234 0.00071')
! CALL RSADDCONFIG(HANDLE,'U235 0.182')
! CALL RSADDCONFIG(HANDLE,'U236 0.00284')
! CALL RSADDCONFIG(HANDLE,'U238 99.814')
! CALL RSADDCONFIG(HANDLE,'AGE 15')
! SUCCESS = RSSOURCECONFIG(HANDLE)

         IF (SUCCESS .NEQV. .TRUE.) THEN
             PRINT *, 'Error setting up problem'
             STOP
         ENDIF
       endif

       wgt=1.0
       ipt=2
       erg=RSGETPHOTON(HANDLE,rang)/1000.0
       print *, erg
       tme=0.

!....Spherical source located at (0,0,0)
       r=rdum(1)*sqrt(rang())
       phi=2.*pie*rang()
       ctheta=2.*rang()-1.
       stheta=sqrt(1.0-ctheta**2)
       zzz=r*ctheta
       yyy=r*stheta*cos(phi)
       xxx=r*stheta*sin(phi)
       jsu=0
       j=1
       i=1

!....Make sure source location is within the desired cell
! End the program if is not in the cell
       icl=namchg(1,idum(1))
       call chkcel(icl,2,j)

       if(j.ne.0) call expire(1,'Source',
     & 'Source is not in any cells on the idum card')





       return
       end subroutine source
