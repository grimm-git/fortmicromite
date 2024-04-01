

CONST HELI_MAX=2
CONST HELI_MAXSHOTS=10
CONST HELI_MAXFUEL=1000
CONST HELI_SHOTLIFE=50
CONST HELI_SPEEDFLIGHT=50
CONST HELI_SPEEDLANDED=100

DIM Integer Heli.W=45
DIM Integer Heli.H=25
DIM Float   Heli.Core.X(4)= (-18,-20,-5,-14,-13)  'relative to Heli's center
DIM Float   Heli.Core.Y(4)= ( -5, -1,-1, -1, -5)
DIM Float   Heli.Core.W(4)= ( 32, 33,10, 33, 32)
DIM Float   Heli.Core.H(4)= ( 15, 11,11, 11, 15)
DIM Integer Heli.ofsB
DIM Integer Heli.ofsE

DIM Integer Heli.Trigger    'last trigger event

DIM Float   Heli.X(HELI_MAX)     'Playfield coordinates
DIM Float   Heli.Y(HELI_MAX)
DIM Integer Heli.State(HELI_MAX)
DIM Integer Heli.trs(HELI_MAX)
DIM Integer Heli.seq(HELI_MAX)
DIM Integer Heli.dir(HELI_MAX)
DIM Integer Heli.idx(HELI_MAX)
DIM Integer Heli.Shots.life(HELI_MAX,HELI_MAXSHOTS)
DIM Float   Heli.Shots.X(HELI_MAX,HELI_MAXSHOTS) 
DIM Float   Heli.Shots.Y(HELI_MAX,HELI_MAXSHOTS)
DIM Float   Heli.Shots.dX(HELI_MAX,HELI_MAXSHOTS)
DIM Float   Heli.Shots.dY(HELI_MAX,HELI_MAXSHOTS)
DIM Integer Heli.landed(HELI_MAX)
DIM Float   Heli.Speed(HELI_MAX)
DIM Float   Heli.tiAnim(HELI_MAX)
DIM Float   Heli.tiRotor(HELI_MAX)


sub Heli.init()
  LOCAL Integer t

  for t=0 to HELI_MAX-1
    Heli.reset t
    Heli.State(t)=0
  next
end sub

sub Heli.reset(no%,x!,y!)
  LOCAL Integer t

  Heli.X(no%)=x!
  Heli.Y(no%)=y!
  Heli.State(no%)=3
  Heli.trs(no%)=0 
  Heli.seq(no%)=0
  Heli.idx(no%)=0
  Heli.dir(no%)=&B100
  Heli.landed(no%)=0
  Heli.Speed(no%)=HELI_SPEEDFLIGHT
  Heli.ofsB=0
  Heli.ofsE=0

  for t=0 to HELI_MAXSHOTS-1
    Heli.Shots.life(no%,t)=0
  next
end sub

' States                                                     
'                        
' forward <-> hover <-> hover <-> hover <-> forward
'  left       left      nose      right      right
'
' State Name           Anim  Dir Transistions
'  0 -  Inactive
'  1 A  forward right   11    0  B:10
'  2 B  hover right      6    0  A:10
'  3 C  hover nose       1    0  B:5, D:5
'  4 D  hover left       6    1  E:10
'  5 E  forward left    11    1  D:10
sub Heli.setState(no%, newState%)
  LOCAL Integer oldState=Heli.State(no%)

  if (newState% <> oldState) then
    Heli.trs(no%)=0  ' default
    select case newState%
      case 1:
        Heli.seq(no%)=8
        Heli.dir(no%)=&B100
        if (oldState=2) then Heli.trs(no%)=25
      case 2:
        Heli.seq(no%)=4
        Heli.dir(no%)=&B100
        if (oldState=1) then Heli.trs(no%)=25
      case 3:
        Heli.seq(no%)=0
        Heli.dir(no%)=&B100
        if (oldState=2 or oldState=4) then Heli.trs(no%)=24
      case 4:
        Heli.seq(no%)=4
        Heli.dir(no%)=&B101
        if (oldState=5) then Heli.trs(no%)=25
      case 5:
        Heli.seq(no%)=8
        Heli.dir(no%)=&B101
        if (oldState=4) then Heli.trs(no%)=25
    end select
    Heli.State(no%)=newState%
  end if
end sub

sub Heli.toHover(no%)            '0 1 2 3 4 5
  LOCAL Integer hoverStates(5) = (0,2,2,3,4,4)

  if Heli.trs(no%)=0 then Heli.setState(no%,hoverStates(Heli.State(no%)))

  Player.dX=Player.dX*0.96:if abs(Player.dX)<0.001 then Player.dX=0  'Air drag
  Player.dY=Player.dY*0.90:if abs(Player.dY)<0.001 then Player.dY=0  'Air drag
  if Player.dY=0 then Player.dY=1/9.81 'Gravitation
  if Heli.landed(no%)=1 then Player.dY=0
end sub

