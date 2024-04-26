COMPILER1=g++
COMPILER2=mpic++

TARGET1=life
TARGET2=life_mpi

SOURCES1=life.cpp
SOURCES2=life_mpi.cpp

# include -lhdf5 if need to save data
CXXFLAGS=-pg -O0 -Werror

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SOURCES1)
	$(COMPILER1) $(CXXFLAGS) $(SOURCES1) -o $(TARGET1)
$(TARGET2): $(SOURCES2)
	$(COMPILER2) $(CXXFLAGS) $(SOURCES2) -o $(TARGET2)

clean:
	rm -f $(TARGET1) $(TARGET2)

.PHONY: all clean
