; ModuleID = '_tanh_RemoveSpecials.bc'
source_filename = "../../funcglibc/_tanh.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree nounwind willreturn memory(write) uwtable
define dso_local double @_tanh(double noundef %0) local_unnamed_addr #0 {
  %2 = bitcast double %0 to i64
  %3 = lshr i64 %2, 32
  %4 = trunc i64 %3 to i32
  %5 = trunc i64 %2 to i32
  %6 = and i32 %4, 2147483647
  %7 = icmp ult i32 %6, 1077280768
  br i1 %7, label %8, label %26

8:                                                ; preds = %1
  %9 = or i32 %6, %5
  %10 = icmp eq i32 %9, 0
  br i1 %10, label %31, label %11

11:                                               ; preds = %8
  %12 = icmp ugt i32 %6, 1072693247
  %13 = call double @llvm.fabs.f64(double %0)
  br i1 %12, label %14, label %20

14:                                               ; preds = %11
  %15 = fmul double %13, 2.000000e+00
  %16 = call double @expm1(double noundef %15) #3
  %17 = fadd double %16, 2.000000e+00
  %18 = fdiv double 2.000000e+00, %17
  %19 = fsub double 1.000000e+00, %18
  br label %26

20:                                               ; preds = %11
  %21 = fmul double %13, -2.000000e+00
  %22 = call double @expm1(double noundef %21) #3
  %23 = fneg double %22
  %24 = fadd double %22, 2.000000e+00
  %25 = fdiv double %23, %24
  br label %26

26:                                               ; preds = %20, %14, %1
  %27 = phi double [ %19, %14 ], [ %25, %20 ], [ 1.000000e+00, %1 ]
  %28 = icmp sgt i64 %2, -1
  %29 = fneg double %27
  %30 = select i1 %28, double %27, double %29
  br label %31

31:                                               ; preds = %26, %8
  %32 = phi double [ %30, %26 ], [ %0, %8 ]
  ret double %32
}

; Function Attrs: mustprogress nofree nounwind willreturn memory(write)
declare double @expm1(double noundef) local_unnamed_addr #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fabs.f64(double) #2

; Function Attrs: mustprogress nofree nounwind willreturn memory(write) uwtable
define dso_local i32 @main() local_unnamed_addr #0 {
  ret i32 0
}

attributes #0 = { mustprogress nofree nounwind willreturn memory(write) uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind willreturn memory(write) "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