sub Heli.toLeft(no%,enable%)    '0 1 2 3 4 5
  LOCAL Integer leftStates(5) = (0,2,3,4,5,5)
  LOCAL Integer state=Heli.State(no%)
  
  if state=3 and enable%=0 then exit sub
  if Heli.trs(no%)=0 then Heli.setState no%,leftStates(state)
end sub

sub Heli.toRight(no%,enable%)    '0 1 2 3 4 5
  LOCAL Integer rightStates(5) = (0,1,1,2,3,4)
  LOCAL Integer state=Heli.State(no%)
 
  if state=3 and enable%=0 then exit sub
  if Heli.trs(no%)=0 then Heli.setState no%,rightStates(state)
end sub

sub Heli.fire(no%)       '  1  2  3  4  5
  LOCAL Float offsetX(4)= (10,12,0,-12,-9)
  LOCAL Float offsetY(4)= ( 6, 4, 8, 4, 6)
  LOCAL Float fireDX(4) = ( 3, 3, 0,-3,-3)
  LOCAL Float fireDY(4) = (0.5,0, 3, 0,0.5)
  LOCAL Integer z,idx

  if Heli.State(no%)=0 then exit sub

  idx=Heli.State(no%)-1
  for z=0 to HELI_MAXSHOTS-1
    if Heli.Shots.life(no%,z)=0 then
      Heli.Shots.life(no%,z)=HELI_SHOTLIFE
      Heli.Shots.X(no%,z)=Heli.X(no%)+offsetX(idx)
      Heli.Shots.dX(no%,z)=fireDX(idx)
      Heli.Shots.Y(no%,z)=Heli.Y(no%)+offsetY(idx)
      Heli.Shots.dY(no%,z)=fireDY(idx)
      PlaySample 4,22050
      exit sub
    endif
  next
end sub

sub Heli.draw()
  LOCAL Integer t,c,r
  LOCAL Integer n,x,y,idx,ev

  for t=0 to HELI_MAX-1
    if Heli.State(t)=0 then continue for

    markOnRadar Heli.X(t)*Radar.Sx,Heli.Y(t)*Radar.Sy
    
    if isVisible%(Heli.X(t),Heli.Y(t)) then
      if TIMER-Heli.tiAnim(t)>Heli.Speed(t) then
        Heli.tiAnim(t)=TIMER  
        Heli.idx(t)=(Heli.idx(t)+1) AND 3
        if Heli.trs(t)>0 then Heli.trs(t)=0
      endif

      if TIMER-Heli.tiRotor(t)>Heli.Speed(t)+100 then
        Heli.tiRotor(t)=TIMER
        if Heli.landed(t)=1 then
          if (Heli.Speed(t) < HELI_SPEEDLANDED) then Heli.Speed(t)=Heli.Speed(t)+5
        else
          if (Heli.Speed(t) > HELI_SPEEDFLIGHT) then Heli.Speed(t)=Heli.Speed(t)-5
        endif
      endif
      Heli.Sound

      'Check for heli hits the ground
      idx=Heli.State(t)-1
      x=Heli.X(t)+Heli.Core.X(idx)
      y=Heli.Y(t)+Heli.Core.Y(idx)
      if Map.collision%(x,y,Heli.Core.W(idx),Heli.Core.H(idx)) then
[DBG] text 0,60,"Heli destroyed!","L",7
[INV] Heli.destroy(t)
      elseif Drones.isHit(x,y,Heli.Core.W(idx),Heli.Core.H(idx))<>-1 then
        Heli.destroy(t)
      endif

      'Check for player's heli triggered something
      if t=0 then
        ev=Trigger.isHit(x+Heli.Core.W(idx)/2,y+Heli.Core.H(idx)/2)
        if ev>-1 then Heli.Trigger=ev:set Game.Requests,Trigger.Req(ev)
      endif

      'Check for heli to heli collision
      for n=t+1 to HELI_MAX-1
        if Heli.State(n)=0 then continue for
        idx=Heli.State(n)-1
        x=Heli.X(n)+Heli.Core.X(idx)
        y=Heli.Y(n)+Heli.Core.Y(idx)
        if Heli.isHit%(t,x,y,Heli.Core.W(idx), Heli.Core.H(idx)) then
          Heli.destroy(n)
          Heli.destroy(t)
        endif
      next

      idx=choice(Heli.trs(t)>0,Heli.trs(t),Heli.seq(t)+Heli.idx(t))
      Heli.toScreen t,idx,Heli.X(t),Heli.Y(t)
    endif
  next
end sub

sub Heli.toScreen(t%,idx%,x!,y!)
  LOCAL Float _x=x!,_y=y!
  LOCAL Integer c,r,st,dx,dw

  if Heli.ofsE>0 then
    st=Heli.State(t%)-1
    dw=Heli.ofsE+(Heli.W-Heli.Core.W(st))/2-2
  elseif Heli.ofsB>0 then
    dw=Heli.ofsB
  endif

  if t%>0 then inc idx%,choice(idx%>23,2,12)
  c=idx% mod 4:r=int(idx%/4)

  _x=Screen.VPx+pfxToVPx!(x!)-Heli.W/2+Heli.ofsE
  _y=Screen.VPy+pfyToVPy!(y!)-Heli.H/2
  blit c*Heli.W+Heli.ofsB,r*Heli.H,_x,_y,Heli.W-dw,Heli.H,PAGE_SPRITES,Heli.dir(t%)

