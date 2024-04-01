' Game objects
' Code by "Matthias Grimm"

CONST STATE_INTRO=0
CONST STATE_DEMO=1
CONST STATE_GAME=2
CONST STATE_DEAD=3
CONST STATE_PLAYER=4
CONST STATE_LEVEL=5
CONST STATE_OVER=6
CONST STATE_CUTSCENE=7
CONST STATE_VICTORY=8

CONST PAGE_DISPLAY=0
CONST PAGE_BUFFER=1
CONST PAGE_SPRITES=2
CONST PAGE_DASHBRD=3
CONST PAGE_TILESET=10
CONST PAGE_RADAR=11

CONST REQ_FUELING=0  'request
CONST REQ_NEXTLVL=1
CONST REQ_SPAWNFOE=2
CONST REQ_SPAWNTANKS=3
CONST REQ_SHOWSTATUS=4
CONST REQ_PORTAL=5
CONST REQ_REACTOREX=6

CONST ACQ_FUELING=16  'acknowledge
CONST ACQ_NEXTLVL=17
CONST ACQ_SPAWNFOE=18
CONST ACQ_SPAWNTANKS=19
CONST ACQ_PORTAL=20
CONST ACQ_REACTOREX=21


DIM Float frameTimer

DIM Integer Game.State      ' Default=STATE_INTRO
DIM Integer Game.Requests   ' bit field
DIM Float   Game.frameTime=1
DIM Float   Game.updateTime
DIM Integer Game.Controls   ' 0=keyboard, 1=Nunchuk
DIM Integer Game.Level      ' levels start with 1
DIM String  Game.StatusMsg$
DIM Float   Game.StatusTime
DIM Integer Game.Reactor
DIM Integer Game.ReactorHits

sub Game.init()
  Controls.init
  Heli.init
  Explosion.init
  cmap.init
end sub

' Page Setup
' 0 : current screen page
' 1 : double buffer page
' 2 : helicopter sprites
' 3 : dashboard
'10 : level tile set
sub Game.loadAssets()
  LOCAL Integer n

  load data "img/sprites.raw", MM.INFO(page address PAGE_SPRITES)
  load font PWD$+"/apocalypse.fnt"
  cmap.load "img/palette.cmap"
  cmap.activate

  PAGE write PAGE_DASHBRD:cls:Game.drawDash
  PAGE write PAGE_BUFFER
end sub

sub Game.new
  Humans.init
  Tanks.init
  Drones.init
  Foe.init
  Trigger.init

  Game.State=STATE_INTRO
  Game.Requests=0
  Game.Level=1
  Game.Reactor=1
  Game.ReactorHits=0
end sub

sub Game.clrScreen
  LOCAL Integer ox

  frameTimer=TIMER

  if Playfield.VPx>(Playfield.W-VP.W) then ox=Playfield.VPx-Playfield.W+VP.W
  blit Playfield.VPx,Playfield.VPy,Screen.VPx,Screen.VPy,VP.W-ox,VP.H,FRAMEBUFFER,0
  if ox>0 then blit 0,Playfield.VPy,Screen.VPx+VP.W-ox,Screen.VPy,ox,VP.H,FRAMEBUFFER,0
end sub

