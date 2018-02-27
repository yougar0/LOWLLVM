SOURCE :=
EXT_CC := .cc
EXT_BC := .bc
EXT_ORIGINAL := .orig
EXT_S := .s
EXT_OPT := .opt 
LIB := libtrob.so
TYPE := -flatten

compile:
	clang++ $(SOURCE)$(EXT_CC) -o $(SOURCE)$(EXT_ORIGINAL)
	clang++ -emit-llvm $(SOURCE)$(EXT_CC) -c -o $(SOURCE)$(EXT_BC)
	opt -load ./$(LIB) $(TYPE) < $(SOURCE)$(EXT_BC) > $(SOURCE).2$(EXT_BC)
	llc $(SOURCE).2$(EXT_BC) -o $(SOURCE)$(EXT_S)
	clang++ $(SOURCE)$(EXT_S) -o $(SOURCE)$(EXT_OPT)
	rm $(SOURCE)$(EXT_BC) $(SOURCE).2$(EXT_BC) $(SOURCE)$(EXT_S)

clean:
	rm $(SOURCE)$(EXT_ORIGINAL) $(SOURCE)$(EXT_OPT)

clean-so:
	rm $(LIB)