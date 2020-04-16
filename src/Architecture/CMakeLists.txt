add_library(Architecture BIOS.cpp
					     BIOS.h
						 CPU.cpp
						 CPU.h
						 Interconnect.cpp
						 Interconnect.h
						 Ram.cpp
						 Ram.h)
						 
target_link_libraries(Architecture PUBLIC Utilities)
