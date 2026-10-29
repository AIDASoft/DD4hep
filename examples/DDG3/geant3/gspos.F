#include "geant321/pilot.h"
      SUBROUTINE GSPOS(CHNAME,NR,CHMOTH,X,Y,Z,IROT,CHONLY)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *      PLACE A COPY OF VOLUME 'CHNAME' WITH USER NUMBER 'NUMBER'   *
C.    *      INSIDE 'CHMOTH'                                           *
C.    *                                                                *
C.    *          JVO=POINTER TO CHMOTH VOLUME                          *
C.    *          JIN=POINTER TO THE COPY 'CHNAME','NUMBER'               *
C.    *          JIN=LQ(JVO-IN)                                        *
C.    *                                                                *
C.    *             Q(JIN+1)=NENTRY                                    *
C.    *             Q(JIN+2)=VOLUME NUMBER                             *
C.    *             Q(JIN+3)=USER NUMBER                               *
C.    *             Q(JIN+4)=IROT                                      *
C.    *             Q(JIN+5)=X                                         *
C.    *             Q(JIN+6)=Y                                         *
C.    *             Q(JIN+7)=Z                                         *
C.    *             Q(JIN+8)=ONLY                                      *
C.    *                                                                *
C.    *    ==>Called by : <USER>                                       *
C.    *         Authors R.Brun,  A.McPherson  *********                *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcnum.inc"
      CHARACTER*4 CHNAME,CHMOTH,CHONLY
      INTEGER     IPLACE
      COMMON / NUMPLA / IPLACE
C.
C.    ------------------------------------------------------------------
C.
C              CHECK IF VOLUME MASTER BANK EXISTS
C
      IF(JVOLUM.GT.0)GO TO 10
      WRITE(CHMAIL,1000)
      CALL GMAIL(0,0)
      GO TO 99
C
C              CHECK IF CHMOTH VOLUME EXISTS
C
  10  CALL GLOOK(CHMOTH,IQ(JVOLUM+1),NVOLUM,IVO)
      IF(IVO.GT.0)GO TO 20
      WRITE(CHMAIL,2000)CHMOTH
      CALL GMAIL(0,0)
      GO TO 99
C
C              CHECK IF CHNAME VOLUME EXISTS
C
  20  CALL GLOOK(CHNAME,IQ(JVOLUM+1),NVOLUM,IN)
      IF(IN.GT.0)GO TO 30
      WRITE(CHMAIL,2000)CHNAME
      CALL GMAIL(0,0)
      GO TO 99
C
C              CHECK IF ROTATION MATRIX EXISTS
C
  30  IF(IROT.LE.0)GO TO 50
      IF(JROTM.GT.0)GO TO 40
      WRITE(CHMAIL,3000)IROT
      CALL GMAIL(0,0)
      GO TO 99
  40  IF(LQ(JROTM-IROT).GT.0)GO TO 50
      WRITE(CHMAIL,3000)IROT
      CALL GMAIL(0,0)
      GO TO 99
C
C              CHECK IF MOTHER IS NOT DIVIDED
C
  50  JVO=LQ(JVOLUM-IVO)
      ICOPY=1
      NIN=Q(JVO+3)
      IF(NIN.GE.0)GO TO 60
      WRITE(CHMAIL,4000)CHMOTH
      CALL GMAIL(0,0)
      GO TO 99
C
C              CHECK IF ('NAME',NUMBER') EXISTS
C
  60  IF(NIN.EQ.0)GO TO 80
      DO 70 I=1,NIN
      JIN=LQ(JVO-I)
      IF(Q(JIN+2).NE.IN)GO TO 70
      IF(Q(JIN+3).NE.NR)GO TO 70
      WRITE(CHMAIL,5000)CHNAME,NR
      CALL GMAIL(0,0)
      GO TO 90
  70  CONTINUE
      ICOPY=NIN+1
C
C               CREATE BANK FOR THAT COPY
C
  80  NINL=IQ(JVO-2)
      IF(ICOPY.GT.NINL)CALL MZPUSH(IXCONS,JVO,50,0,'I')
      CALL MZBOOK(IXCONS,JIN,JVO,-ICOPY,'VOPO',1,1,8,3,0)
      IF(IEOTRI.NE.0)GO TO 95
      IQ(JIN-5)=100*IVO+ICOPY
      Q(JVO+3)=Q(JVO+3)+1
C
C               NOW STORE PARAMETERS INTO BANK AREA
C
      IPLACE = IPLACE+1
      WRITE(6,100) IPLACE, CHNAME, NR, CHMOTH, X, Y, Z, IROT, CHONLY

 100  FORMAT('aleph_geometry.add_placement( { ',
     &  '  "index":'      I5,
     &  ', "name": "',    (A), '"',
     &  ', "copy":',      I5,
     &  ', "mother": "',  (A), '"',
     &  ', "x":',         F10.4,
     &  ', "y":',         F10.4,
     &  ', "z":',         F10.4,
     &  ', "rotation":',  I5,
     &  ', "only": "',    (A), '"',
     &  ', "params": [] } )' )

  90  Q(JIN+2)=IN
      Q(JIN+3)=NR
      Q(JIN+4)=IROT
      Q(JIN+5)=X
      Q(JIN+6)=Y
      Q(JIN+7)=Z
      IF(CHONLY.EQ.'ONLY')Q(JIN+8)=1.
      GO TO 99
C
C              NOT ENOUGH SPACE
C
  95  WRITE(CHMAIL,6000)CHNAME,NR,CHMOTH
      CALL GMAIL(0,0)
C
 1000 FORMAT(' ***** GSPOS called and no volumes defined')
 2000 FORMAT(' ***** GSPOS volume ',A4,' does not exist')
 3000 FORMAT(' ***** GSPOS rotation matrix',I5,' does not exist')
 4000 FORMAT(' ***** GSPOS mother ',A4,' already divided')
 5000 FORMAT(' ***** GSPOS copy ',A4,' number ',I5,
     +       ' already created in ',A4,' *****')
 6000 FORMAT(' ***** GSPOS not enough space to store copy ',A4,
     +       ' number ',I5,' in ',A4,' *****')
  99  END
