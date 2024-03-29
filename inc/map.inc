' Code to load level data from disk
' Requires: cmap

CONST MAPTILESIZE=16
CONST MAXBRICKS=14

DIM Integer Map.W=100
DIM Integer Map.H=18
DIM Integer Map.map(Map.W,Map.H)
DIM Integer Map.Masks(255)
DIM Integer Map.Brick.Idx(MAXBRICKS)
DIM Integer Map.Brick.Suc(MAXBRICKS,3)

Map.prepareBricks()

'Load the level map and stores it in Level.Map
sub Map.load(name$)
  LOCAL char$,tmp$
  LOCAL r%,c%

  on error abort
  open name$ for input as #1
  if MM.ERRNO>0 then exit sub

  do
    char$=input$(1,#1)
    if char$="," then
       Map.map(c%,r%)=val(tmp$)
       inc c%:tmp$=""
    elseif char$=chr$(10) then
       Map.map(c%,r%)=val(tmp$)
       inc r%:c%=0:tmp$=""
    elseif char$=chr$(13) then
      'nop
    else
      tmp$=tmp$+char$
    endif
  loop while EOF(#1)=0
  close #1

  if tmp$<>"" then Map.map(c%,r%)=val(tmp$)
end sub

sub Map.draw()
  LOCAL c%,r%,tile%
  LOCAL Integer sx,sy,tx,ty

  PAGE write FRAMEBUFFER
  for r%=0 to Map.H-1
    for c%=0 to Map.W-1
      tile%=Map.map(c%,r%)
      if tile%>=0 then
        sx=(tile% mod 20)*MAPTILESIZE
        sy=int(tile%/20)*MAPTILESIZE
        tx=c%*MAPTILESIZE:ty=r%*MAPTILESIZE
        blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,0
      endif
    next
  next
  PAGE write Game.PBuffer
end sub

sub Map.putTile(idx%,c%,r%)
  LOCAL Integer sx,sy,tx,ty

  PAGE write FRAMEBUFFER
  if idx%>=0 then
    sx=(idx% mod 20)*MAPTILESIZE
    sy=int(idx%/20)*MAPTILESIZE
    tx=c%*MAPTILESIZE
    ty=r%*MAPTILESIZE
    blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B0
  endif
  PAGE write Game.PBuffer
end sub

' x,y = playfield coordinates
function Map.collision%(x!,y!,w%,h%)
  LOCAL Float _x=x!,_y=y!
  LOCAL Integer tx,ty,_tx,_ty,tile,mask
  LOCAL Integer shot,q,idx,suc
  LOCAL Float dx,dy,sw

  _x=rnX(x!)
  if _y<0 then exit function
  tx=int(_x/MAPTILESIZE):ty=int(_y/MAPTILESIZE)
  shot=choice(w%=0 and h%=0,1,0)
  
  _ty=ty
  do
    _tx=tx:if _tx>=MAP.W then inc _tx,-MAP.W
    do
      if _tx<0 or _tx>Map.W-1 then error "ERROR: _tx="+str$(_tx)+" tx="+str$(tx)
      if _ty<0 or _ty>Map.H-1 then error "ERROR: _ty="+str$(_ty)+" ty="+str$(ty)
      tile=Map.map(_tx,_ty)

      if DEBUG_MAP=1 then
        dx=pfxToVPx!(x!)
        dy=pfyToVPy!(y!)
        frame (Screen.VPx+dx,Screen.VPy+dy,w%,h%)
        dx=pfxToVPx!(_tx*MAPTILESIZE)
        dy=pfyToVPy!(_ty*MAPTILESIZE)
        frame (Screen.VPx+dx,Screen.VPy+dy,MAPTILESIZE,MAPTILESIZE)
        text Screen.VPx+dx+8,Screen.VPy+dy+6,str$(tile),"C",7
        if sw=0 then
          sw=1
          text Screen.VPx+dx-16,Screen.VPy+dy+6,str$(_ty),"C",7
          text Screen.VPx+dx+8,Screen.VPy+dy-16,str$(_tx),"C",7
        endif
      endif
      
      if shot then
        select case tile
          case 60 to 71,98,99,114:  'Bricks
            dx=_x-tx*MAPTILESIZE:dy=_y-ty*MAPTILESIZE
            if dx<MAPTILESIZE/2 then q=0 else q=1
            if dy>MAPTILESIZE/2 then inc q,2
            idx=Map.findBrick(tile)
            suc=Map.Brick.Suc(idx,q)
            if suc<>tile then
              playSample 15,22050
              Map.map(_tx,_ty)=choice(suc=0,-1,suc)
              Map.putTile suc,_tx,_ty
            else
              Map.collision%=0   'hit a hole in the brick
              exit function
            endif
          case 57 to 59:
            Map.collision%=0   'laser don't stop bullets
            exit function
          case 180 to 192:
            if cmap.get(tile-49)=0 then
              Map.collision%=0
              exit function
            endif
            Event.MVBLK.dir=Event.MVBLK.dir*-1
        end select
      else
        select case tile
          case 0:
            set Game.Requests,REQ_FUELING
          case 2:
            Heli.Y(0)=ty*MAPTILESIZE+9
            Heli.landed(0)=1
            Map.collision%=0
            exit function
          case 23,24,25:
            Map.prepareFuelStation
            Map.collision%=0
            exit function
          case 20,21,22,30,31,32:
            set Game.Requests,REQ_FUELING
            Heli.Y(0)=ty*MAPTILESIZE+6
            if tile>=30 then inc Heli.Y(0),5
            Heli.landed(0)=1
            Map.collision%=0
            exit function
          case 78,79:  ' Level Exit
            Heli.Y(0)=ty*MAPTILESIZE+6
            Heli.landed(0)=1
            Map.prepareLevelExit
            Map.collision%=0
            exit function
          case 91:
            if 224>ty*MAPTILESIZE then
              Heli.Y(0)=ty*MAPTILESIZE+9
              Heli.landed(0)=1
              Player.saveSpawnPt
              Map.collision%=0
              exit function
            endif
          case 57:
            if cmap.get(128)=0 then
              Map.collision%=0
              exit function
            endif
          case 58:
            if cmap.get(129)=0 then
              Map.collision%=0
              exit function
            endif
          case 59:
            if cmap.get(128)+cmap.get(129)=0 then
              Map.collision%=0
              exit function
            endif
          case 180 to 192:
            if cmap.get(tile-49)=0 then
              Map.collision%=0
              exit function
            endif
        end select
      endif

      if tile>0 then
        mask=calcMask(_tx,_ty,_x,_y,choice(w%=0,4,w%),h%)
        if mask AND Map.Masks(tile) then
          if shot then
            select case tile
              case 115:
                playSample 15,22050
                Map.map(_tx,_ty)=-1
                Map.putTile 0,_tx,_ty
            end select
          endif
          Map.collision%=1
          exit function
        endif
      endif

      inc _ty
    loop while _y+h%>_ty*MAPTILESIZE
    _ty=ty:inc tx
  loop while _x+w%>tx*MAPTILESIZE
  Map.collision%=0
end function

sub Map.prepareFuelStation
  STATIC Integer tiles(26)=(49,6,0,50,6,0,51,6,0,49,7,0,50,7,0,51,7,0,49,8,20,50,8,21,51,8,22)
  LOCAL Integer n

  for n=0 to 26 step 3
    Map.map(tiles(n),tiles(n+1))=tiles(n+2)
    Map.putTile tiles(n+2),tiles(n),tiles(n+1)
  next
end sub

sub Map.resetFuelStation
  STATIC Integer tiles(26)=(49,6,23,50,6,24,51,6,25,49,7,0,50,7,26,51,7,0,49,8,27,50,8,28,51,8,29)
  LOCAL Integer n

  for n=0 to 26 step 3
    Map.map(tiles(n),tiles(n+1))=tiles(n+2)
    Map.putTile tiles(n+2),tiles(n),tiles(n+1)
  next
end sub

sub Map.prepareLevelExit
  LOCAL Float x,y
  LOCAL Integer tx1,tx2,ty

  ty=Level.ExitY
  tx1=Level.ExitX
  tx2=tx1+1:if tx2>=Map.W then inc tx2,-Map.W

  Map.putTile 0,tx1,ty:Map.map(tx1,ty)=1
  Map.putTile 0,tx2,ty:Map.map(tx2,ty)=1
  x=pfxToVPx!(tx1*MAPTILESIZE):y=pfyToVPy!(ty*MAPTILESIZE)

  blit 288,48,Screen.VPx+x,Screen.VPy+y,2*MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B100
  set(Game.requests,REQ_NEXTLVL)
end sub

sub Map.levelExit
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

function calcMask(tx%,ty%,x!,y!,w%,h%) as Integer
  LOCAL Float x1=x!-tx%*MAPTILESIZE,x2=x1+w%
  LOCAL Float y1=y!-ty%*MAPTILESIZE,y2=y1+h%
  LOCAL Integer mask,row,n

  if x1<0 then x1=0:if y1<0 then y1=0
  if x2>MAPTILESIZE then x2=MAPTILESIZE
  if y2>MAPTILESIZE then y2=MAPTILESIZE
  row=2^(4-fix(x1/4))-1 AND MAPTILESIZE-2^(4-fix(x2/4))
    
  for n=0 to 3
    mask=mask<<4
    if n*4<=y2 AND (n+1)*4>y1 then mask=mask OR row
  next
  calcMask=mask
end function

function calcTileMask(tile%) as Integer
  LOCAL Integer addr=MM.INFO(Page Address PAGE_TILESET)
  LOCAL Integer col=tile% mod 20
  LOCAL Integer row=fix(tile%/20)
  LOCAL Integer mask,part,n1,n2,n3

  inc addr,col*MAPTILESIZE+row*320*MAPTILESIZE

  for n3=0 to 3 ' 4 rows = 16 bit
    for n2=0 to 3 ' row = 4 bit
      part=0
      for n1=0 to 3  'One group 4x4 = 1 bit
        inc part,peek(BYTE addr+n1+0*320+4*n2)
        inc part,peek(BYTE addr+n1+1*320+4*n2)
        inc part,peek(BYTE addr+n1+2*320+4*n2)
        inc part,peek(BYTE addr+n1+3*320+4*n2)
      next
      mask=mask<<1
      if part>0 then mask=mask OR 1  
    next
    inc addr,4*320
  next
  calcTileMask=mask
end function

function Map.findBrick(tile%) AS Integer
  LOCAL Integer n

  for n=0 to MAXBRICKS
    if tile%=Map.Brick.Idx(n) then exit for
  next
  Map.findBrick=n
end function

sub Map.prepareBricks
  LOCAL Integer idx,q0,q1,q2,q3
  LOCAL Integer n

  restore Bricks
  for n=0 to MAXBRICKS
    read idx,q0,q1,q2,q3
    Map.Brick.Idx(n)=idx
    Map.Brick.Suc(n,0)=q0
    Map.Brick.Suc(n,1)=q1
    Map.Brick.Suc(n,2)=q2
    Map.Brick.Suc(n,3)=q3
  next      
end sub
Bricks: '0  1  2  3     '01
DATA 60,61,62,64,63     '23
DATA 61,61,65,68,113
DATA 62,65,62,114,66
DATA 63,113,66,67,63
DATA 64,68,114,64,67
DATA 65,65,65,98,69
DATA 66,69,66,70,66
DATA 67,71,70,67,67
DATA 68,68,98,68,71
DATA 69,69,69,0,69
DATA 70,0,70,70,70
DATA 71,71,0,71,71
DATA 98,98,98,98,0
DATA 99,99,69,71,99
DATA 114,98,114,114,69


