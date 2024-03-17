' Explosion code

CONST MAX_EXPLOSIONS=5
CONST SPEED_EXPLOSIONS=20

DIM Integer Explosion.W=25
DIM Integer Explosion.H=25
DIM Integer Explosion.frame(MAX_EXPLOSIONS)
DIM Float   Explosion.X(MAX_EXPLOSIONS)
DIM Float   Explosion.Y(MAX_EXPLOSIONS)

sub Explosion.init()
  LOCAL Integer t
  
  for t=0 to MAX_EXPLOSIONS-1
    Explosion.frame(t)=0
    Explosion.X(t)=0
    Explosion.Y(t)=0
  next
end sub

'x!,y! in playfield coordinates
sub Explosion.add(x!,y!,ch%)
  LOCAL Integer t

  for t=0 to MAX_EXPLOSIONS-1
    if (Explosion.frame(t)=0) then
      Explosion.frame(t)=1
      Explosion.X(t)=x!-Explosion.W/2
      Explosion.Y(t)=y!-Explosion.H/2
      PlaySample 11,22050,ch%
      exit sub
    endif
  next
end sub

sub Explosion.draw
  STATIC Float interval=TIMER
  LOCAL Integer t,c,r,x,y,newFrame
  
  if (TIMER-interval>SPEED_EXPLOSIONS) then
    interval=TIMER
    newFrame=1
  endif

  for t=0 to MAX_EXPLOSIONS-1
    if (Explosion.frame(t) > 0) then
      c=Explosion.frame(t) mod 4
      r=int(Explosion.frame(t) / 4)
      x=Screen.VPx+pfxToVPx!(Explosion.X(t))
      y=Screen.VPy+pfyToVPy!(Explosion.Y(t))-Explosion.frame(t)
      blit 220+c*Explosion.W,r*Explosion.H,x,y,Explosion.W,Explosion.H,2,&B100
      Explosion.frame(t)=(Explosion.frame(t)+newFrame) AND 31
    endif
  next
end sub


