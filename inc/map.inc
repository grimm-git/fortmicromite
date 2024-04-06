' Code to load level data from disk
' Requires: cmap

CONST MAPTILESIZE=16
CONST MAXBRICKS=14

DIM Integer Map.W=100
DIM Integer Map.H=18
DIM Integer Map.map(Map.W,Map.H)
DIM Integer Map.Masks(240)
DIM Integer Map.Brick.Idx(MAXBRICKS)
DIM Integer Map.Brick.Suc(MAXBRICKS,3)
DIM Integer Map.TCs.Tiles(50,1)
DIM String  Map.TCs.Cmd$(50)
DIM Integer Map.TCs.next

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
  cls
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
  PAGE write PAGE_BUFFER
end sub

sub Map.putTile(idx%,c%,r%)
  LOCAL Integer sx,sy,tx,ty

  PAGE write FRAMEBUFFER
  if idx%>=0 then
    sx=(idx% mod 20)*MAPTILESIZE
    sy=int(idx%/20)*MAPTILESIZE
    tx=c%*MAPTILESIZE
    ty=r%*MAPTILESIZE
    blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,0
  endif
  PAGE write PAGE_BUFFER
end sub

' x,y = playfield coordinates
function Map.objectHit(x!,y!,w%,h%) as Integer
  LOCAL Float _x=x!,_y=y!
  LOCAL Integer tx,_tx,ty,_ty,tile,mask
  LOCAL Integer z,rc
[MAP]  LOCAL Float dx,dy,sw

  _x=rnX(x!)
  if _y<0 then exit function
  tx=int(_x/MAPTILESIZE):ty=int(_y/MAPTILESIZE)

  _ty=ty  
  do
    _tx=tx:if _tx>=MAP.W then inc _tx,-MAP.W
    do
      tile=Map.map(_tx,_ty)

[MAP]  dx=pfxToVPx!(x!)
[MAP]  dy=pfyToVPy!(y!)
[MAP]  frame (Screen.VPx+dx,Screen.VPy+dy,w%,h%)
[MAP]  dx=pfxToVPx!(_tx*MAPTILESIZE)
[MAP]  dy=pfyToVPy!(_ty*MAPTILESIZE)
[MAP]  frame (Screen.VPx+dx,Screen.VPy+dy,MAPTILESIZE,MAPTILESIZE)
[MAP]  text Screen.VPx+dx+8,Screen.VPy+dy+6,str$(tile),"C",7
[MAP]  if sw=0 then
[MAP]    sw=1
[MAP]    text Screen.VPx+dx-16,Screen.VPy+dy+6,str$(_ty),"C",7
[MAP]    text Screen.VPx+dx+8,Screen.VPy+dy-16,str$(_tx),"C",7
[MAP]  endif
      
      if tile<>-1 then
        for z=0 to Map.TCs.next-1
          if tile>=Map.TCs.Tiles(z,0) AND tile<=Map.TCs.Tiles(z,1) then
            rc=call("mc."+Map.TCs.Cmd$(z),tile,_tx,_ty)
            if rc=-1 then exit for
            Map.objectHit=rc
            exit function
          endif
        next      
      endif

      if tile>0 then
        mask=Map.calcMask(_tx,_ty,_x,_y,choice(w%=0,4,w%),h%)
        if mask AND Map.Masks(tile) then
          Map.objectHit=1
          exit function
        endif
      endif

      inc _ty
    loop while _y+h%>_ty*MAPTILESIZE
    _ty=ty:inc tx
  loop while _x+w%>tx*MAPTILESIZE
  Map.objectHit=0
end function

' x,y = playfield coordinates
function Map.bulletHit(x!,y!) as Integer
  LOCAL Float _x,_y
  LOCAL Integer tx,ty,tile,mask
  LOCAL Integer q,idx,suc

  Map.bulletHit=0

  _x=rnX(x!):_y=y!
  if _y<0 then exit function
  tx=int(_x/MAPTILESIZE):ty=int(_y/MAPTILESIZE)
  if tx>=MAP.W then inc tx,-MAP.W
  
  tile=Map.map(tx,ty)
  if tile<=0 then exit function

[MAP] LOCAL Float dx,dy
[MAP] dx=pfxToVPx!(x!)
[MAP] dy=pfyToVPy!(y!)
[MAP] frame (Screen.VPx+dx,Screen.VPy+dy,4,4)
[MAP] dx=pfxToVPx!(tx*MAPTILESIZE)
[MAP] dy=pfyToVPy!(ty*MAPTILESIZE)
[MAP] frame (Screen.VPx+dx,Screen.VPy+dy,MAPTILESIZE,MAPTILESIZE)
[MAP] text Screen.VPx+dx+8,Screen.VPy+dy+6,str$(tile),"C",7
      
  select case tile
    case 60 to 71,98,99,114:  'Bricks
      dx=_x-tx*MAPTILESIZE:dy=_y-ty*MAPTILESIZE
      if dx<MAPTILESIZE/2 then q=0 else q=1
      if dy>MAPTILESIZE/2 then inc q,2
      idx=Map.findBrick(tile)
      suc=Map.Brick.Suc(idx,q)
      if suc<>tile then
        playSample 15,22050
        Map.map(tx,ty)=choice(suc=0,-1,suc)
        Map.putTile suc,tx,ty
      else
        exit function 'hit a hole in the brick
      endif
    case 57 to 59,7,11,139:
      exit function   'laser don't stop bullets
    case 180 to 192:
      if cmap.get(tile-49)=0 then
        exit function
      endif
      Fx.MVBLK.dir=Fx.MVBLK.dir*-1
    case 2,3:
      Map.bulletHit=1
      exit function
    case 212,213,232,233:  'Reaktor
      if Game.ReactorHits<MAXREACTORHITS then
        inc Game.ReactorHits
      else
        set Game.Requests,REQ_REACTOREX
      endif
  end select

  mask=Map.calcMask(tx,ty,_x,_y,4,0)
  if mask AND Map.Masks(tile) then
    select case tile
      case 115:               'Drone
        playSample 15,22050
        Map.map(tx,ty)=-1
        Map.putTile 0,tx,ty
      case 212,213,232,233:   'Reaktor
        dx=Screen.VPx+pfxToVPx!(800):dy=Screen.VPy+pfyToVpy!(175)
        circle dx,dy,Game.ReactorHits/MAXREACTORHITS*6+2,,,map(WHITE),map(WHITE)
    end select
    Map.bulletHit=1
    exit function
  endif
end function

function Map.getDirection(tx%,ty%,x!,y!) as Integer
  LOCAL Float col=2,row=2
  LOCAL Integer _tx=tx%*MAPTILESIZE,_ty=ty%*MAPTILESIZE

  if x!<_tx then
    col=0
  elseif x!<_tx+MAPTILESIZE then
    col=1
  endif

  if y!<_ty then
    row=0
  elseif y!<_ty+MAPTILESIZE then
    row=1
  endif

  Map.getDirection=row*3+col
end function

function Map.calcMask(tx%,ty%,x!,y!,w%,h%) as Integer
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
  Map.calcMask=mask
end function

function Map.calcTileMask(tile%) as Integer
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
  Map.calcTileMask=mask
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
DATA 61,61,65,68,99
DATA 62,65,62,114,66
DATA 63,99,66,67,63
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


