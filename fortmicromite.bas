' (c) 2024, Program by "Matthias Grimm"
' This program was published under the GPL3 license

OPTION BASE 0
OPTION EXPLICIT
OPTION DEFAULT NONE

CONST PWD$=getParent$(MM.INFO(current))
CONST DEBUG=0
CONST DEBUG_MAP=0
CONST DEBUG_HELI=0
CONST DEBUG_RECORD=0

#Include "inc/common.inc"
#Include "inc/game.inc"
#Include "inc/cmap.inc"
#Include "inc/map.inc"
#Include "inc/explosion.inc"
#include "inc/sound.inc"
#Include "inc/controls.inc"
#Include "inc/text.inc"
#Include "inc/intro.inc"
#Include "inc/heli.inc"
#Include "inc/events.inc"
#Include "inc/level.inc"
#Include "inc/tanks.inc"
#Include "inc/humans.inc"
#Include "inc/drones.inc"
#Include "inc/foe.inc"
#Include "inc/trigger.inc"
#Include "inc/record.inc"

CONST MAXLIVES=5

DIM Float   Player.X    'Viewport coordinates
DIM Float   Player.Y    ' because that is the area for the player to be
DIM Float   Player.dX
DIM Float   Player.dY
DIM Float   Player.SpawnPt(3)  'x,y,VPx,VPy
DIM Integer Player.lives=MAXLIVES
DIM Integer Player.Score
DIM Integer Player.Fuel
DIM Integer Player.Bonus
DIM Integer Player.Rescued

mode 3,8   ' 320x200, 8bit color

DIM Integer Screen.W=MM.HRES
DIM Integer Screen.H=MM.VRES
DIM Float   Screen.VPx=0
DIM Float   Screen.VPy=48  '6*8
DIM Float   Screen.RPx=96
DIM Float   Screen.RPy=12

DIM Integer VP.W=Screen.W-Screen.VPx  ' ViewPort
DIM Integer VP.H=Screen.H-Screen.VPy
DIM Integer VP.PRw=120
DIM Integer VP.PRh=70
DIM Integer VP.PRx=(VP.W-VP.PRw)/2
DIM Integer VP.PRy=(VP.H-VP.PRh)/2-5

DIM Integer RP.W=128                  ' RadarPort
DIM Integer RP.H=24

DIM Integer Playfield.W=100*MAPTILESIZE
DIM Integer Playfield.H= 18*MAPTILESIZE
DIM Float   Playfield.VPx=(Playfield.W-VP.W)/2
DIM Float   Playfield.VPy=0

DIM Integer Radar.W=320
DIM Integer Radar.H=58
DIM Float   Radar.VPx=(Radar.W-RP.W)/2
DIM Float   Radar.VPy=0
DIM Float   Radar.Sx=Radar.W/Playfield.W
DIM Float   Radar.Sy=Radar.H/Playfield.H

Game.Init
Game.loadAssets

FRAMEBUFFER Create Playfield.W,Playfield.H

' ********************************************************************************
'                                Main Game Loop
' ********************************************************************************
DIM Integer key
DIM Integer lock.fire   ' single fire only
DIM Integer lock.turn
DIM Integer ctrl,ctrlTime
DIM Float   dX,dY

