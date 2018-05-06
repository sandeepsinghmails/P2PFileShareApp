CC = g++
TARGET = p2p
MKDIR_P = mkdir -p

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = include
BINDIR = bin

# Flags
INC_FLAGS := $(addprefix -I,$(INCDIR))
CPPFLAGS = $(INC_FLAGS) -g -pthread -std=c++11
LDFLAGS = -g -pthread -std=c++11

# Libraries
LIBS = 

# Files and folders
SRCS    = $(shell find $(SRCDIR) -name '*.cpp')
OBJS    = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
$(shell mkdir -p $(OBJDIR) $(BINDIR))


all: directories $(TARGET)

directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OBJDIR}

# Targets
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)
