

CONST LINECNT=16

DIM Integer cnt

sub Record.open
  on error abort
  open "record.data" for output as #2
end sub

sub Record.close
  close #2
end sub

sub Record.write(ctrl%)
  if cnt=0 then print #2,"DATA ";

  print #2,str$(ctrl%);

  inc cnt
  if cnt=LINECNT then
    print #2:
    cnt=0
  else
    print #2,",";
  endif
end sub

