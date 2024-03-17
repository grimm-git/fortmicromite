

CONST LEVEL_CNT=2
CONST LEVEL_FOES_MAX=5

DIM String  Level.Name
DIM Integer Level.Color
DIM Integer Level.ExitX
DIM Integer Level.ExitY
DIM Float   Level.Foe.X(LEVEL_FOES_MAX)
DIM Float   Level.Foe.Y(LEVEL_FOES_MAX)
DIM Integer Level.Foe.State(LEVEL_FOES_MAX)
DIM Integer Level.Foe.Dyn(LEVEL_FOES_MAX)

DIM Integer Level.Foe.next
DIM Integer Level.loaded

' level%=1,2...
sub Level.next(level%)
  if Level.loaded<>level% then
    Level.loaded=level%
    Level.load level%
  endif
  Level.init level%
  Event.init
end sub

sub Level.addTank(y%,x%,minX%,maxX%)
  Tanks.set y%,x%,minX%,maxX%
end sub

sub Level.addDrone(y%,minX%,maxX%)
  Drones.set y%,minX%,maxX%
end sub

sub Level.addHumans(cnt%,y%,minX%,maxX%)
  LOCAL Integer x

  Humans.set cnt%,y%,minX%,maxX%
  for x=minX% to maxX%
    Map.map(x,y%)=2
  next   
end sub

sub Level.load(level%)
  LOCAL String name$
  LOCAL Integer n

  if level%>LEVEL_CNT then exit sub

  name$="lvl/"+str$(level%)+"."
  Map.load name$+"map"

  load data name$+"tiles", MM.INFO(Page Address PAGE_TILESET)
  for n=0 to 255:Map.Masks(n)=calcTileMask(n):next

  load data name$+"radar", MM.INFO(Page Address PAGE_RADAR)
  Map.draw

  cmap.load name$+"cmap"
  cmap.activate  
end sub

sub Level.init(level%)
  LOCAL String cmd$
  LOCAL Integer arg1,arg2,arg3,arg4,arg5,arg6
  LOCAL Integer idx

  Level.Foe.next=0
  select case level%
    case 1:
      Playfield.VPx=(Playfield.W-VP.W)/2
      Playfield.VPy=0
      restore Level1
    case 2:
      Playfield.VPx=(Playfield.W-VP.W)/2
      Playfield.VPy=0
      restore Level2
  end select

  read cmd$:Level.Name=cmd$
  read arg1:Level.Color=arg1

  do while cmd$<>"END"
    read cmd$
    select case cmd$
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
    end select
  loop
end sub

Level1:
DATA "DRAGONS  CAVE",1
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
DATA "END"
Level2:
DATA "MACHINES HELL",3
DATA "E",0,0
DATA "END"


