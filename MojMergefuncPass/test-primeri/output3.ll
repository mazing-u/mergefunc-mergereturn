; ModuleID = 'input3.ll'
source_filename = "input3.ll"

define i32 @func_one(i32 %x) {
  %res = mul i32 %x, 5
  ret i32 %res
}

define i32 @func_two(i32 %x) {
  %res = mul i32 %x, 6
  ret i32 %res
}
