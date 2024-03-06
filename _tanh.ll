; ModuleID = '../../funcglibc/_tanh.c'
source_filename = "../../funcglibc/_tanh.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree nounwind uwtable willreturn writeonly
define dso_local double @_tanh(double noundef %0) local_unnamed_addr #0 {
  %2 = bitcast double %0 to i64
  %3 = lshr i64 %2, 32
  %4 = trunc i64 %3 to i32
  %5 = trunc i64 %2 to i32
  %6 = and i32 %4, 2147483647
  %7 = icmp ugt i32 %6, 2146435071
  br i1 %7, label %8, label %15

8:                                                ; preds = %1
  %9 = icmp sgt i64 %2, -1
  %10 = fdiv double 1.000000e+00, %0
  br i1 %9, label %11, label %13

11:                                               ; preds = %8
  %12 = fadd double %10, 1.000000e+00
  br label %45

13:                                               ; preds = %8
  %14 = fadd double %10, -1.000000e+00
  br label %45

15:                                               ; preds = %1
  %16 = icmp ult i32 %6, 1077280768
  br i1 %16, label %17, label %40

17:                                               ; preds = %15
  %18 = or i32 %6, %5
  %19 = icmp eq i32 %18, 0
  br i1 %19, label %45, label %20

20:                                               ; preds = %17
  %21 = icmp ult i32 %6, 1015021568
  br i1 %21, label %22, label %25

22:                                               ; preds = %20
  %23 = fadd double %0, 1.000000e+00
  %24 = fmul double %23, %0
  br label %45

25:                                               ; preds = %20
  %26 = icmp ugt i32 %6, 1072693247
  %27 = call double @llvm.fabs.f64(double %0)
  br i1 %26, label %28, label %34

28:                                               ; preds = %25
  %29 = fmul double %27, 2.000000e+00
  %30 = call double @expm1(double noundef %29) #3
  %31 = fadd double %30, 2.000000e+00
  %32 = fdiv double 2.000000e+00, %31
  %33 = fsub double 1.000000e+00, %32
  br label %40

34:                                               ; preds = %25
  %35 = fmul double %27, -2.000000e+00
  %36 = call double @expm1(double noundef %35) #3
  %37 = fneg double %36
  %38 = fadd double %36, 2.000000e+00
  %39 = fdiv double %37, %38
  br label %40

40:                                               ; preds = %15, %28, %34
  %41 = phi double [ %33, %28 ], [ %39, %34 ], [ 1.000000e+00, %15 ]
  %42 = icmp sgt i64 %2, -1
  %43 = fneg double %41
  %44 = select i1 %42, double %41, double %43
  br label %45

45:                                               ; preds = %17, %40, %22, %13, %11
  %46 = phi double [ %12, %11 ], [ %14, %13 ], [ %24, %22 ], [ %44, %40 ], [ %0, %17 ]
  ret double %46
}

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @expm1(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.fabs.f64(double) #2

; Function Attrs: mustprogress nofree nounwind uwtable willreturn writeonly
define dso_local i32 @main() local_unnamed_addr #0 {
  ret i32 0
}

attributes #0 = { mustprogress nofree nounwind uwtable willreturn writeonly "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind willreturn writeonly "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree nosync nounwind readnone speculatable willreturn }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
