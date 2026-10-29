C#include "geant321/pilot.h"
      SUBROUTINE GPROTM(NUMB)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Routine to print rotation matrices structure JROTM       *
C.    *       NUMB     Rotation matrix number                          *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GPRINT                               *
C.    *         Author  R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcunit.inc"
      CHARACTER*1024 TEXT
      INTEGER   LENOCC
C.
C.    ------------------------------------------------------------------
C.
      CALL GSROTM(333, 90.0, 0.0, 90.0, 90.0, 0.0, 0.0)
      IF (JROTM.LE.0)  GO TO 99
C
      IF (NUMB  .EQ.0)THEN
         N1     = 1
         N2     = IQ(JROTM-2)
      ELSE
         N1     = ABS(NUMB)
         N2     = ABS(NUMB)
      ENDIF
C
      DO 30 I=N1, N2
      JR = LQ(JROTM-I)
      IF (JR.LE.0)   GO TO 30
C
      IFLAG = Q(JR+10)
      WRITE(TEXT,100) I, IFLAG, (Q(JR+J), J = 11,16)
      DO J = 1, 9
        WRITE(TEXT(LENOCC(TEXT)+1:), 101) Q(JR+J)
      END DO
      WRITE(TEXT(LENOCC(TEXT):), 102) '] } )'
      WRITE(6, 102) TEXT(1:LENOCC(TEXT)+1)
 30   CONTINUE

 99   RETURN

 100  FORMAT('aleph_geometry.add_rotation( {',
     &  '  "index": ', 1X, I4,
     &  ', "flag":',   1X, I5,
     &  ', "theta1":', 1X, E10.4,
     &  ', "phi1":',   1X, E10.4,
     &  ', "theta2":', 1X, E10.4,
     &  ', "phi2":',   1X, E10.4,
     &  ', "theta3":', 1X, E10.4,
     &  ', "phi3":',   1X, E10.4,
     &  ', "matrix": [' )
 101  FORMAT( F10.4, ',' )
 102  FORMAT( 1X, (A) )
      END
