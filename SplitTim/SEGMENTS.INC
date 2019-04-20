;*****************************************************************************
;**
;** Segments
;**

               .286

TSR_CODE       segment  WORD PUBLIC READONLY 'CODE' 
TSR_CODE       ends
TSR_DATA       segment  WORD PUBLIC 'DATA'
TSR_DATA       ends
TSR_TMP        segment  WORD PUBLIC 'TMP'
TSR_TMP        ends
STACK_TMP      segment  WORD PUBLIC STACK 'STACK'
STACK_TMP      ends
               
TSR	          group    TSR_CODE, TSR_DATA, TSR_TMP, STACK_TMP

               assume  CS: TSR, DS: TSR, ES: NOTHING, SS: NOTHING

