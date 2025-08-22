
# Create the final program verriable
TARGET_EXEC := CProgram

# Init the directories
BUILD_DIR := ./build
SRC_DIR := ./src
TST_DIR := ./tests
INC_DIR := ./includes
LIB_DIR := ./libs

# Set explicit compilers
CC := gcc
CXX := g++


# Find all .c source files in source directory location
SRCS := $(wildcard $(SRC_DIR)/**/*.c)
SRCS += main.c # since it is outide src
# SRCS := \
#   $(wildcard $(SRC_DIR)/*.c) \
#   $(wildcard $(SRC_DIR)/*/*.c) \
#   $(wildcard $(SRC_DIR)/*/*/*.c)

# Collecting Compile & dependency Files
# -------------------------------------

# 1. Creates a seperate list of objects that need to be compiled from all the srouce files in SRCS.
# It appends a .o to the end of it so the type is known and seperate.
OBJS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.c=.o))


# 2. Creates another seperate list of objects from the copiled list, where the .o is replaced by a .d
# This simbolyses a dependency file and contains the objects dependent files. 
DEPS := $(OBJS:.o=.d)

# Configuring Include Directories
# -------------------------------

# 1. Recursivley itterates through source directory and writes each directory to the INC_DIRS list. 
# Sympoliesed by -type d(directory) for all directories than can contain include files.

# Update this with a python script or something similar to find all directories containing .h files, rather than just all directories. 

#INC_DIRS := $(wildcard $(SRC_DIR)/*) $(wildcard $(INC_DIR)/*)
INC_DIRS := $(filter %/, $(wildcard $(INC_DIR)/*/)) \
		    $(filter %/, $(wildcard $(INC_DIR)/*/inc/**/)) \
			$(filter %/, $(wildcard $(INC_DIR)/*/inc/**/**/)) \
			$(filter %/, $(wildcard $(LIB_DIR)/*/inc/)) \
			$(filter %/, $(wildcard $(LIB_DIR)/*/inc/**/)) \
			$(filter %/, $(wildcard $(LIB_DIR)/*/inc/**/**/))
#INC_DIRS := $(shell dir /B /S /AD includes libraries 2>NUL)

# 2. Creates a sperate list from the include directories with the directories containing an appended -I infront
# This tells the compiler to look here for includes
#INC_FLAGS := $(addprefix -I,$(INC_DIRS))
INC_FLAGS := $(foreach dir,$(INC_DIRS),-I"$(dir)")

# 3. Internal make veriable to generate make files for dependency matching (-MMD) and Phony Targets (-MP) to stop errors
# on missing files. This is done in all listed directories. 
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# Configuring Library Directories
# -------------------------------

# 1. Find all lib files we want to link.
LIB_FILES := $(wildcard $(LIB_DIR)/*/lib/*.a)
#LIB_FILES := $(filter %/, $(wildcard $(LIB_DIR)/*/lib)) $(filter %/, $(wildcard $(LIB_DIR)/*/lib/**/))
#LIB_FILES := $(shell dir /B /S libraries\*.lib libraries\*.a 2>NUL)

#LIB_FILES_TMP := $(wildcard $(LIB_DIR)/*/lib/) $(wildcard $(LIB_DIR)/*/lib/*/)
#LIB_FILES := $(filter %/, $(LIB_FILES_TMP))

# 2. Get the lib file directories
LIB_DIRS := $(sort $(dir $(LIB_FILES)))

# 3. Add prefix to -L to indicate its a library directory for linking.
LIB_FLAGS := $(addprefix -L,$(LIB_DIRS))

# 4. Update Flags to include libs
LDFLAGS := $(LIB_FLAGS) -lglfw3 -lgdi32 -lopengl32 -luser32 -lkernel32 -lws2_32 -lpthread -lvulkan-1

# Conducting the compilation and build
# ------------------------------------

# Debug prints all elements int SRCS
$(info INC_DIRS = $(INC_DIRS))
$(info LIB_FILES = $(LIB_FILES))
#$(info INC_FLAGS = $(INC_FLAGS))
$(info LIB_FLAGS = $(LIB_FLAGS))
#$(info OBJS = $(OBJS))
#$(info SRCS = $(SRCS))

# 0. Default target
all: $(BUILD_DIR)/$(TARGET_EXEC)

# 1. Generates the actuall executable file, requires all compiled OBJS file as input.
# CXX is internal veriable for g++ compiler. $@ = target (name). LDFLAGS are for additional internal flags. 
$(BUILD_DIR)/$(TARGET_EXEC) : CopyDlls $(OBJS) 
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# 2. Compiles all c files in build directories, mkdir -p makes sure it exists with $(dir $@) telling it where to find/make it.
# $(CC) = the gcc compiler, $(CPPFLAGS) are flags for -I and other include files, $(CFLAGS) internal flags, -c $< -o $@ gets the source file.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(shell mkdir $(dir $@) 2>NUL)
	$(shell mkdir $(subst /,\,$(dir $@)) 2>NUL)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@	

# 3. Compiles all c++ files in build direcoties, 
# only difference is the g++ (CXX) compiler rahter than gcc.
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(shell mkdir $(dir $@) 2>NUL)
	$(shell mkdir $(subst /,\,$(dir $@)) 2>NUL)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

CopyDlls: 
	@for %%f in ($(LIB_DIR)/**/*.dll) do copy "%%f" $(BUILD_DIR)


# Cleaning
# --------

# Defines RM veriable relating to operating system to clean build directory.
ifeq ($(OS),Windows_NT)
RM = if exist $(subst /,\,$(BUILD_DIR)) rmdir /S /Q $(subst /,\,$(BUILD_DIR))
else
RM = rm -rf $(BUILD_DIR)
endif

.PHONY: clean
clean:
	$(RM)


# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)