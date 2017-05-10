# A reusable basic makefile:
# just replace the following variables below
TARGET := parser

SOURCE_DIRS := src
INCLUDE_DIRS := include public

LIBS := 

SRC_EXT := cpp
OBJ_EXT := o

CPPFLAGS := --std=c++11 -Wall -g


# BEGIN STUFF THAT SHOULDN'T BE CHANGED

OUTDIR := build

GLOB_SOURCES=$(wildcard $(mydir)/*.$(SRC_EXT))

SOURCES := $(foreach mydir,$(SOURCE_DIRS),$(GLOB_SOURCES))
OBJECTS := $(SOURCES:%.$(SRC_EXT)=$(OUTDIR)/%.$(OBJ_EXT))

INCLUDE_FLAGS := $(foreach myvar,$(INCLUDE_DIRS),-I$(myvar))

LIB_DIR_FLAGS := $(foreach myvar,$(dir $(LIBS)),-L$(myvar))

LIB_FILES := $(basename $(notdir $(LIBS)))
LIB_FLAGS := $(patsubst lib%,-l%,$(LIB_FILES))

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBS)
	g++ $(LIB_DIR_FLAGS) $(OBJECTS) $(LIB_FLAGS) -o $@

$(OUTDIR)/%.o : %.cpp
	mkdir -p $(@D)
	g++ $(CPPFLAGS) $(INCLUDE_FLAGS) -c $^ -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)
ifeq ($(strip $(OUTDIR)),)
	@echo "ERROR: clean attempted to remove the root directory. Your makefile is completely broken, please fix it carefully."
else
	rm -f -r $(OUTDIR)/*
endif


