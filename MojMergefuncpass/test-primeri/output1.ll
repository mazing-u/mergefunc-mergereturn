; ModuleID = 'input.ll'
source_filename = "input.ll"

define i32 @foo(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = mul i32 %a, 2
  ret i32 %b
}

define private i32 @bar(i32 %x) {
entry:
  %0 = musttail call i32 @foo(i32 %x)
  ret i32 %0
}
