' Explosion code

CONST EXPLOSIONS_MAX=5
CONST EXPLOSIONS_SPEED=20
CONST PARTICLES_MAX=20
CONST PARTICLES_SPEED=50

DIM Integer Explosion.W=25
DIM Integer Explosion.H=25
DIM Integer Explosion.frame(EXPLOSIONS_MAX)
DIM Float   Explosion.X(EXPLOSIONS_MAX)
DIM Float   Explosion.Y(EXPLOSIONS_MAX)

DIM Integer Particle.life(PARTICLES_MAX)
DIM Float   Particle.X(PARTICLES_MAX)
DIM Float   Particle.Y(PARTICLES_MAX)
DIM Float   Particle.dX(PARTICLES_MAX)
DIM Float   Particle.dY(PARTICLES_MAX)


sub Explosion.init()
  LOCAL Integer t
  
  for t=0 to EXPLOSIONS_MAX-1
    Explosion.frame(t)=0
    Explosion.X(t)=0
    Explosion.Y(t)=0
  next
end sub

'x!,y! in playfield coordinates
sub Explosion.add(x!,y!,ch%)
  LOCAL Integer t

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)=0 then
      Explosion.frame(t)=1
      Explosion.X(t)=x!-Explosion.W/2
      Explosion.Y(t)=y!-Explosion.H/2
      PlaySample 11,22050,ch%
      exit sub
    endif
  next
end sub

sub Explosion.draw()
  STATIC Float interval=TIMER
  LOCAL Integer t,c,r,x,y,newFrame
  
  if TIMER-interval>EXPLOSIONS_SPEED then
    interval=TIMER
    newFrame=1
  endif

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)>0 then
      c=Explosion.frame(t) mod 4
      r=int(Explosion.frame(t) / 4)
      x=Screen.VPx+pfxToVPx!(Explosion.X(t))
      y=Screen.VPy+pfyToVPy!(Explosion.Y(t))-Explosion.frame(t)
      blit 220+c*Explosion.W,r*Explosion.H,x,y,Explosion.W,Explosion.H,2,&B100
      Explosion.frame(t)=(Explosion.frame(t)+newFrame) AND 31
    endif
  next
end sub

sub Particle.show()
  LOCAL Integer z
  LOCAL Float _x,_y

  for z=0 to PARTICLES_MAX-1
    if Particle.life(z)=0 then continue for

    if isVisible%(Particle.X(z),Particle.Y(z)) then
      _x=Screen.VPx+pfxToVPx!(Particle.X(z))
      _y=Screen.VPy+pfyToVPy!(Particle.Y(z))
      sprite show z+1,_x,_y,0
    else
      Particle.life(z)=0
    endif
  next
  Map.putTile 0,49,10:Map.map(49,10)=-1
  Map.putTile 0,50,10:Map.map(50,10)=-1
  Map.putTile 0,49,11:Map.map(49,11)=-1
  Map.putTile 0,50,11:Map.map(50,11)=-1
end sub

sub Particle.draw()
  STATIC Float ivalTimer,_x,_y
  LOCAL Integer z

  if TIMER-ivalTimer>PARTICLES_SPEED then
    ivalTimer=TIMER

    for z=0 to PARTICLES_MAX-1
      if Particle.life(z)>0 then
        inc Particle.X(z),Particle.dX(z)
        inc Particle.Y(z),Particle.dY(z)

        if Map.collision%(Particle.X(z),Particle.Y(z),8,8) then
          Explosion.add(Particle.X(z)+4,Particle.Y(z)+4)
          Particle.life(z)=0
          sprite hide z+1
        elseif isVisible%(Particle.X(z),Particle.Y(z)) then
          _x=Screen.VPx+pfxToVPx!(Particle.X(z))
          _y=Screen.VPy+pfyToVPy!(Particle.Y(z))
          if _y>=200 then error "E#z="+str$(z)+": _y="+str$(_y)+" y="+str$(Particle.Y(z))
          sprite next z+1,_x,_y
          inc Particle.life(z),-1
          if Particle.life(z)=0 then sprite hide z+1
        else
          Particle.life(z)=0
          sprite hide z+1
        endif
      endif
    next
  endif
  sprite move
end sub

sub Particle.prepReactor()
  LOCAL Float alpha
  LOCAL Integer c,r,z

  for z=0 to 15
    c=z mod 4:r=fix(z/4)
    sprite read z+1,192+c*8,160+r*8,8,8,PAGE_TILESET
    Particle.X(z)=784+c*8
    Particle.Y(z)=160+r*8
    Particle.life(z)=100

    alpha=atan2(c-1.5+(RND-0.5)/10,r-1.5+(RND-0.5)/10)
    Particle.dX(z)=10*RND*sin(alpha)
    Particle.dY(z)=10*RND*cos(alpha)
  next
end sub


