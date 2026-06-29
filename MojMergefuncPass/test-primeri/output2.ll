; ModuleID = 'input2.ll'
source_filename = "input2.ll"

define i32 @max_x(i32 %a, i32 %b) {
  %cmp = icmp sgt i32 %a, %b
  br i1 %cmp, label %if.true, label %if.false

if.true:                                          ; preds = %0
  ret i32 %a

if.false:                                         ; preds = %0
  ret i32 %b
}

define private i32 @max_y(i32 %a, i32 %b) {
entry:
  %0 = musttail call i32 @max_x(i32 %a, i32 %b)
  ret i32 %0
}
