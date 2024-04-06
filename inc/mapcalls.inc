
CONST EXIT_WITH_COLLISION=1
CONST EXIT_WITHOUT_COLLISION=0
CONST DONT_EXIT=-1

'level 1: tile 0
function mc.Fueling(tile%,tx%,ty%) as Integer
  set Game.Requests,REQ_FUELING
  mc.fueling=DONT_EXIT
end function

'level 1: tiles 2
'level 2: tiles 2
function mc.landOnGrass(tile%,tx%,ty%) as Integer
  Heli.Y(0)=ty%*MAPTILESIZE-7
  Heli.landed(0)=1
  mc.landOnGrass=EXIT_WITHOUT_COLLISION
end function

'level 2: tiles 3
function mc.landOnBlk(tile%,tx%,ty%) as Integer
  Heli.Y(0)=ty%*MAPTILESIZE-9
  Heli.landed(0)=1
  mc.landOnBlk=EXIT_WITHOUT_COLLISION
end function

'level 1: tiles 20,21,22
function mc.refuelAtMainStation(tile%,tx%,ty%) as Integer
  set Game.Requests,REQ_FUELING
  Heli.Y(0)=ty%*MAPTILESIZE-10
  Heli.landed(0)=1
  mc.refuelAtMainStation=EXIT_WITHOUT_COLLISION
end function

'level 1: tiles 30,31,32
'level 2: tiles 30,31,32
function mc.refuelAtSubStation(tile%,tx%,ty%) as Integer
  set Game.Requests,REQ_FUELING
  Heli.Y(0)=ty%*MAPTILESIZE-5
  Heli.landed(0)=1
  mc.refuelAtSubStation=EXIT_WITHOUT_COLLISION
end function


'level 1: tiles 23,24,25
function mc.prepFuelStation(tile%,tx%,ty%) as Integer
  STATIC Integer tiles(26)=(49,6,0,50,6,0,51,6,0,49,7,0,50,7,0,51,7,0,49,8,20,50,8,21,51,8,22)
  LOCAL Integer n

  for n=0 to 26 step 3
    Map.map(tiles(n),tiles(n+1))=tiles(n+2)
    Map.putTile tiles(n+2),tiles(n),tiles(n+1)
  next

  mc.prepFuelStation=EXIT_WITHOUT_COLLISION
end function

sub mc.resetFuelStation()
  STATIC Integer tiles(26)=(49,6,23,50,6,24,51,6,25,49,7,0,50,7,26,51,7,0,49,8,27,50,8,28,51,8,29)
  LOCAL Integer n

  for n=0 to 26 step 3
    Map.map(tiles(n),tiles(n+1))=tiles(n+2)
    Map.putTile tiles(n+2),tiles(n),tiles(n+1)
  next
end sub

'level 1: tiles 78,79
'level 2: tiles 78,79
function mc.prepLevelExit(tile%,tx%,ty%) as Integer
  LOCAL Float x,y
  LOCAL Integer tx1,tx2,ty1

  Heli.Y(0)=ty%*MAPTILESIZE-10
  Heli.landed(0)=1

  ty1=Level.ExitY
  tx1=Level.ExitX
  tx2=tx1+1:if tx2>=Map.W then inc tx2,-Map.W

  Map.putTile 0,tx1,ty1:Map.map(tx1,ty1)=1
  Map.putTile 0,tx2,ty1:Map.map(tx2,ty1)=1
  x=pfxToVPx!(tx1*MAPTILESIZE):y=pfyToVPy!(ty1*MAPTILESIZE)

  blit 288,48,Screen.VPx+x,Screen.VPy+y,2*MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B100
  set(Game.requests,REQ_NEXTLVL)
  mc.prepLevelExit=EXIT_WITHOUT_COLLISION
end function

sub mc.doLevelExit()
  STATIC Float ti=TIMER,ofs=0
  LOCAL Float x,y
  LOCAL Integer tx,ty

  if TIMER-ti>180 then ti=TIMER:inc ofs:inc Player.Y

  if ofs<15 then
    tx=Level.ExitX:ty=Level.ExitY
    x=pfxToVPx!(tx*MAPTILESIZE):y=pfyToVPy!(ty*MAPTILESIZE)+ofs
    blit 288,48,Screen.VPx+x,Screen.VPy+y,2*MAPTILESIZE,MAPTILESIZE-ofs,PAGE_TILESET,&B100
  else
    ofs=0
    clr(Game.requests,REQ_NEXTLVL)
    set(Game.requests,ACQ_NEXTLVL)
  endif
end sub

'level 1: tiles 91
'level 2: tiles 223
function mc.landOnPlatform(tile%,tx%,ty%) as Integer
  LOCAL Integer rc=DONT_EXIT
  if Map.getDirection(tx%,ty%,Heli.X(0),Heli.Y(0))<3 then
     Heli.Y(0)=ty%*MAPTILESIZE-7
     Heli.landed(0)=1
     Player.saveSpawnPt
     rc=EXIT_WITHOUT_COLLISION
  endif
  mc.landOnPlatform=rc
end function

'level 1: tiles 57
function mc.laser45N(tile%,tx%,ty%) as Integer
  mc.laser45N=choice(cmap.get(129)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 1: tiles 58
function mc.laser45P(tile%,tx%,ty%) as Integer
  mc.laser45P=choice(cmap.get(128)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 1: tiles 59
'level 2: tiles 11,138,139
function mc.laser45PN(tile%,tx%,ty%) as Integer
  mc.laser45PN=choice(cmap.get(128)+cmap.get(129)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 2: tiles 7
function mc.laser0PN(tile%,tx%,ty%) as Integer
  mc.laser0PN=choice(cmap.get(144)+cmap.get(145)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 2: tiles 198,218,248
function mc.openPortal(tile%,tx%,ty%) as Integer
  set Game.Requests,REQ_PORTAL
  mc.openPortal=EXIT_WITHOUT_COLLISION
end function

'level 2: tiles 40,41,46,47
function mc.movingBlockC1(tile%,tx%,ty%) as Integer
  mc.movingBlockC1=choice(cmap.get(133)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 2: tiles 42,43,48,50
function mc.movingBlockC2(tile%,tx%,ty%) as Integer
  mc.movingBlockC2=choice(cmap.get(142)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 2: tiles 44,45,51,52
function mc.movingBlockC3(tile%,tx%,ty%) as Integer
  mc.movingBlockC3=choice(cmap.get(143)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function

'level 1: tiles 180 - 192   Moving Blocks
function mc.movingBlock(tile%,tx%,ty%) as Integer
  mc.movingBlock=choice(cmap.get(tile%-49)=0,EXIT_WITHOUT_COLLISION,DONT_EXIT)
end function



