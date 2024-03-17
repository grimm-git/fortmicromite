' Code to handle enimy tanks

CONST TANK_MAX=10
CONST TANK_SPEED=100
CONST TANK_FIRERATE=2000
CONST TANK_NUMSHOTS=2
CONST TANK_SHOTLIFE=200

DIM Integer Tank.W=30
DIM Integer Tank.H=18

DIM Integer Tanks.active(TANK_MAX)
DIM Float   Tanks.X(TANK_MAX)
DIM Float   Tanks.dX(TANK_MAX)
DIM Float   Tanks.Y(TANK_MAX)
DIM Float   Tanks.minX(TANK_MAX)
DIM Float   Tanks.maxX(TANK_MAX)
DIM Integer Tanks.dir(TANK_MAX)
DIM Integer Tanks.seq(TANK_MAX)
DIM Integer Tanks.idx(TANK_MAX)
DIM Integer Tanks.Shots.life(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.X0(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.Y0(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.V0(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.beta(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.t(TANK_MAX,TANK_NUMSHOTS)
DIM Float   Tanks.Shots.dt(TANK_MAX,TANK_NUMSHOTS)

sub Tanks.init()
  LOCAL Integer t,n

  for t=0 to TANK_MAX-1
    Tanks.active(t)=0
    for n=0 to TANK_NUMSHOTS-1
      Tanks.Shots.life(t,n)=0
    next
  next
end sub

' Set one tank in the map and activate it
' x%,y% = map block to put the tank on (top)
' xmin%, xmax% = movement area of tank 
sub Tanks.set(y%,x%,xmin%,xmax%)
  LOCAL Integer t,n
  LOCAL Float r=RND

  for t=0 to TANK_MAX-1
    if Tanks.active(t)=0 then
      Tanks.active(t)=1
      Tanks.X(t)=x%*MAPTILESIZE+(MAPTILESIZE-Tank.W)/2
      Tanks.Y(t)=y%*MAPTILESIZE-Tank.H/2+2
      Tanks.minX(t)=xmin%*MAPTILESIZE
      Tanks.maxX(t)=(xmax%+1)*MAPTILESIZE
      Tanks.dX(t)=choice(r<0.5,-1,1)
      Tanks.dir(t)=choice(r<0.5,&B100,&B101)
      Tanks.seq(t)=0
      Tanks.idx(t)=0

      for n=0 to TANK_NUMSHOTS-1
        Tanks.Shots.life(t,n)=0
      next

      exit sub
    endif
  next
end sub

sub Tanks.draw()
  STATIC Float interval=TIMER
  STATIC Float nextShot=TIMER
  LOCAL Float x
  LOCAL Integer n,action,dist,idx,fire
  LOCAL Integer sndON

  if (TIMER-interval>TANK_SPEED) then interval=TIMER:action=1
  if (TIMER-nextShot>TANK_FIRERATE) then nextShot=TIMER:fire=1

  sndON=0
  for n=0 to TANK_MAX-1
    if Tanks.active(n)=0 then continue for

    markOnRadar Tanks.X(n)*Radar.Sx,(Tanks.Y(n)+Tank.H/4)*Radar.Sy

    if action then
      x=rnX(Tanks.X(n)+Tanks.dX(n))
      if x>Tanks.maxX(n) or x<Tanks.minX(n) then Tanks.dX(n)=Tanks.dX(n)*-1
      Tanks.X(n)=x
    endif

    if isVisible%(Tanks.X(n),Tanks.Y(n)) then
      if action then
        dist=distanceX(x+Tank.W/2,Heli.X(0)+Heli.W/2)
        Tanks.dir(n)=choice(dist<0,&B100,&B101)
        Tanks.idx(n)=(Tanks.idx(n)+1) AND 1
        Tanks.seq(n)=choice(abs(dist)<50,1,0)
      endif

      sndON=1
      idx=2*Tanks.seq(n)+Tanks.idx(n)
      Tanks.toScreen n,idx,Tanks.X(n),Tanks.Y(n)
      if fire=1 then Tanks.fire n
    endif
  next
  Tanks.Sound sndON
end sub

sub Tanks.toScreen(t%,idx%,x!,y!)
  LOCAL Float _x=x!,_y=y!

  _x=Screen.VPx+pfxToVPx!(x!)-Tank.W/2
  _y=Screen.VPy+pfyToVPy!(y!)-Tank.H/2
  blit idx%*Tank.W,175,_x,_y,Tank.W,Tank.H,PAGE_SPRITES,Tanks.dir(t%)
end sub

sub Tanks.fire(t%) 
  LOCAL Integer s,x
  LOCAL Float dx,dy,a
  LOCAL Float xs,ys

  if Game.State<>STATE_GAME and Game.State<>STATE_DEMO then exit sub

  for s=0 to TANK_NUMSHOTS-1
    if (Tanks.Shots.life(t%,s)=0) then
      xs=distanceX(Heli.X(0),Tanks.X(t%))
      ys=Tanks.Y(t%)-Heli.Y(0)

      if ys>0 then
        Tanks.Shots.life(t%,s)=TANK_SHOTLIFE
        Tanks.Shots.X0(t%,s)=Tanks.X(t%)
        Tanks.Shots.Y0(t%,s)=Tanks.Y(t%)
        Tanks.Shots.beta(t%,s)=atan2(2*ys,xs)
        Tanks.Shots.V0(t%,s)=xs*sqr(2*ys)/ys/2/cos(Tanks.Shots.beta(t%,s))
        Tanks.Shots.dt(t%,s)=0.2
        Tanks.Shots.t(t%,s)=0
        PlaySample 6,22050
        exit for
      endif
    endif
  next
end sub

sub Tanks.drawShot(x!,y!)
  LOCAL Float _x,_y

  _x=Screen.VPx+pfxToVPx!(x!)
  _y=Screen.VPy+pfyToVPy!(y!)
  pixel _x,_y,map(WHITE)
  pixel _x,_y+1,map(WHITE)
  pixel _x+1,_y,map(WHITE)
  pixel _x+1,_y+1,map(WHITE)
end sub

sub Tanks.moveAndDrawShots()
  LOCAL Float _x,_y
  LOCAL Integer r,s,n
  LOCAL Float x,y,t,b,v0

  for r=0 to TANK_MAX-1
    for s=0 to TANK_NUMSHOTS-1
      if Tanks.Shots.life(r,s)>0 then
        Tanks.Shots.life(r,s)=Tanks.Shots.life(r,s)-1

        inc Tanks.Shots.t(r,s),Tanks.Shots.dt(r,s)
        t=Tanks.Shots.t(r,s)
        b=Tanks.Shots.beta(r,s)
        v0=Tanks.Shots.V0(r,s)
        x=v0*t*cos(b)          'must be calculated seperately
        y=v0*t*sin(b)-0.5*t*t  'before adding to the coordinates

        _x=rnX(Tanks.Shots.X0(r,s)+x)
        _y=Tanks.Shots.Y0(r,s)-y

        if _y<0 then Tanks.Shots.life(r,s)=0:continue for
        if isVisible%(_x,_y) then Tanks.drawShot(_x,_y)
        if Map.collision%(_x,_y) then
          Tanks.Shots.life(r,s)=0
        else
          for n=0 to HELI_MAX-1
            if Heli.isHit%(n,_x,_y) then
              Tanks.Shots.life(r,s)=0
              Heli.destroy n
            endif
          next
        endif
      endif
    next
  next
end sub

function Tanks.isHit(x!,y!) as Integer
  LOCAL Float _x,_y
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer t

  Tanks.isHit=-1
  for t=0 to TANK_MAX-1  
    if Tanks.active(t)=0 then continue for

    _x=rnX(x!):_y=y!
    x1=Tanks.X(t)-Tank.W/2:x2=x1+Tank.W
    y1=Tanks.Y(t)-Tank.H/2:y2=y1+Tank.H

    if x1<_x and _x<x2 and y1<_y and _y<y2 then
      Tanks.isHit=t
      exit function
    endif
  next
end function

sub Tanks.destroy(t%)
  Explosion.add Tanks.X(t%),Tanks.Y(t%)
  Tanks.active(t%)=0
end sub

sub Tanks.Sound arg%
  STATIC Float Interval=TIMER
  STATIC Float limit=2000
  STATIC Integer state=0 '=off

  if (state=0 and arg%=1) then
    state=1 '=on
    playSample 12,22050,2
    Interval=TIMER
    limit=2000

  elseif (state=1 and arg%=0) then
    state=0 '=off
    playSample 13,22050,2

  else if (state=1 and TIMER-Interval>limit) then
    playSample 5,22050,2
    Interval=TIMER
    limit=5000
  endif
end sub



