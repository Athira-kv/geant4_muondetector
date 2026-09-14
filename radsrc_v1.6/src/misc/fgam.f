c
      PROGRAM FGAM
c
c
c     External Routine Declarations
c     -----------------------------
      LOGICAL RSMIX,RSLOADCONFIG,RSSOURCECONFIG
      INTEGER RSNLINES
      REAL*8 RSGETPHOTON
      INTEGER RSGETREPORT
c
      EXTERNAL RSNEWSOURCE, RSADDISOTOPE, RSMIX
      EXTERNAL RSSORT, RSGETLINES, RSNLINES
      EXTERNAL RSGETPHOTON,RSLOADCONFIG,RSSOURCECONFIG
      EXTERNAL RSGET4V, RSGETREPORT, RSADDCONFIG
c
c     Local Variable Declarations
c     ---------------------------
      PARAMETER (NMAX = 100) ! max gamma lines
      INTEGER*8 HANDLE
      LOGICAL SUCCESS
      REAL*8 LINES(2,NMAX) ! Energy (keV), Intensity (#/s/g)
      INTEGER I,J,NL
      REAL*8 ENERGY(4)
      CHARACTER*1024 REPORT
      INTEGER LENREPORT
      CHARACTER*80 INPUTLINE ! a line from an the input deck
c
c     Intrinsics and Subroutines
c     --------------------------
c
      REAL*8 RNG
      EXTERNAL SEED, RNG
c
c     Common
c     ------
      COMMON RANDSTATE
      INTEGER RANDSTATE
c
 1010 FORMAT (F10.4,2X,E12.5)
c     Begin executable statements
c     ---------------------------
c
      CALL SEED(1)
c     Set up problem
      CALL RSNEWSOURCE(HANDLE)
c
c  --- Various input options
c
c  Directly configure by function calls
c     CALL RSADDISOTOPE(HANDLE,82,210,0,1.D0) ! Pb-210
c     CALL RSADDISOTOPE(HANDLE,92,238,0,1.D0) ! U-238, produces brem
c     SUCCESS = RSMIX(HANDLE,25.D0)
c
c  Configure by file specified by ENV Variable
c     SUCCESS = RSLOADCONFIG(HANDLE,'')
c
c  Configure by file specified by program
c     SUCCESS = RSLOADCONFIG(HANDLE,'./input')
c
c  Configure by inline character strings
      INPUTLINE = 'U234 1.0 U235 93.3 U236 0.6'
      CALL RSADDCONFIG(HANDLE,INPUTLINE)
      INPUTLINE = 'U238 5.1 AGE 20'
      CALL RSADDCONFIG(HANDLE,INPUTLINE)
      SUCCESS = RSSOURCECONFIG(HANDLE)
c
c  --- End of Input Options
      IF (SUCCESS .NEQV. .TRUE.) THEN
         PRINT *,
     >   'Error setting up problem, check RADSRC_CONFIG env var.'
         STOP
      ENDIF
c     At this point, we can get data or photons
c
c     Get a summary report
      LENREPORT = RSGETREPORT(HANDLE,REPORT)
      PRINT *, REPORT(1:LENREPORT)
c
c     Retrieve Data
      NL = RSNLINES(HANDLE)
      PRINT *, NL,' lines computed.'
      CALL RSSORT(HANDLE,2) ! 1=energy, 2= intensity
      CALL RSGETLINES(HANDLE,LINES,NMAX)
      IF (NL .GT. NMAX) NL = NMAX
c
c     Write lines to lines.txt
      OPEN(UNIT=15,FILE='lines.txt')
      WRITE (UNIT=15,FMT=*) 'ENERGY  INTENSITY'
      DO 10 I = 1, NL
         WRITE (UNIT=15,FMT=1010) LINES(1,I), LINES(2,I)
   10 CONTINUE
      CLOSE (UNIT=15)
c
c     Sample from the distribution
      OPEN(UNIT=15,FILE='photons.txt')
      DO 20 I = 1, 5000
c        WRITE (UNIT=15,FMT=*) RSGETPHOTON(HANDLE,RNG)
         CALL RSGET4V(HANDLE,ENERGY,RNG)
         WRITE (UNIT=15,FMT=*) (ENERGY(J),J=1,4)
   20 CONTINUE
      CLOSE (UNIT=15)
c
      END
c
c
c     Low quality LCPRNG
c
c
      FUNCTION RNG()
      REAL*8 RNG
c
      COMMON RANDSTATE
      INTEGER RANDSTATE
      INTEGER*8 TMP
c
c
      TMP=RANDSTATE
      TMP = (TMP * 214013 + 2531011)
      RANDSTATE = MOD(TMP,16777216)
      RNG = RANDSTATE / 16777216D0
c
      END
c
      SUBROUTINE SEED(I)
      INTEGER I
      COMMON RANDSTATE
      INTEGER RANDSTATE
c
      RANDSTATE=I
      END

