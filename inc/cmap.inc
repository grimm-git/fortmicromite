

CONST BLACK=0
CONST RED=1
CONST GREEN=2
CONST BLUE=3
CONST PURPLE=4
CONST YELLOW=5
CONST CYAN=6
CONST WHITE=7
CONST DARKGRAY=8
CONST DARKRED=9
CONST DARKGREEN=10
CONST DARKBLUE=11
CONST DARKPURPLE=12
CONST DARKYELLOW=13
CONST DARKCYAN=14
CONST GRAY=15

DIM Integer cmap.clut(255)  ' custom color [24], R [16-23], G [8-15], B [0-7]
DIM Integer cmap.custom(255)
DIM Integer cmap.cmode

sub cmap.init()
  cmap.cmode=cmap.getCMode%()
  cmap.reset
end sub

sub cmap.reset()
  LOCAL Integer t

  map reset
  for t=0 to 255
    cmap.clut(t)=map(t)
    cmap.custom(t)=0
  next
end sub

sub cmap.load(name$)
  LOCAL buffer$,tmp$
  LOCAL parse%,R%,G%,B%,idx%
  LOCAL n%

  on error abort
  open name$ for input as #1
  if MM.ERRNO>0 then exit sub

  do
    line input #1, buffer$
    if buffer$="#" then parse%=1: continue do
    if parse%=1 then
      do
        idx%=instr(buffer$,"  ")
        if idx%>0 then buffer$=left$(buffer$,idx%-1)+mid$(buffer$,idx%+1)
      loop until idx%=0
      if left$(buffer$,1)=" " then buffer$=mid$(buffer$,2)

      R%=val(field$(buffer$,1," "))
      G%=val(field$(buffer$,2," "))
      B%=val(field$(buffer$,3," "))
      cmap.set(n%,RGB(R%,G%,B%))
      n%=n%+1
    endif
  loop while EOF(#1)=0
  close #1
end sub

sub cmap.set(reg%,color%)
  cmap.clut(reg%)=color%
  cmap.custom(reg%)=1
  map(reg%)=color%
end sub

sub cmap.setRGB(reg%,R%,G%,B%)
  cmap.set(reg%,RGB(R%,G%,B%))
end sub

function cmap.get(reg%) as Integer
  cmap.get%=cmap.clut(reg%)
end function

function cmap.isCustom(reg%) as Integer
  cmap.isCustom%=cmap.custom(reg%)
end function

sub cmap.activate
  if cmap.cmode<>8 then exit sub

  map set
end sub

sub cmap.reactivate
  LOCAL Integer t
  
  if cmap.cmode<>8 then exit sub

  for t=0 to 255
    if cmap.custom(t)=1 then map(t)=cmap.clut(t)
  next
  map set
end sub

function cmap.getCMode%()
  LOCAL cm$=mid$(str$(MM.INFO(MODE)),3)
  cmap.getCMode%=val(cm$)
end function










