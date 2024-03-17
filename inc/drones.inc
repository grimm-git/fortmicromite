
' Code to handle drones that fly around

CONST DRONES_MAX=32
CONST DRONES_SPEED=160

DIM Integer Drone.W=7
DIM Integer Drone.H=8

DIM Integer Drones.next
DIM Float   Drones.X(DRONES_MAX)
DIM Float   Drones.Y(DRONES_MAX)
DIM Float   Drones.minX(DRONES_MAX)
DIM Float   Drones.maxX(DRONES_MAX)
DIM Float   Drones.dX(DRONES_MAX)

sub Drones.init()
  Drones.next=0
end sub

' Set a drone in the map
' y% = y-coordinate of drone
' xmin%, xmax% = movement area of drone in X
sub Drones.set(y%,xmin%,xmax%)
  LOCAL Integer idx
  
  idx=Drones.next
  Drones.X(idx)=xmin%+(xmax%-xmin%)*RND
  Drones.Y(idx)=y%-Drone.H/2
  Drones.minX(idx)=xmin%
  Drones.maxX(idx)=xmax%
  Drones.dX(idx)=choice(RND<0.5,-1,1)
  if idx<DRONES_MAX then inc Drones.next
end sub

sub Drones.draw()
  STATIC Float interval=TIMER,cnt=0
  LOCAL Integer n,x,action,idx
  LOCAL Float dist

  if TIMER-interval>DRONES_SPEED then interval=TIMER:action=1

  for n=0 to Drones.next-1
    markOnRadar rnX(Drones.X(n))*Radar.Sx,Drones.Y(n)*Radar.Sy

    if action=1 then
      inc Drones.X(n),Drones.dX(n)
      if Drones.X(n)>Drones.maxX(n) or Drones.X(n)<Drones.minX(n) then
        Drones.dX(n)=-1*Drones.dX(n)
      endif
    endif

    if isVisible%(Drones.X(n),Drones.Y(n)) then
      Drones.toScreen Drones.X(n),Drones.Y(n)
    endif
  next
end sub

sub Drones.toScreen(x!,y!)
  LOCAL Float _x=x!,_y=y!

  _x=Screen.VPx+pfxToVPx!(x!)-Drone.W/2
  _y=Screen.VPy+pfyToVPy!(y!)-Drone.H/2
  blit 245,84,_x,_y,Drone.W,Drone.H,PAGE_TILESET,&B100
end sub

function Drones.isHit(x!,y!,w%,h%) as Integer
  LOCAL Float _x,_y
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer t

  Drones.isHit=-1
  for t=0 to Drones.next-1  
    _x=rnX(x!):_y=y!
    x1=rnX(Drones.X(t)-Drone.W/2):x2=x1+Drone.W
    y1=Drones.Y(t)-Drone.H/2:y2=y1+Drone.H
    if not (_x+w%<x1 or _x>x2 or _y+h%<y1 or _y>y2) then
      Drones.isHit=t
      exit for
    endif
  next
end function

sub Drones.destroy(t%)
  playSample 15,22050
  Drones.copy t%
end sub

sub Drones.copy(idxTo%)
  LOCAL Integer idxFrom

  idxFrom=Drones.next-1
  Drones.X(idxTo%)=Drones.X(idxFrom)
  Drones.Y(idxTo%)=Drones.Y(idxFrom)
  Drones.minX(idxTo%)=Drones.minX(idxFrom)
  Drones.maxX(idxTo%)=Drones.maxX(idxFrom)
  Drones.dX(idxTo%)=Drones.dX(idxFrom)
  inc Drones.next,-1
end sub


