' ApocalypseFont
' Methods to create output for Apocalypse Font

DIM number$(9)
DIM alpha$(25)

sub prt(x!,y!,msg$,col!,ln%)
  LOCAL String tmp$
  LOCAL Float _x=x!

  tmp$=txt$(msg$)
  if ln%>0 then
    if len(tmp$)<ln% then tmp$=space$(ln%-len(tmp$))+tmp$
  endif
  if _x=-1 then _x=20-len(tmp$)/2        

  text _x*8,y!*8,tmp$,"L",8,,col!
end sub

sub prt0(x!,y!,msg$,col!)
  text x!*8,y!*8,txt$(msg$),"L",8,,col!,-1
end sub

sub prtS(x!,y!,msg$,col!)
  text x!*8,y!*8,txt$(msg$),"L",8,,map(0),-1
  text x!*8-1,y!*8-1,txt$(msg$),"L",8,,col!,-1
end sub

function txt$(in$)
  LOCAL out$
  LOCAL i%,c%

  for i%=1 to len(in$)
    c% = asc(mid$(in$,i%,1))
    if c%=asc("/") then
      out$=out$+","
    elseif c%=asc("(") then
      out$=out$+"/"
    elseif c%=asc(")") then
      out$=out$+"@"
    elseif c%>=48 and c%<58 then
      out$=out$+number$(c%-48)
    elseif c%>=65 and c%<91 then
      out$=out$+alpha$(c%-65)
    elseif c%>=97 and c%<123 then
      out$=out$+alpha$(c%-97)
    else
      out$=out$+chr$(c%)
    endif
  next
  
  txt$=out$
end function

function num$(i)
  LOCAL num$ = str$(i)
  num$ = txt$(num$)
end function

number$(0) = "0" + chr$(42)
number$(1) = "1!"
number$(2) = "2" + chr$(34)
number$(3) = "3" + chr$(35)
number$(4) = "4$"
number$(5) = "5%"
number$(6) = "6&"
number$(7) = "7" + chr$(39)
number$(8) = "8("
number$(9) = "9)"

alpha$(0) = "Aa"
alpha$(1) = "Bb"
alpha$(2) = "Cc"
alpha$(3) = "Dd"
alpha$(4) = "Ee"
alpha$(5) = "Ff"
alpha$(6) = "Gg"
alpha$(7) = "Hh"
alpha$(8) = "Ii"
alpha$(9) = "Jj"
alpha$(10) = "Kk"
alpha$(11) = "Ll"
alpha$(12) = "Mm"
alpha$(13) = "Nn"
alpha$(14) = "Oo"
alpha$(15) = "Pp"
alpha$(16) = "Qq"
alpha$(17) = "Rr"
alpha$(18) = "Ss"
alpha$(19) = "Tt"
alpha$(20) = "Uu"
alpha$(21) = "Vv"
alpha$(22) = "Ww"
alpha$(23) = "Xx"
alpha$(24) = "Yy"
alpha$(25) = "Zz"