sub Game.draw
  STATIC Float timer1=TIMER, interval1=10
  STATIC Integer blast
  LOCAL Integer foe,t
  LOCAL Float col
  LOCAL Float _x,_y

  if Game.State=STATE_GAME or Game.State=STATE_DEMO then
    if tst(Game.Requests,REQ_FUELING) then
      clr Game.Requests,REQ_FUELING
      set Game.Requests,ACQ_FUELING
      if TIMER-timer1>interval1 then
        if Player.Fuel<HELI_MAXFUEL then inc Player.Fuel else interval1=1000
        playSample 1,22400+4*Player.Fuel,2
        timer1=TIMER
      endif
    elseif tst(Game.Requests,ACQ_FUELING) then
      if TIMER-timer1>3000 then    ' delay restorage of fuel station for 3 seconds
        clr Game.Requests,ACQ_FUELING
        Map.resetFuelStation
        interval1=10
      endif
    endif

    if tst(Game.Requests,REQ_NEXTLVL) then
      Map.LevelExit
    elseif tst(Game.Requests,ACQ_NEXTLVL) then
      clr Game.Requests,ACQ_NEXTLVL
      inc Game.Level
      if Game.Level>LEVEL_CNT then
        Game.State=STATE_VICTORY
      else
        playSample 8,22050,1
        Level.next Game.Level
        Playfield.VPx=Player.SpawnPt(2)
        Playfield.VPy=Player.SpawnPt(3)
        Game.State=STATE_LEVEL
      endif
    endif

    if tst(Game.Requests,ACQ_SPAWNFOE) then
    elseif tst(Game.Requests,REQ_SPAWNFOE) then
      if Trigger.Req(Heli.trigger)=REQ_SPAWNFOE then
        foe=Trigger.Arg(Heli.trigger)
        Foe.spawn Level.Foe.X(foe),Level.Foe.Y(foe),foe
      endif
    endif

    if tst(Game.Requests,ACQ_SPAWNTANKS) then
      Trigger.rem REQ_SPAWNTANKS,0
      clr Game.Requests,REQ_SPAWNTANKS
      clr Game.Requests,ACQ_SPAWNTANKS
    elseif tst(Game.Requests,REQ_SPAWNTANKS) then
      if Game.Level=1 then
        Tanks.active(0)=1
        Tanks.active(1)=1
        Tanks.active(2)=1
        Tanks.active(3)=1
        set Game.Requests,ACQ_SPAWNTANKS
      endif
    endif

    if tst(Game.Requests,ACQ_REACTOREX) then
      if blast<64 then
        _x=Screen.VPx+pfxToVPx!(800):_y=Screen.VPy+pfyToVPy!(175)
        col=choice(blast AND 1,0,7)
        circle _x,_y,104,0,0.833,map(col),map(col)
        inc blast
      else      
        Particle.draw
        if sprite(N)=0 then
          explosion.add 800,175
          clr Game.Requests,REQ_REACTOREX
          clr Game.Requests,ACQ_REACTOREX
          sprite close all
          Game.Reactor=0
          Fx.powerOFF
          Map.putTile 78,49,17:Map.map(49,17)=78
          Map.putTile 79,50,17:Map.map(50,17)=79
        endif
      endif
    elseif tst(Game.Requests,REQ_REACTOREX) then
      set Game.Requests,ACQ_REACTOREX
      Particle.prepReactor
      Particle.show
    endif

    if tst(Game.Requests,ACQ_PORTAL) then
    elseif tst(Game.Requests,REQ_PORTAL) then
      set Game.Requests,ACQ_PORTAL
      Portal.State=STATE_IN
      Game.State=STATE_CUTSCENE
    endif

    if tst(Game.Requests,REQ_SHOWSTATUS) then
      prt -1,6,Game.StatusMsg$,map(PURPLE)
      if TIMER-Game.StatusTime>0 then clr Game.Requests,REQ_SHOWSTATUS
    endif

[TRG]  for t=0 to Trigger.next-1
[TRG]    _x=Screen.VPx+pfxToVPx!(Trigger.X(t))
[TRG]    _y=Screen.VPy+pfyToVPy!(Trigger.Y(t))
[TRG]    frame _x,_y,Trigger.W(t),Trigger.H(t)
[TRG]  next
  endif

  blit 0,0,0,0,Screen.W,Screen.VPy,PAGE_DASHBRD,0 'Dashboard
  Game.drawRadar
  Game.updateDash
 
  Heli.moveAndDrawShots
  Humans.draw
  Portal.draw
  Heli.draw
  Foe.draw
  Tanks.moveAndDrawShots
  Tanks.draw
  Drones.draw
  Fx.draw
  Explosion.draw
end sub

sub Game.drawDash
  prt 8,0,"SCORE",map(6)
  prt 0,1.5,"FUEL",map(10)
  prt 30,1.5,"BONUS",map(10)
  prt 11,1.5,":                =",map(7)
  prt 11,2.5,";                >",map(7)
  prt 11,3.5,"<                ?",map(7)
  prt 12,4.5,"NAVATRON",map(6)
  Game.updateDash
end sub

