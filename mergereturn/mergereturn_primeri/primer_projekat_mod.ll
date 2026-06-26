; ModuleID = 'primer_projekat2.ll'
source_filename = "primer.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @probnaFunkcija1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  br label %13

6:                                                ; preds = %0
  %7 = load i32, ptr %2, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %6
  br label %13

10:                                               ; preds = %6
  %11 = load i32, ptr %2, align 4
  %12 = mul nsw i32 %11, 2
  br label %13

13:                                               ; preds = %5, %9, %10
  %14 = phi i32 [ %12, %10 ], [ 0, %9 ], [ -1, %5 ]
  ret i32 %14
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local float @probnaFunkcija2() #0 {
  %1 = alloca float, align 4
  %2 = alloca float, align 4
  %3 = load float, ptr %2, align 4
  %4 = fcmp olt float %3, 0.000000e+00
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  br label %13

6:                                                ; preds = %0
  %7 = load float, ptr %2, align 4
  %8 = fcmp oeq float %7, 0.000000e+00
  br i1 %8, label %9, label %10

9:                                                ; preds = %6
  br label %13

10:                                               ; preds = %6
  %11 = load float, ptr %2, align 4
  %12 = fmul float %11, 2.000000e+00
  br label %13

13:                                               ; preds = %5, %10, %9
  %14 = phi float [ 0.000000e+00, %9 ], [ %12, %10 ], [ -1.000000e+00, %5 ]
  ret float %14
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @probnaFunkcija3() #0 {
  %1 = alloca i32, align 4
  %2 = load i32, ptr %1, align 4
  %3 = icmp slt i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  store i32 -1, ptr %1, align 4
  br label %12

5:                                                ; preds = %0
  %6 = load i32, ptr %1, align 4
  %7 = icmp eq i32 %6, 0
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  store i32 0, ptr %1, align 4
  br label %12

9:                                                ; preds = %5
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 %10, 2
  store i32 %11, ptr %1, align 4
  br label %12

12:                                               ; preds = %4, %8, %9
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca float, align 4
  %3 = call i32 @probnaFunkcija1()
  store i32 %3, ptr %1, align 4
  %4 = call float @probnaFunkcija2()
  store float %4, ptr %2, align 4
  call void @probnaFunkcija3()
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.0"}
