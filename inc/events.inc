

DIM Integer Event.onff
DIM Float   Event.Intervals(1)=(2000,2000)

'Configuration for Moving Blocks
DIM Integer Event.LASER.color=&H6FE0F0

'Configuration for Moving Blocks
DIM Integer Event.MVBLK.dir=1
DIM Integer Event.MVBLK.color=&H406080

'Configuration for Gate Blocks
DIM Integer Event.GTBLK.color=&H406080
DIM Float   Event.GTBLK.ival(1)=(0,4)

'Configuration for Glowing Bricks
DIM Float   Event.GLBRK.ival(1)=(0,7)
DIM Integer Event.GLBRK.idx

sub Event.init()
  Event.GateBlocks.draw
  Event.ExitBlocks.draw
end sub

sub Event.draw()
  STATIC Float ti=TIMER

  if TIMER-ti>Event.Intervals(Event.onff) then
    ti=TIMER
    Event.onff=(Event.onff+1) AND 1
    
    inc Event.GLBRK.ival(0)
    if Event.GLBRK.ival(0)>Event.GLBRK.ival(1) then
      Event.GLBRK.ival(0)=0
      Event.GLBRK.idx=0
    endif

    inc Event.GTBLK.ival(0)
    if Event.GTBLK.ival(0)>Event.GTBLK.ival(1) then
       Event.GTBLK.ival(0)=0
       Event.GateBlocks.draw
    endif

    Event.Laser.draw Event.onff
    Event.MovingBlocks.draw
    cmap.activate
  endif
  Event.GlowingBricks.draw
end sub

sub Event.GlowingBricks.draw()
  STATIC Integer colors(10)=(&HFF0A12,&HFF2020,&HFF7070,&HFFA0A0,&HFFC0C0,&HFFFFFF,&HFFC0C0,&HFFA0A0,&HFF7070,&HFF2020,&HFF0A12)
  STATIC Float   ti=TIMER

  if Event.GLBRK.idx<11 and TIMER-ti>20 then
    ti=TIMER
    cmap.set(130,colors(Event.GLBRK.idx))
    cmap.activate
    inc Event.GLBRK.idx
  endif
end sub

sub Event.Laser.draw(onff%)
  STATIC Integer laser
  LOCAL  Integer color

  if onff%=1 then laser=fix(rnd*4):color=Event.LASER.color
  select case laser
    case 0: cmap.set(128,color)
    case 1: cmap.set(129,color)
    case 2,3:
      cmap.set(128,color)
      cmap.set(129,color)
  end select
end sub

sub Event.MovingBlocks.draw()
  STATIC Integer pattern=&B0111000000
  LOCAL Integer color,n '  _87654321_

  if Event.MVBLK.dir>0 then
    pattern=pattern>>1
    if tst(pattern,0) then pattern=pattern AND &B0111111110 OR &B0100000000
  else
    pattern=pattern<<1
    if tst(pattern,9) then pattern=pattern AND &B0111111110 OR &B0000000010
  endif
  
  for n=1 to 8
    cmap.set(135+n,choice(tst(pattern,n),Event.MVBLK.color,0))
  next
end sub

sub Event.GateBlocks.draw()
  LOCAL Float gate=fix(rnd*3)

  cmap.set(131,choice(gate=0,0,Event.GTBLK.color))
  cmap.set(132,choice(gate=1,0,Event.GTBLK.color))
  cmap.set(133,choice(gate=2,0,Event.GTBLK.color))
end sub

sub Event.ExitBlocks.draw()
  LOCAL Float gate=fix(rnd*2)

  if rnd>0.5 then
    cmap.set(134,Event.GTBLK.color)
    cmap.set(135,0)
  else
    cmap.set(134,0)
    cmap.set(135,Event.GTBLK.color)
  endif
end sub




