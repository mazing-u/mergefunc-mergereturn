
define i32 @max_x(i32 %a, i32 %b) {
  %cmp = icmp sgt i32 %a, %b
  br i1 %cmp, label %if.true, label %if.false

if.true:
  ret i32 %a

if.false:
  ret i32 %b
}

define i32 @max_y(i32 %a, i32 %b) {
  %cmp = icmp sgt i32 %a, %b
  br i1 %cmp, label %then, label %else

then:
  ret i32 %a

else:
  ret i32 %b
}
