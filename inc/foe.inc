' Code for enemy helicopter
' the graphics is handled by heli.inc

CONST FOE_SPEED=80
CONST FOE_SHOTREP=1000
CONST FOE_DEADTIME=3000

DIM Integer Foes(HELI_MAX)
DIM Integer Foes.Trigger(HELI_MAX)

sub Foe.init()
  LOCAL Integer n

  for n=1 to HELI_MAX-1
    Foes(n)=0
    Heli.State(n)=0
  next

  clr(Game.Requests,REQ_SPAWNFOE)
  clr(Game.Requests,ACQ_SPAWNFOE)
end sub

sub Foe.spawn(x!,y!,trigger%)
  STATIC Float deadTimer=0
  LOCAL Integer n

  if TIMER-deadTimer<FOE_DEADTIME then exit sub
  deadTimer=TIMER

  for n=1 to HELI_MAX-1
    if Foes(n)=0 then
      Heli.reset n,x!,y!
      Foes(n)=n
      Foes.Trigger(n)=trigger%
      set(Game.Requests,ACQ_SPAWNFOE)
      exit for
    endif
  next
end sub

sub Foe.draw()
  STATIC Float ti=TIMER
  LOCAL Float distX,distY
  LOCAL Integer t,fire

  if TIMER-ti>FOE_SHOTREP then ti=TIMER:fire=1

  for t=1 to HELI_MAX-1
    if Foes(t)=0 then continue for

    if Heli.State(0)=0 then
      if Heli.State(t)=5 then
        Heli.setState t,4
      elseif Heli.State(t)=1 then
        Heli.setState t,2
      endif
    elseif Heli.State(t) then
      distX=distanceX(Heli.X(0),Heli.X(t))
      if distX<0 then
        Heli.setState t,5  'attack left
        inc Heli.X(t),corrFPS!(-0.7)
      else
        Heli.setState t,1  'attack right
        inc Heli.X(t),corrFPS!(0.7)
      endif
      Heli.X(t)=rnX(Heli.X(t))

      distY=Heli.Y(0)-Heli.Y(t)
      if distY<-0.5 then
        inc Heli.Y(t),corrFPS!(-0.2)
      elseif distY>0.5 then
        inc Heli.Y(t),corrFPS!(0.2)
      endif
      if fire then Heli.fire t
    else
      ' Foe has been killed (State=0)
      Foes(t)=0
      Trigger.rem REQ_SPAWNFOE,Foes.Trigger(t)
      clr(Game.Requests,REQ_SPAWNFOE)
      clr(Game.Requests,ACQ_SPAWNFOE)
    endif
  next
end sub


