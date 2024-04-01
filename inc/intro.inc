' code by Matthias Grimm

CONST INTRO_SONGLEN=30000
CONST INTRO_CYCSPEED=200
CONST INTRO_C1=250
CONST INTRO_C2=251

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

  map(INTRO_C1)=&HFFFFFF
  map(INTRO_C2)=&HFF0000
  map set

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
  playMOD "intro"
end sub

sub Intro.victory
  LOCAL i%

  map(INTRO_C1)=&HFFFFFF
  map(INTRO_C2)=&HFF0000
  map set

  PAGE write 0:cls
  prt 0,0,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C2)
  prt0 1,0,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C1)
  for i%=1 to 23
    prt  0,i%,"]",choice(i% mod 2=1,map(INTRO_C1),map(INTRO_C2))
    prt 39,i%,"]",choice(i% mod 2=1,map(INTRO_C2),map(INTRO_C1))
  next
  prt 0,24,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C2)
  prt0 1,24,"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]",map(INTRO_C1)

  text VP.W/2,32,txt$("VICTORY"),"C",8,2,map(5)

  prt 9,8.5,"DR. EVIL HAS",map(5)
  prt 7,10,"BEEN DEFEATED",map(5)

  prt 7.5,13,"SCORE",map(14)
  prt 24,13,str$(Player.Score+Player.Bonus),map(6)
  prt 7.5,14.5,"BONUS",map(14)
  prt 24,14.5,str$(Player.Bonus),map(6)
  prt 7.5,16,"RESCUED",map(14)
  prt 24,16,str$(Player.Rescued)+"/"+str$(Level.Humans),map(6)
  playMOD "intro"
end sub

