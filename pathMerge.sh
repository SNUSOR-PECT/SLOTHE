/usr/local/bin/llc -filetype=obj _tanh_UnreachablePath.ll -o _tanh_UnreachablePath.o
/usr/local/bin/clang++ runFunc.cpp _tanh_UnreachablePath.o -o runFunc -lm
./runFunc