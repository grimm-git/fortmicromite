
CONST FX_SURGE=0      'High energy discharge
CONST FX_LASER=1      'Laser beams
CONST FX_GLOWBRK=2    'Glowing Bricks
CONST FX_EXITBLK=3    'Blocks, which are set at game start
CONST FX_GATEBLK=4    'Blocks, which open/close in intervals
CONST FX_MVBLK=5      'Blocks, which move

DIM Integer Fx.active       'Bit mask=active special effects
DIM Integer Fx.trigger      'Bit mask=triggered special effects
DIM Integer Fx.tact=200     'base tact

'Fraction of a second effets
DIM Float   Fx.SURGE.onTime=200
DIM Integer Fx.SURGE.color=&H6FE0F0

DIM Integer Fx.LASER.color=&H6FE0F0

'Multiple seconds effects
DIM Integer Fx.GLOWBRK.cnt
DIM Float   Fx.GLOWBRK.limit=14

DIM Integer Fx.GATEBLK.color=&H406080
DIM Integer Fx.GATEBLK.cnt=1
DIM Integer Fx.GATEBLK.limit=8

DIM Integer Fx.MVBLK.color=&H406080
DIM Integer Fx.MVBLK.pattern=&B111000000
DIM Integer Fx.MVBLK.dir=choice(RND<0.5,-1,1)
DIM Integer Fx.MVBLK.cnt
DIM Integer Fx.MVBLK.limit

'OneTimer
DIM Integer Fx.EXITBLK.color=&H406080

sub Fx.init()
  LOCAL Integer colors(6)=(128,129,131,132,133,144,145)
  LOCAL Integer z
  
  for z=0 to Bound(colors())-1
    cmap.set(colors(z),0)
  next

  'effects area: one timer
  if tst(Fx.active,FX_EXITBLK) then Fx.drawExitBlocks
end sub

sub Fx.draw()
  STATIC Float tactTimer=TIMER
  STATIC Integer idx,creg,onff

  if TIMER-tactTimer>Fx.tact then
    tactTimer=TIMER
    inc idx:if idx>9 then idx=0

    if tst(Fx.active,FX_SURGE) then
      if idx=1 then creg=128:set Fx.trigger,FX_SURGE
      if idx=6 then creg=144:set Fx.trigger,FX_SURGE
    else if tst(Fx.active,FX_LASER) then
      if idx=1 then onff=onff+1 AND 1:Fx.drawLaser onff
    endif

    if idx=0 or idx=5 then
      if tst(Fx.active,FX_GLOWBRK) then
        if isReady(Fx.GLOWBRK.cnt,Fx.GLOWBRK.limit) then set Fx.trigger,FX_GLOWBRK
      endif

      if tst(Fx.active,FX_GATEBLK) then
        if isReady(Fx.GATEBLK.cnt,Fx.GATEBLK.limit) then Fx.drawGateBlocks
      endif

      if tst(Fx.active,FX_MVBLK) then
        if isReady(Fx.MVBLK.cnt,Fx.MVBLK.limit) then Fx.drawMovingBlocks
      endif
    endif
  endif

  Fx.drawGlowBricks
  Fx.drawSurge creg
end sub

function isReady(value%,limit%) as Integer
  isReady=0
  inc value%
  if value%=limit% then value%=0:isReady=1
end function

sub Fx.installSurge()
  set Fx.active,FX_SURGE
end sub

sub Fx.installLaser()
  set Fx.active,FX_LASER
end sub

sub Fx.installGlowBricks()
  read Fx.GLOWBRK.limit
  set Fx.active,FX_GLOWBRK
end sub

sub Fx.installMovingBlocks()
  LOCAL Integer pattern

  read Fx.MVBLK.color
  read Fx.MVBLK.limit
  read pattern

  Fx.MVBLK.pattern=(pattern AND 255)<<1
  set Fx.active,FX_MVBLK
end sub

sub Fx.installGateBlocks()
  read Fx.GATEBLK.color
  read Fx.GATEBLK.limit
  set Fx.active,FX_GATEBLK
end sub

sub Fx.installExitBlocks()
  read Fx.EXITBLK.color
  set Fx.active,FX_EXITBLK
end sub
'-----------------------------------

sub Fx.drawSurge(creg%)
  STATIC Float exitTimer
  STATIC Integer sw

  if tst(Fx.trigger,FX_SURGE) then
    if exitTimer=0 then exitTimer=TIMER
    if TIMER-exitTimer>Fx.SURGE.onTime then
      clr Fx.trigger,FX_Surge
      cmap.set(creg%,0)
      cmap.set(creg%+1,0)
      exitTimer=0
      exit sub
    endif

    sw=sw+1 AND 1    
    cmap.set(creg%,choice(sw=0,Fx.SURGE.color,0))
    cmap.set(creg%+1,choice(sw=0,0,Fx.SURGE.color))
  endif
end sub

sub Fx.drawLaser(onff%)
  STATIC Integer laser
  LOCAL  Integer color

  if onff%=1 then laser=fix(rnd*4):color=Fx.LASER.color
  select case laser
    case 0: cmap.set(128,color)
    case 1: cmap.set(129,color)
    case 2,3:
      cmap.set(128,color)
      cmap.set(129,color)
  end select
end sub

sub Fx.drawGlowBricks()
  STATIC Integer colors(10)=(&HFF0A12,&HFF2020,&HFF7070,&HFFA0A0,&HFFC0C0,&HFFFFFF,&HFFC0C0,&HFFA0A0,&HFF7070,&HFF2020,&HFF0A12)
  STATIC Float   interval
  STATIC Integer col

  if NOT tst(Fx.trigger,FX_GLOWBRK) then exit sub

  if col<11 and TIMER-interval>20 then
    interval=TIMER
    cmap.set(130,colors(col))
    inc col
  else
    clr Fx.trigger,FX_GLOWBRK
    col=0
  endif
end sub

sub Fx.drawMovingBlocks()
  LOCAL Integer color,n

  if Fx.MVBLK.dir>0 then
    Fx.MVBLK.pattern=Fx.MVBLK.pattern>>1
    if tst(Fx.MVBLK.pattern,0) then Fx.MVBLK.pattern=Fx.MVBLK.pattern AND &B0111111110 OR &B0100000000
  else
    Fx.MVBLK.pattern=Fx.MVBLK.pattern<<1
    if tst(Fx.MVBLK.pattern,9) then Fx.MVBLK.pattern=Fx.MVBLK.pattern AND &B0111111110 OR &B0000000010
  endif
  
  for n=1 to 8
    cmap.set(135+n,choice(tst(Fx.MVBLK.pattern,n),Fx.MVBLK.color,0))
  next
end sub

sub Fx.drawGateBlocks()
  LOCAL Float gate=fix(rnd*3)

  cmap.set(131,choice(gate=0,0,Fx.GATEBLK.color))
  cmap.set(132,choice(gate=1,0,Fx.GATEBLK.color))
  cmap.set(133,choice(gate=2,0,Fx.GATEBLK.color))
end sub

sub Fx.drawExitBlocks()
  LOCAL Float gate=fix(rnd*2)

  if rnd>0.5 then
    cmap.set(134,Fx.EXITBLK.color)
    cmap.set(135,0)
  else
    cmap.set(134,0)
    cmap.set(135,Fx.EXITBLK.color)
  endif
end sub

sub Fx.powerOFF()
  clr Fx.active,FX_SURGE
  clr Fx.active,FX_LASER
  clr Fx.active,FX_EXITBLK
  Fx.drawLaser 0   'switch off LASER
  cmap.set(135,0)  'open all exit doors
  cmap.set(134,0) 
end sub



