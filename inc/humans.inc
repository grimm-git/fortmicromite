' Code to handle humans that should be rescued

CONST HUMANS_MAX=32
CONST MAX_HUMANS=32
CONST SPEED_HUMANS=160

CONST STATE_INACTIVE=0
CONST STATE_STANDING=1
CONST STATE_WAVING=2
CONST STATE_RUNNING=3

DIM Integer Human.W=10
DIM Integer Human.H=9

DIM Integer Humans.next
DIM Integer Humans.state(MAX_HUMANS)
DIM Float   Humans.X(MAX_HUMANS)
DIM Float   Humans.Y(MAX_HUMANS)
DIM Float   Humans.minX(MAX_HUMANS)
DIM Float   Humans.maxX(MAX_HUMANS)
DIM Integer Humans.idx(MAX_HUMANS)
DIM Float   Humans.dX(MAX_HUMANS)

sub Humans.init()
  Humans.next=0
end sub

' Set a group of humans in the map and distributes them
' cnt% = how many humans should be set
' y! = y-coordiate where the humans walk
' xmin%, xmax% = movement area of humans
sub Humans.set(cnt%,y!,xmin%,xmax%)
  LOCAL Integer idx,n
  LOCAL Float r=RND,minX,maxX
  
  minX=xmin%*MAPTILESIZE+4
  maxX=(xmax%+1)*MAPTILESIZE-4

  for n=0 to cnt%-1
    idx=Humans.next
    Humans.state(idx)=STATE_STANDING
    Humans.X(idx)=minX+(maxX-minX)*RND
    Humans.Y(idx)=y!-Human.H/2
    Humans.minX(idx)=minX
    Humans.maxX(idx)=maxX
    Humans.dX(idx)=0
    Humans.idx(idx)=0
    if idx+1<HUMANS_MAX then Humans.next=idx+1
  next
end sub

sub Humans.draw()
  STATIC Float interval=TIMER,cnt=0
  LOCAL Integer z,n,x,action,idx
  LOCAL Float dist

  if TIMER-interval>SPEED_HUMANS then interval=TIMER:action=1
  if action then inc cnt:if cnt>29 then cnt=0

  n=0
  for z=0 to Humans.next-1
    markOnRadar Humans.X(n)*Radar.Sx,(Humans.Y(n)-Human.H/4)*Radar.Sy

    if action=1 then
      if Heli.landed(0) and abs(Humans.Y(n)-Heli.Y(0))<8 then
        dist=distanceX(Humans.X(n),Heli.X(0))
        Humans.dX(n)=choice(dist<0,1,-1)
        inc Humans.X(n),Humans.dX(n)

        if abs(dist)<2 then
          Humans.copy Humans.next-1,n
          inc Humans.next,-1
          inc Player.Rescued
          inc Player.Score,50
          Game.showMsg str$(Level.Humans-Player.Rescued)+" HUMANS TO RESCUE",1500
          PlaySample 1,22050
          continue for
        elseif Humans.X(n)>Humans.maxX(n) then
          Humans.X(n)=Humans.maxX(n)
          Humans.state(n)=STATE_STANDING
        elseif Humans.X(n)<Humans.minX(n) then
          Humans.X(n)=Humans.minX(n)
          Humans.state(n)=STATE_STANDING
        else
          Humans.state(n)=STATE_RUNNING
        endif
      else
        if cnt=1 then
          Humans.state(n)=fix(1+RND*3)
          if Humans.state(n)=STATE_RUNNING then
            Humans.dX(n)=choice(RND<0.5,-1,1)
          else
            Humans.dX(n)=0
          endif
        endif
        inc Humans.X(n),Humans.dX(n)
        if Humans.X(n)>Humans.maxX(n) or Humans.X(n)<Humans.minX(n) then
          Humans.dX(n)=-1*Humans.dX(n)
        endif
      endif

      inc Humans.idx(n) ' animation housekeepping
    endif

    if isVisible%(Humans.X(n),Humans.Y(n)) then
      select case Humans.state(n)
        case STATE_WAVING:
          Humans.idx(n)=Humans.idx(n) AND 1
          idx = Humans.idx(n)
        case STATE_RUNNING:
          Humans.idx(n)=Humans.idx(n) AND 3
          idx = Humans.idx(n)+3
        case else: ' standing
          Humans.idx(n)=0
          idx = 2
      end select

      Humans.toScreen n,idx,Humans.X(n),Humans.Y(n)
    endif
    inc n
  next
end sub

sub Humans.toScreen(t%,idx%,x!,y!)
  LOCAL Float _x=x!,_y=y!
  LOCAL Integer dir,c,r

  dir=&B100+choice(Humans.dX(t%)<0,1,0)
  _x=Screen.VPx+pfxToVPx!(x!)-Human.W/2
  _y=Screen.VPy+pfyToVPy!(y!)-Human.H/2
  c=idx% mod 4:r=int(idx%/4)
  blit 180+c*Human.W,r*Human.H,_x,_y,Human.W,Human.H,PAGE_SPRITES,dir
end sub

sub Humans.copy(idxFrom%,idxTo%)
  if idxFrom%=idxTo% then exit sub

  Humans.state(idxTo%)=Humans.state(idxFrom%)
  Humans.X(idxTo%)=Humans.X(idxFrom%)
  Humans.Y(idxTo%)=Humans.Y(idxFrom%)
  Humans.minX(idxTo%)=Humans.minX(idxFrom%)
  Humans.maxX(idxTo%)=Humans.maxX(idxFrom%)
  Humans.idx(idxTo%)=Humans.idx(idxFrom%)
  Humans.dX(idxTo%)=Humans.dX(idxFrom%)
end sub

