' code by Matthias Grimm

CONST INTRO_SONGLEN=30000
CONST INTRO_CYCSPEED=200
CONST INTRO_C1=128
CONST INTRO_C2=129

function Intro.play() as Integer
  STATIC Float exitTimer
  STATIC idx%=0
  LOCAL Integer key

  if exitTimer=0 then exitTimer=TIMER
  if TIMER-exitTimer>INTRO_SONGLEN then
    exitTimer=0
    key=22 'NAK
    cls
  else
    key=Controls.readKey%()
    if key=0 then
      pause INTRO_CYCSPEED
    else
      exitTimer=0
      playMOD "soundfx"
      cls
    endif
  endif

  idx%=idx%+1 and 1
  cmap.set(INTRO_C1,choice(idx%=0,&HFFFFFF,&HFF0000))
  cmap.set(INTRO_C2,choice(idx%=0,&HFF0000,&HFFFFFF))
  cmap.activate

  Intro.play=key
end function

sub Intro.show
  LOCAL i%

  PAGE write 0:cls
  prt 0,0,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C2)
  prt0 1,0,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C1)
  for i%=1 to 23
    prt  0,i%,"]",choice(i% mod 2=1,map(INTRO_C1),map(INTRO_C2))
    prt 39,i%,"]",choice(i% mod 2=1,map(INTRO_C2),map(INTRO_C1))
  next
  prt 0,24,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C2)
  prt0 1,24,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C1)

  prt 6,4,"FORT MICROMITE",map(5)
  prt 6,7.5,"CMM2 VERSION BY",map(12)
  prt 6,8.5,"MATTHIAS GRIMM",map(4)
  prt 10,10.5,"SOUND FX BY",map(12)
  prt 7,11.5,"DOMINIK BRAUN",map(4)
  prt 6,12.5,"MATTHIAS GRIMM",map(4)
  prt 12.5,14.5,"MUSIC BY",map(12)
  prt 16,15.5,"HAWK",map(4)

  prt 13,20,"(C) 2024",map(14)
  PAGE DISPLAY 0

  playMOD "intro"
end sub
