' Code for event triggers

CONST TRIGGER_MAX=100

DIM Integer Trigger.next
DIM Float   Trigger.X(TRIGGER_MAX)
DIM Float   Trigger.Y(TRIGGER_MAX)
DIM Float   Trigger.W(TRIGGER_MAX)
DIM Float   Trigger.H(TRIGGER_MAX)
DIM Float   Trigger.Req(TRIGGER_MAX)
DIM Float   Trigger.Arg(TRIGGER_MAX)


sub Trigger.init()
  Trigger.next=0
end sub

sub Trigger.add(x!,y!,w%,h%$,req%,arg%)
  LOCAL Integer idx=Trigger.next
  
  Trigger.X(idx)=x!
  Trigger.Y(idx)=y!
  Trigger.W(idx)=w%
  Trigger.H(idx)=h%
  Trigger.Req(idx)=req%
  Trigger.Arg(idx)=arg%
  inc Trigger.next
end sub

sub Trigger.rem(req%,arg%)
  LOCAL Integer z,t

  for z=0 to Trigger.next-1
    if Trigger.Req(t)=req% and Trigger.Arg(t)=arg% then
      Trigger.copy Trigger.next-1,t
      inc Trigger.next,-1
    else
      inc t
    endif
  next
end sub

function Trigger.isHit(x!,y!) AS INTEGER
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer t
  
  Trigger.isHit=-1
  for t=0 to Trigger.next-1
    x1=Trigger.X(t):x2=x1+Trigger.W(t)
    y1=Trigger.Y(t):y2=y1+Trigger.H(t)
    if x1<x! and x!<x2 and y1<y! and y!<y2 then
      Trigger.isHit=t
      exit function
    endif
  next
end function

sub Trigger.copy(idxFrom%,idxTo%)
  if idxFrom%=idxTo% then exit sub

  Trigger.X(idxTo%)=Trigger.X(idxFrom%)
  Trigger.Y(idxTo%)=Trigger.Y(idxFrom%)
  Trigger.W(idxTo%)=Trigger.W(idxFrom%)
  Trigger.H(idxTo%)=Trigger.H(idxFrom%)
  Trigger.Req(idxTo%)=Trigger.Req(idxFrom%)
  Trigger.Arg(idxTo%)=Trigger.Arg(idxFrom%)
end sub



