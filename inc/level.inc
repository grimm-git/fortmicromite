

CONST LEVEL_CNT=2
CONST LEVEL_FOES_MAX=4

DIM String  Level.Name
DIM Integer Level.Color
DIM Integer Level.ExitX
DIM Integer Level.ExitY
DIM Integer Level.Humans
DIM Float   Level.Foe.X(LEVEL_FOES_MAX)
DIM Float   Level.Foe.Y(LEVEL_FOES_MAX)
DIM Integer Level.Foe.State(LEVEL_FOES_MAX)
DIM Integer Level.Foe.Dyn(LEVEL_FOES_MAX)

DIM Integer Level.Foe.next

' level%=1,2...
sub Level.next(level%)
  Drones.init
  Humans.init
  Tanks.init
  Trigger.init
  Foe.init
  Level.load level%
  Level.init level%
  Fx.init
end sub

sub Level.load(level%)
  LOCAL String name$
  LOCAL Integer n

  if level%>LEVEL_CNT then exit sub

  name$="lvl/"+str$(level%)+"."
  load data name$+"tiles", MM.INFO(Page Address PAGE_TILESET)
  for n=0 to 240:map.Masks(n)=calcTileMask(n):next

  Map.load name$+"map"
  load data name$+"radar", MM.INFO(Page Address PAGE_RADAR)
  Map.draw
end sub

sub Level.init(level%)
  LOCAL String cmd$,fx$
  LOCAL Integer arg1,arg2,arg3,arg4,arg5,arg6
  LOCAL Integer idx

  Fx.active=0
  Level.Foe.next=0
  select case level%
    case 1:
      Player.SpawnPt(2)=(Playfield.W-VP.W)/2
      Player.SpawnPt(3)=0
      restore Level1
    case 2:
      Player.SpawnPt(2)=(Playfield.W-VP.W)/2
      Player.SpawnPt(3)=0
      restore Level2
  end select

  read cmd$:Level.Name=cmd$
  read arg1:Level.Color=arg1

  do while cmd$<>"END"
    read cmd$
    select case cmd$
      case "S":  'Spawn
        read arg1,arg2
        Player.SpawnPt(0)=arg1
        Player.SpawnPt(1)=arg2
      case "E":  'Exit
        read arg1,arg2
        Level.ExitX=arg1
        Level.ExitY=arg2
      case "P":  'Pamzer=Tanks
        read arg1,arg2,arg3,arg4
        Level.addTank arg1,arg2,arg3,arg4
      case "H":  'Humans
        read arg1,arg2,arg3,arg4
        Level.addHumans arg1,arg2,arg3,arg4
      case "D":  'Drone
        read arg1,arg2,arg3
        Level.addDrone arg1,arg2,arg3
      case "F":  'Foe
        read arg1,arg2,arg3,arg4
        idx=Level.Foe.next
        Level.Foe.X(idx)=arg1
        Level.Foe.Y(idx)=arg2
        Level.Foe.State(idx)=arg3
        Level.Foe.Dyn(idx)=arg4
        inc Level.Foe.next
      case "T":  'Trigger
        read arg1,arg2,arg3,arg4,arg5,arg6
        Trigger.add arg1,arg2,arg3,arg4,arg5,arg6
      case "X":  'fX'
        read fx$
        call "Fx.install"+fx$
    end select
  loop
end sub

sub Level.addTank(y%,x%,minX%,maxX%)
  Tanks.set y%,x%,minX%,maxX%
end sub

sub Level.addDrone(y%,minX%,maxX%)
  Drones.set y%,minX%,maxX%
end sub

sub Level.addHumans(cnt%,y%,minX%,maxX%)
  STATIC Integer tiles(9)=(80,92,100,122,123,163,173,173,175,225)
  LOCAL Integer x,ofs

  inc Level.Humans,cnt%
  for x=0 to Bound(tiles())-1
    if Map.map(minX%,y%)=tiles(x) then ofs=2:exit for
  next
  Humans.set cnt%,y%*MAPTILESIZE+ofs,minX%,maxX%

  for x=minX% to maxX%
    Map.map(x,y%)=2+(ofs=0)
  next
end sub

Level1:
DATA "DRAGONS  CAVE",1
DATA "S",VP.W/3,20
DATA "E",99,17
DATA "P",9,14,6,33
DATA "P",9,24,6,33
DATA "P",9,74,67,93
DATA "P",9,86,67,93
DATA "P",14,80,73,84
DATA "P",14,27,25,28
DATA "H",2,6,44,46
DATA "H",3,14,94,96
DATA "D",50,200,600
DATA "D",30,1000,1400
DATA "D",185,1450,1750
DATA "F",10,10,5,1
DATA "F",1400,170,5,1
DATA "T",1300,0,4,130,REQ_SPAWNFOE,0
DATA "T",300,0,4,130,REQ_SPAWNFOE,0
DATA "T",1530,152,70,10,REQ_SPAWNTANKS,9 'Trigger split into two because
DATA "T",0,152,70,10,REQ_SPAWNTANKS,9    ' it wraps around Playfield.W
DATA "T",1530,180,70,16,REQ_SPAWNFOE,1   'Trigger split into two because
DATA "T",0,180,70,16,REQ_SPAWNFOE,1      ' it wraps around Playfield.W
DATA "X","GLOWBRICKS",14
DATA "X","LASER"
DATA "X","MOVINGBLOCKS",&H406080,2,&B11100000
DATA "X","GATEBLOCKS",&H406080,7
DATA "X","EXITBLOCKS",&H406080
DATA "END"
Level2:
DATA "MACHINES HELL",3
DATA "S",VP.W/2,30
DATA "E",49,17
DATA "H",3,17,70,73
DATA "H",2,9,57,60
DATA "H",2,9,86,88
DATA "H",3,4,23,29
DATA "H",2,17,13,16
DATA "D",50,1430,1675
DATA "D",120,1385,1550
DATA "D",180,1450,1575
DATA "D",200,1450,1575
DATA "D",110,44,120
DATA "D",170,7,57
DATA "D",207,7,57
DATA "P",17,25,21,40
DATA "P",17,35,21,40
DATA "X","GLOWBRICKS",14
DATA "X","SURGE"
DATA "X","MOVINGBLOCKS",&H406080,2,&B11110000
DATA "X","EXITBLOCKS",&H406080
DATA "END"