sub Game.updateDash
  STATIC Float tiFuel=TIMER
  STATIC Integer trgFuel

  prt 20,0,str$(Player.Score),map(5),12
  prt 0,2.5,str$(Player.Fuel),map(5),8
  prt 29,2.5,str$(Player.Bonus),map(5),10

  if TIMER-tiFuel>400 then
    tiFuel=TIMER
    trgFuel=(trgFuel+1) AND 1
    if Game.State=STATE_GAME then
      if Player.Bonus>0 then inc Player.Bonus,-1
      if NOT tst(Game.Requests,REQ_FUELING) then
        if Player.Fuel>0 then inc Player.Fuel,-1
      endif
    endif
  endif    
  if Player.Fuel<100 AND trgFuel=1 then prt 10,6,"LOW ON FUEL",map(PURPLE)
  if Player.Fuel<2 then inc Player.dY,corrFPS!(0.05)
end sub

sub Game.drawRadar
  LOCAL Integer ox,n
  LOCAL Float x,y

  x=Heli.X(0):y=Heli.Y(0)
  if x>Playfield.W-1 then x=x-PLayfield.W

  x=x*Radar.Sx-RP.W/2:ox=0:Radar.VPx=x
  if x<0 then x=x+Radar.W
  if x>(Radar.W-RP.W) then ox=x-Radar.W+RP.W

  y=int(y*Radar.Sy-RP.H/2)
  if y<0 then y=0
  if y>Radar.H-RP.H then y=Radar.H-RP.H
  Radar.VPy=y

  if tst(Game.Requests,ACQ_FUELING) or Game.Reactor=0 then inc y,RADAR.H

  blit x,y,Screen.RPx,Screen.RPy,RP.W-ox,RP.H,PAGE_RADAR,&B100
  if ox>0 then blit 0,y,Screen.RPx+RP.W-ox,Screen.RPy,ox,RP.H,PAGE_RADAR,&B100
end sub

sub Game.swapPage
  LOCAL tmp%

  Game.updateTime=TIMER-frameTimer
[FPS] Game.showFPS
  map set
  page copy PAGE_BUFFER to PAGE_DISPLAY,I
  Game.frameTime=TIMER-frameTimer
end sub

Function Game.ShowLevelIntro() as Integer
  STATIC Float exitTimer=TIMER

  if exitTimer=0 then exitTimer=TIMER
  if TIMER-exitTimer>3000 then Game.showLevelIntro=1:exitTimer=0:exit function

  prt 12,9,"ENTERING",map(choice(RND<0.5,Level.Color+8,Level.Color))
  prt 8,10,Level.Name,map(choice(RND<0.5,Level.Color+8,Level.Color))
  prt 11,12,"PILOT NO 1",map(choice(RND<0.5,7,Level.Color))

  Game.showLevelIntro=0
end function

function Game.showPlayer() as Integer
  STATIC Float exitTimer=TIMER

  if exitTimer=0 then exitTimer=TIMER
  if TIMER-exitTimer>3000 then Game.showPlayer=1:exitTimer=0:exit function
  prt 11,9,"PILOT NO "+str$(MAXLIVES-Player.lives+1),map(choice(RND<0.5,7,Level.Color))
  Game.showPlayer=0
end function

function Game.showOver() as Integer
  STATIC Float exitTimer=0

  if exitTimer=0 then exitTimer=TIMER:playSample 10,22050
  if TIMER-exitTimer>3000 then Game.showOver=1:exitTimer=0:exit function
  text VP.W/2,VP.H/2-9,txt$("GAME"),"C",8,2,map(0),-1
  text VP.W/2-1,VP.H/2-10,txt$("GAME"),"C",8,2,map(5),-1
  text VP.W/2,VP.H/2+10,txt$("OVER"),"C",8,2,map(0),-1
  text VP.W/2-1,VP.H/2+9,txt$("OVER"),"C",8,2,map(5),-1
  Game.showOver=0
end function

function Game.waitDeadTime(deadTime!) as Integer
  STATIC Float exitTimer=TIMER

  if exitTimer=0 then exitTimer=TIMER
  if TIMER-exitTimer>deadTime! then Game.waitDeadTime=1:exitTimer=0:exit function
  Game.waitDeadTime=0
end function

Sub Game.showFPS
  text 0,0,str$(int(1000/Game.frameTime)),"L",7,,,-1
  text VP.W,0,str$(int(Game.updateTime*10)/10,2,2)+"ms","R",7,,,-1
end Sub

sub Game.showMsg(msg$,timeVal%)
  Game.StatusMsg=msg$
  Game.StatusTime=TIMER+timeVal%
  set Game.Requests,REQ_SHOWSTATUS
end sub


