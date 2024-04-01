

CONST PORTAL_MAXSPEED=40

CONST STATE_NONE=0
CONST STATE_IN=1
CONST STATE_OPEN=2
CONST STATE_OUT=3
CONST STATE_CLOSE=4

DIM Integer Portal.State=STATE_NONE
DIM Float   Portal.TargetX=730
DIM Float   Portal.TargetY=110

sub Portal.draw()
  if Portal.State=STATE_NONE then exit sub

  select case Portal.State
    case STATE_IN: 
      Portal.flyin
    case STATE_OPEN:
      Playfield.VPx=800-VP.W/2
      Playfield.VPy=175-VP.H/2
      Player.X=Portal.TargetX-Playfield.VPx+8+Heli.W/2
      Player.Y=Portal.TargetY-Playfield.VPy+20
      Portal.open
    case STATE_OUT: 
      Portal.flyout
    case STATE_CLOSE:
      Portal.close
  end select
end sub

sub Portal.flyin()
  STATIC Float ivalTimer

  if TIMER-ivalTimer>PORTAL_MAXSPEED then
    ivalTimer=TIMER
    if Heli.ofsE<Heli.W then
      inc Heli.ofsE
    else
      Heli.ofsE=0
      Heli.ofsB=Heli.W
      Portal.State=STATE_OPEN
    endif
  endif
end sub

sub Portal.flyout()
  STATIC Float ivalTimer
  LOCAL Float _x,_y

  _x=Screen.VPx+pfxToVPx!(Portal.TargetX)
  _y=Screen.VPy+pfyToVPy!(Portal.TargetY)
  blit 304,144,_x,_y,16,48,PAGE_TILESET,&B100

  if TIMER-ivalTimer>PORTAL_MAXSPEED then
    ivalTimer=TIMER
    if Heli.ofsB>0 then
      inc Heli.ofsB,-1
    else
      Game.State=STATE_GAME
      Portal.State=STATE_CLOSE
    endif
  endif
end sub

sub Portal.open()
  STATIC Float ivalTimer
  STATIC Integer ofs
  LOCAL Float _x,_y

  _x=Screen.VPx+pfxToVPx!(Portal.TargetX)
  _y=Screen.VPy+pfyToVPy!(Portal.TargetY)
  blit 304,144,_x,_y,ofs,48,PAGE_TILESET,&B100
  blit 320-ofs,144,_x+ofs,_y,ofs,48,PAGE_TILESET,&B100

  if TIMER-ivalTimer>PORTAL_MAXSPEED then
    ivalTimer=TIMER
    if ofs<MAPTILESIZE/2 then
      inc ofs
    else
      ofs=0
      Heli.X(0)=rnX(Playfield.VPx+Player.X)
      Heli.Y(0)=Playfield.VPy+Player.Y
      Portal.State=STATE_OUT
    endif
  endif  
end sub

sub Portal.close()
  STATIC Float ivalTimer
  STATIC Integer ofs=MAPTILESIZE/2
  LOCAL Float _x,_y

  _x=Screen.VPx+pfxToVPx!(Portal.TargetX)
  _y=Screen.VPy+pfyToVPy!(Portal.TargetY)
  blit 304,144,_x,_y,ofs,48,PAGE_TILESET,&B100
  blit 320-ofs,144,_x+ofs,_y,ofs,48,PAGE_TILESET,&B100

  if TIMER-ivalTimer>PORTAL_MAXSPEED then
    ivalTimer=TIMER
    if ofs>0 then
      inc ofs,-1
    else
      ofs=MAPTILESIZE/2
      clr Game.Requests,ACQ_PORTAL
      clr Game.Requests,REQ_PORTAL
      Portal.State=STATE_NONE
    endif
  endif  
end sub