if DEBUG_RECORD then Record.open
do
  if Game.State=STATE_INTRO then
    Intro.Show
    do
      key=Intro.play()
    loop while key=0

    if key=27 or key=asc("q") or key=asc("Q") then goto GameEnd

    Game.new
    Level.next Game.Level
    Player.reset

    if key=22 then
      Player.spawn
      Controls.playback 1
      Game.State=STATE_DEMO
    else
      Controls.playback 0
      Game.State=STATE_LEVEL
    endif
  endif

  Game.clrScreen

  if Game.State=STATE_LEVEL then
    if Game.showLevelIntro() then
      Player.spawn
      Game.State=STATE_GAME
    endif
  endif

  if Game.State=STATE_DEAD then
    if Game.waitDeadTime(2000) then Game.State=STATE_PLAYER
  endif

  if Game.State=STATE_PLAYER then
    if Player.lives>0 then
      Playfield.VPx=Player.SpawnPt(2)
      Playfield.VPy=Player.SpawnPt(3)
      if Game.showPlayer() then
        Player.spawn
        Game.State=STATE_GAME
      endif
    else
      if Game.showOver() then
        Game.State=STATE_INTRO
      endif
    endif
  endif

  if Game.State=STATE_GAME or Game.State=STATE_DEMO then  
    Game.updateDash

    ctrl=Controls.read%()
    if DEBUG_RECORD then Record.write ctrl

    if (ctrl=0) then Heli.toHover(0)

    if (ctrl and 15)=0 then Heli.toHover(0)  ' joystick in center position
    if (ctrl and 3) =0 then lock.turn=0

    if (ctrl and 1) > 0 then   ' right
      if Player.dX<1.5 then inc Player.dX,0.04

      Heli.toRight(0,lock.turn)
      ctrlTime=Controls.getTime!(CTRL_RIGHT)
      if (ctrlTime>10) then lock.turn=1
    endif

    if (ctrl and 2) > 0 then   ' left
      if Player.dX>-1.5 then inc Player.dX,-0.04

      Heli.toLeft(0,lock.turn)
      ctrlTime=Controls.getTime!(CTRL_LEFT)
      if (ctrlTime>10) then lock.turn=1
    endif

    if (ctrl and 4) > 0 then   ' up
      if Player.dY>-1.5 then inc Player.dY,-0.04
    endif

    if (ctrl and 8) > 0 then   ' down
      if Player.dY<1.5 then inc Player.dY,0.04
    endif

    if (ctrl and 16) > 0 then  ' fire
      if (lock.fire=0) then
        Heli.fire
        lock.fire=1
      endif
    else
      lock.fire=0
    endif

    dX=corrFPS!(Player.dX)
    dY=corrFPS!(Player.dY)

    ' Player and Playfield Movements
    inc Player.X,dX 'Player.dX
    if Player.X<VP.PRx or Player.X>VP.PRx+VP.PRw then
      Player.X=Player.X-dX 'Player.dX
      Playfield.VPx = Playfield.VPx+dX 'Player.dX
      if Playfield.VPx<VP.W then Playfield.VPx=Playfield.VPx+Playfield.W
      if Playfield.VPx>Playfield.W then Playfield.VPx=Playfield.VPx-Playfield.W
    endif

    inc Player.Y,dY 'Player.dY
    if Player.dY>0 and Heli.landed(0) then inc Player.Y,-dY 'Player.dY
    if Player.dY<0 and Heli.landed(0) then Heli.landed(0)=0
    if Player.dY>0 and Player.Y>VP.PRy+VP.PRh then
      Playfield.VPy=Playfield.VPy+dY 'Player.dY
      if Playfield.VPy<Playfield.H-VP.H then
        Player.Y=Player.Y-dY' Player.dY
      else
        Playfield.VPy=Playfield.H-VP.H
      endif
      if Player.Y>VP.H-Heli.H/2 then Player.Y=VP.H-Heli.H/2:Player.dY=0
    elseif Player.dY<0 and Player.Y<VP.PRy then
      Playfield.VPy=Playfield.VPy+dY 'Player.dY
      if (Playfield.VPy>0) then
        Player.Y=Player.Y-dY 'Player.dY
      else
        Playfield.VPy=0
      endif
      if Player.Y<Heli.H/2 then Player.Y=Heli.H/2:Player.dY=0
    endif

    'Viewport coordinates -> Playfield coordinates
    Heli.X(0)=rnX(Playfield.VPx+Player.X)
    Heli.Y(0)=Playfield.VPy+Player.Y
    if Heli.State(0)=0 then Player.destroy
  endif

  Game.draw  ' Draw all objects to screen

  key=Controls.readKey%()
  if key=asc("f") then Player.Fuel=1000
  if key=27 or key=asc("q") or key=asc("Q") then exit do
  if key = 147 then save image "screenshot.bmp" ' F3 for screen shot

  if DEBUG then Game.showFPS
  Game.swapPage
loop

GameEnd:
if DEBUG_RECORD then Record.close
mode 1
page write 0
print "Good Bye..."

sub Player.reset()
  Player.X=VP.W/3
  Player.Y=20
  Player.dX=0
  Player.dY=0
  Player.lives=MAXLIVES
  Player.Fuel=99
  Player.Bonus=9999
  Player.saveSpawnPt
end sub

sub Player.spawn()
  Player.X=Player.SpawnPt(0)
  Player.Y=Player.SpawnPt(1)
  Player.dX=0
  Player.dY=0

  Playfield.VPx=Player.SpawnPt(2)
  Playfield.VPy=Player.SpawnPt(3)

  Heli.reset(0,Player.X,PLayer.Y)
  Explosion.init()
end sub

sub Player.saveSpawnPt()
  Player.SpawnPt(0)=Player.X
  Player.SpawnPt(1)=Player.Y
  Player.SpawnPt(2)=Playfield.VPx
  Player.SpawnPt(3)=Playfield.VPy
end sub

sub Player.destroy()
  Player.lives=Player.lives-1
  Foe.init 'kill all remaing foes
  Game.State=choice(Game.State=STATE_DEMO,STATE_INTRO,STATE_DEAD)
end sub




