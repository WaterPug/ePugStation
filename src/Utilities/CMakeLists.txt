add_library(Utilities Constants.h
					  Instruction.h
					  OpUtilities.h
					  OpUtilities.cpp
					  Range.h
					  Utils.h)
					  
set_target_properties(Utilities PROPERTIES LINKER_LANGUAGE CXX)