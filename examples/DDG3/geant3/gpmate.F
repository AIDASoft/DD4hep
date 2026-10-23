#include "geant321/pilot.h"
      SUBROUTINE GPMATE (NUMB)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Routine to print material data structures JMATE          *
C.    *       NUMB     Material number                                 *
C.    *                                                                *
C.    *      Changed by S.Egli at 8.5.90: also show mixture contents   *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GPRINT                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcnum.inc"
      CHARACTER TEXT*1024, COMMA*1
      INTEGER   LENOCC
C.
C.    ------------------------------------------------------------------
C.
      IF (JMATE.LE.0) GO TO 999
      IF (NUMB .EQ.0) THEN
         N1     = 1
         N2     = NMATE
      ELSE
         N1     = ABS(NUMB)
         N2     = ABS(NUMB)
      ENDIF
C
      DO 20 I=N1,N2
         JMA = LQ(JMATE-I)
         IF (JMA.LE.0) GO TO 20
C
         NMIXT=Q(JMA+11)
C        'MATERIAL','A','Z','DENSITY','RADIAT L','ABSORP L',' NMIXT',[A,Z,Wmixt]
         WRITE(TEXT,100) I, (Q(JMA + J),J = 1,10)
         IF(NMIXT.GT.1)THEN
           JMX=LQ(JMA-5)
            DO 10 J=1,NMIXT
              WRITE(TEXT(LENOCC(TEXT)+1:), 200)
     &          Q(JMX+NMIXT+J),Q(JMX+J),
     &          Q(JMX+2*NMIXT+J)
 10         CONTINUE
         ELSE
           WRITE(TEXT(LENOCC(TEXT)+1:), 101) 'X'
         ENDIF
         WRITE(TEXT(LENOCC(TEXT):), 101) '] } )'
         WRITE(6,101) TEXT(1:LENOCC(TEXT))
   20 CONTINUE
 100  FORMAT ('aleph_geometry.add_material( {',
     &  '  "index":'    I8,
     &  ', "name": "',  5A4,'"',
     &  ', "A":',       F7.3,
     &  ', "Z":',       F7.3,
     &  ', "density":', F10.3,
     &  ', "intlen":',  E10.3,
     &  ', "radlen":',  E10.3,
     &  ', "mix": [ ' )
 101  FORMAT( (A) )
 200  FORMAT( '{"Z":',F7.3,', "A":', F7.3, ', "wmix":', F6.4,'},' )
 999  END
