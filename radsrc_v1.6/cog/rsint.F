      subroutine rsint(ArgA,ArgV,nargs)

c Isotropic point source.

c Inputs:
c ArgA = array of user's arguments, as 8-character ASCII words.
c ArgV = array of user's arguments, as real*8 values.
c nagrs = # of user arguments.

c Outputs:
c User must generate values for these source parameters:
c--integer*4 values----
c   Sntype = particle ID.
c--real*8 values----
c   Sx,Sy,Sz = Source particle location.
c   Su,Sv,Sw = Direction cosines of particle's velocity vector.
c   Seng = Particle kinetic energy.
c   Svel = Particle velocity.
c   Sage = Particle age at time of creation.
c   Swate = Particle weight.
c   xnors = Source Strength.

c--Sntype Values----
c Sntype     Particle
c  1          neutron
c  2          proton
c  3          deuteron
c  4          triton
c  5          he-3
c  6          alpha
c  7          photon
c  8          electron
c  9          positron

c Point Source at P0:
c  atype = ArgA(1)
c  = "neutron" or "photon"
c  xnors = ArgV(2)
c  P0x = ArgV(3)
c  P0y = ArgV(4)
c  P0z = ArgV(5)
c  Eng = ArgV(6)
c  Age = ArgV(7)


c *******BEGIN DECLARATIONS*******************************************
      implicit none
      character*8 ArgA(1)
      real*8 ArgV(1)
      integer*4 nargs

C
#include "basic.inc"
#include "source.inc"
#include "spav.inc"
#include "Sorcm.inc"
#include "sorpar.inc"
#include "FlagsEtc.inc"

      INTEGER*8 HANDLE
      LOGICAL SUCCESS,RSLOADCONFIG, FIRST, RSSOURCECONFIG
      REAL*8 RSGTRPHOTON

      EXTERNAL RSLOADCONFIG, RSGTRPHOTON, RSADDCONFIG, RSSOURCECONFIG

      common /rscommon/ HANDLE, FIRST
      data first /.true./

c Local variables.
      character*8 atype
      real*4 RNG
      EXTERNAL RNG

      real*8 cospolang,sinpolang,phi
      real*8 rm                     ! Rest mass of particle (MeV)
      real*8 eratio                 ! Particle KE/E0
      real*8 energy, radius

      integer*4 i

#include "PhysConst.inc"

c *******END DECLARATIONS*******************************************
c   set type photon
      Sntype = 7
      
 1010 FORMAT (F10.4,2X,E12.5)

      if(first)then
         print *, " in setup"
         first=.false.
         CALL RSNEWSOURCE(HANDLE)
         do 99 i=1,nargs
            CALL RSADDCONFIG(HANDLE,ArgA(i))
 99      enddo
         SUCCESS = RSSOURCECONFIG(HANDLE)
         IF (SUCCESS .NEQV. .TRUE.) THEN
            PRINT *, 'Error setting up problem'
            STOP
         ENDIF
      endif

      ENERGY = RSGTRPHOTON(HANDLE,RNG)
      Seng = energy/1000.0      ! Convert to MeV
      print *,"Energy = ",seng
    

      
      xnors = 1.0
c      Seng = ArgV(6)
c      Seng = Seng*uce  ! Convert to MeV
      Sage = 0.0

C Choose position
      cospolang = 2.0d0*RNG() - 1.0d0
      sinpolang = DSQRT(1.0d0 - cospolang*cospolang)
c Select an azimuthal angle uniformly on (0,2Pi)
      phi = 2.0d0* Pi * RNG()
      radius = 5.01*RNG()**(1.0/3.0)
      Sx = radius*sinpolang * DCOS(phi)
      Sy = radius*sinpolang * DSIN(phi)
      Sz = radius*cospolang

c Choose emission angle isotropically.
c Select a polar angle direction cosine.
      cospolang = 2.0d0*RNG() - 1.0d0
      sinpolang = DSQRT(1.0d0 - cospolang*cospolang)
c Select an azimuthal angle uniformly on (0,2Pi)
      phi = 2.0d0* Pi * RNG()
      Su = sinpolang * DCOS(phi)
      Sv = sinpolang * DSIN(phi)
      Sw = cospolang

c Calculate velocity.
c Below the given KE/E0 threshold, we use the classical velocity to
c  maintain accuracy.
      if(Sntype.ne.7) then
c Not a photon.
        rm = RME(Sntype)
        eratio = Seng/rm
         if(eratio.gt.1.d-8) then
c Relativistic vel calculation (cm/s).
         Svel = cspeed*DSQRT(1.0d0 - rm*rm/((rm + Seng)*(rm + Seng)))
         else
c Non-relativistic vel calculation (cm/s).
         Svel = cspeed*DSQRT(2.0d0*Seng/rm)
         endif
        else
c Photon
        Svel = cspeed
      endif
      Svel = Svel*ucli

      Swate = 1.0d0

      return
      end