[HXY]  LOCAL Integer i=Heli.State(t%)-1
[HXY]  if i>=0 then
[HXY]    frame(_x+Heli.Core.X(i)+Heli.W/2,_y+Heli.Core.Y(i)+Heli.H/2,Heli.Core.W(i),Heli.Core.H(i))
[HXY]    text _x,_y+Heli.Core.Y(i)+Heli.H/2,str$(x!,4,2),"R",7,,,-1
[HXY]    text _x+Heli.Core.X(i)+Heli.W/2,_y,str$(y!,4,2),"L",7,,,-1
[HXY]  endif
end sub

sub Heli.drawShot(x!,y!,hit%)
  LOCAL Float _x,_y

  _x=Screen.VPx+pfxToVPx!(x!)
  _y=Screen.VPy+pfyToVPy!(y!)
  pixel _x,_y,map(WHITE)
  pixel _x,_y+1,map(WHITE)
  if hit% then
    pixel _x-1,_y-1,map(WHITE)
    pixel _x+1,_y-1,map(WHITE)
    pixel _x-2,_y-2,map(WHITE)
    pixel _x+2,_y-2,map(WHITE)
  endif
end sub

sub Heli.moveAndDrawShots()
  LOCAL Float _x,_y
  LOCAL Integer t,s,n,hit

  for t=0 to HELI_MAX-1
    for s=0 to HELI_MAXSHOTS-1
      if Heli.Shots.life(t,s)>0 then
        Heli.Shots.life(t,s)=Heli.Shots.life(t,s)-1
        inc Heli.Shots.X(t,s),corrFPS!(Heli.Shots.dX(t,s))
        inc Heli.Shots.Y(t,s),corrFPS!(Heli.Shots.dY(t,s))
        _x=Heli.Shots.X(t,s):_y=Heli.Shots.Y(t,s)

        hit=Map.collision%(_x,_y)
        if isVisible%(_x,_y) then Heli.drawShot(_x,_y,hit)
        if hit then
          Heli.Shots.life(t,s)=0
        else          
          for n=0 to HELI_MAX-1
            if n<>t and Heli.isHit%(n,_x,_y) then
              Heli.destroy n
              Heli.Shots.life(t,s)=0
              continue for
            endif
          next

          n=Tanks.isHit(_x,_y)
          if n>=0 then Tanks.destroy n:Heli.Shots.life(t,s)=0:continue for
          n=Drones.isHit(_x,_y)
          if n>=0 then Drones.destroy n:Heli.Shots.life(t,s)=0
        endif
      endif
    next
  next
end sub

function Heli.isHit%(no%,x!,y!,w%,h%)
  LOCAL Integer _x,_y
  LOCAL Integer x1,x2,y1,y2,idx

  Heli.isHit%=0
  if Heli.State(no%)=0 then exit function

  _x=rnX(x!):_y=y!

  idx=Heli.State(no%)-1
  x1=rnX(Heli.X(no%)+Heli.Core.X(idx)):x2=x1+Heli.Core.W(idx)
  y1=Heli.Y(no%)+Heli.Core.Y(idx):y2=y1+Heli.Core.H(idx)
  if not (_x+w%<x1 or _x>x2 or _y+h%<y1 or _y>y2) then Heli.isHit%=1
end function

sub Heli.destroy(no%)
  LOCAL Integer x,y,idx

  idx=Heli.State(no%)-1
  x=rnX(Heli.X(no%)+Heli.Core.X(idx)+Heli.Core.W(idx)/2)
  y=Heli.Y(no%)+Heli.Core.Y(idx)+Heli.Core.H(idx)/2
  Explosion.add x,y,1
  Heli.State(no%)=0
  Heli.Speed(no%)=0
end sub

sub Heli.Sound()
  STATIC Float Interval=TIMER
  STATIC Integer oldSpeed
  LOCAL Float factor,maxInterval
  LOCAL Integer t,n

  for n=0 to HELI_MAX-1
    if Heli.State(n)>0 then t=n:exit for
  next  

  maxInterval=choice(Heli.Speed(t)=HELI_SPEEDLANDED,9900,5000)
  factor=1-(Heli.Speed(t)-HELI_SPEEDFLIGHT)/(HELI_SPEEDLANDED-HELI_SPEEDFLIGHT)
  if (Heli.Speed(t)=0) then
    playSample 14,22050,1  ' stop sound

  elseif TIMER-Interval > maxInterval or Heli.Speed(t)<>oldSpeed then
    Interval=TIMER
    playSample 3,11025+11025*factor,1
    oldSpeed=Heli.Speed(t)
  endif
end sub



