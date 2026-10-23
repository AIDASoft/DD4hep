#include "geant321/pilot.h"
      SUBROUTINE GPVOLU(NUMB)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Routine to print VOLUMES data structures JVOLUM          *
C.    *       NUMB     Volume number                                   *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GPRINT                               *
C.    *         Author  R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcnum.inc"
      CHARACTER TEXT*1024
      INTEGER   LENOCC
      COMMON/FMOTH/INGLOB,IVOMGL
      CHARACTER*4  ISHAP(30), NAME, MOTHER
      DIMENSION    PAR(50),ATT(20)
      SAVE ISHAP
      DATA ISHAP/'BOX ','TRD1','TRD2','TRAP','TUBE','TUBS','CONE',
     +           'CONS','SPHE','PARA','PGON','PCON','ELTU','HYPE',
     +13*'    ', 'GTRA','CTUB','    '/
C.
C.    ------------------------------------------------------------------
      IF (JVOLUM.LE.0) GO TO 99
C
      N1     = 1
      N2     = NVOLUM

      DO 30 I=N1,N2
      JVO = LQ(JVOLUM-I)
      NIN=Q(JVO+3)
      IF (JVO.LE.0)   GO TO 30
C
      MOTHER = ''
      NAME = ''
      CALL UHTOC(IQ(JVOLUM+I),4,NAME,4)
      CALL GFMOTH(NAME,MOTHER,KONLY)
      IF( I.EQ.1 ) MOTHER = ''
      
      JVOMOT=LQ(JVOLUM-IVOMGL)
      MNIN=Q(JVOMOT+3)
      IF(MNIN.LE.0)THEN
        NMBR=1
      ELSE
        JIN = LQ(JVOMOT-INGLOB)
        NMBR=Q(JIN+3)
      ENDIF
 
      IS   = Q(JVO+2)
      NMED = Q(JVO+4)
C
      CALL GFPARA(NAME,NMBR,1,NPAR,NATT,PAR,ATT)
C     Handed fix: gives bad results otherwise
      IF( NAME.EQ.'VPBO' .OR. NAME.EQ.'STUB' .OR. NAME.EQ.'SEQU' ) THEN
        MOTHER = '    '
      END IF
      IF( KONLY.EQ.1 ) THEN
        WRITE(TEXT,100) I, NAME, MOTHER, NMED, ISHAP(IS), 'ONLY'
      ELSE
        WRITE(TEXT,100) I, NAME, MOTHER, NMED, ISHAP(IS), 'MANY'
      END IF
      DO J=1, NPAR
        WRITE(TEXT(LENOCC(TEXT)+1:), 102) PAR(J)
      END DO
      WRITE(TEXT(LENOCC(TEXT):), 103) '] } )'
      WRITE(6, 103) TEXT(1:LENOCC(TEXT))
      
 100  FORMAT('aleph_geometry.add_volume( { ',
     &  '  "index":'      I5,
     &  ', "name": "',    (A), '"',
     &  ', "mother": "',  (A), '"',
     &  ', "medium":',    I5,
     &  ', "shape": "',   (A), '"',
     &  ', "only": "',     A4, '"',
     &  ', "params": [ ' )
 102  FORMAT( F10.4, ',' )
 103  FORMAT( (A) )

   30 CONTINUE
   99 END
