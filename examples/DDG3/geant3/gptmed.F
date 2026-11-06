#include "geant321/pilot.h"
      SUBROUTINE GPTMED (NUMB  )
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Routine to print tracking media data structure JTMED     *
C.    *       NUMB     medium number                                   *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GPRINT                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcnum.inc"
      DIMENSION ITITLE(5)
C.
C.    ------------------------------------------------------------------
C.
      IF (JTMED.LE.0)  GO TO 99
C
      IF (NUMB  .EQ.0)THEN
         N1     = 1
         N2     = NTMED
      ELSE
         N1     = ABS(NUMB)
         N2     = ABS(NUMB)
      ENDIF
C
      DO 10 I=N1,N2
         JTM = LQ(JTMED-I)
         IF (JTM.NE.0)THEN
            IMAT = Q(JTM+6)
            ISVOL = Q(JTM+7)
            IFIELD = Q(JTM+8)
            CALL UCOPY(IQ(JTM+1),ITITLE,5)
            WRITE(6,100) I,ITITLE,IMAT,ISVOL,IFIELD,
     &        (Q(JTM+J),J=9,14)
          ENDIF
 10   CONTINUE
 99   RETURN
 100  FORMAT('aleph_geometry.add_medium( { ',
     &  '  "index":'      I8,
     &  ', "name": "',    5A4,'"',
     &  ', "material":',  I5,
     &  ', "isvol":',     I3,
     &  ', "ifield":',    I3,
     &  ', "magfield":',  F7.3,
     &  ', "tmaxfd":',    E10.3,
     &  ', "stepmax":',   G10.3,
     &  ', "deemax":',    G10.3,
     &  ', "epsilon":',   G10.3,
     &  ', "stepmin":',   G10.3, ' } )')
      END
