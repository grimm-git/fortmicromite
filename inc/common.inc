

' Gets the parent directory of 'f$', or the empty string if it does not have one.
Function getParent$(f$)
  LOCAL i%

  For i% = Len(f$) To 1 Step -1
    If InStr("/\", Mid$(f$, i%, 1)) > 0 Then Exit For
  Next i%

  If i% = 0 Then
    getParent$=""
  Else
    getParent$=Left$(f$, i% - 1)
  EndIf
End Function

function corrFPS!(value!) as Float
  LOCAL Float fps=1000/Game.frameTime

  if fps<1 then
    corrFPS!=value!
  else
    corrFPS!=75/fps*value!
  endif
end function

sub frame(x%,y%,w%,h%)
  line x%,y%,x%+w%-1,y%
  line x%+w%-1,y%,x%+w%-1,y%+h%-1
  line x%,y%,x%,y%+h%-1
  line x%,y%+h%-1,x%+w%-1,y%+h%-1
end sub

sub cross(x!,y!)
  line x!-15,y!,x!+15,y!
  line x!,y!-15,x!,y!+15
end sub

function pfxToVPx!(x!)
  LOCAL Float _x=x!

  if Playfield.VPx+VP.W>Playfield.W then
    inc _x,(Playfield.W-Playfield.VPx)
  else
    inc _x,-Playfield.VPx
  endif

  pfxToVPx!=rnX(_x)
end function

function pfyToVPy!(y!)
  pfyToVPy!=y!-Playfield.VPy
end function

function rnX(x!) as Float
  if x!>Playfield.W then
    rnX=x!-Playfield.W
  elseif x!<0 then
    rnX=x!+Playfield.W
  else
    rnX=x!
  endif
end function

function distanceX(pos1!, pos2!) as Float
  LOCAL Float dX=pos1!-pos2!

  if dX>Playfield.W/2 then
    inc dX,-Playfield.W
  elseif dX<-Playfield.W/2 then
    inc dX,Playfield.W
  endif
  distanceX=dX
end function

function isVisible%(x!,y!)
  LOCAL Float px=x!,py

  isVisible%=0
  px=pfxToVPx!(x!)
  py=pfyToVPy!(y!)
  if px>0 and px<VP.W and py>0 and py<VP.H then isVisible%=1
end function

function isOnRadar%(x!,y!)
  LOCAL Float px=x!,py

  if Radar.VPx+RP.W>Radar.W then
    px=px+Radar.W-Radar.VPx
  else
    px=px-Radar.VPx
  endif
  if px>Radar.W then px=px-Radar.W
  py=y!-Radar.VPy    

  isOnRadar%=0
  if px>0 and px<RP.W and py>0 and py<RP.H then isOnRadar%=1
end function

' Mark an object on the Radar
sub markOnRadar(x!,y!)
  LOCAL Float _x=x!,_y=y!

  if not isOnRadar%(_x,_y) then exit sub

  _x=_x-Radar.VPx:_y=_y-Radar.VPy
  if Radar.VPx+RP.W>Radar.W then _x=_x+Radar.W
  if _x>Radar.W then inc _x,-Radar.W

  pixel Screen.RPx+_x,Screen.RPy+_y,map(WHITE)   
end sub

sub set(arg%,bit%)
  arg%=arg% OR 1<<bit%
end sub

sub clr(arg%,bit%)
  arg%=arg% AND INV(1<<bit%)
end sub

function tst(arg%,bit%) AS Integer
  tst=(arg% AND 1<<bit%)
end function

