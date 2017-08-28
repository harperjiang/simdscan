# Makefile for generating programs for performance test

CXX=g++
RM=rm
GTEST=/home/harper/git/googletest/googletest

INCLUDE=

LIBS = -pthread

CXX_FLAG= -O3 -Wall -std=gnu++11 -mavx2 -c -fmessage-length=0 -pthread $(INCLUDE)

BIN = ../bin

INCLUDE_GTEST= -I $(GTEST)/include
LIB_GTEST= $(LIBS) -L $(GTEST)/make -lgtest
CXX_TESTFLAG = $(CXX_FLAG) $(INCLUDE_GTEST)


ifeq ($(OS),Windows_NT)
	EXE_SUFFIX=.exe
else
	EXE_SUFFIX=	
endif

COMMON_OBJS = \
./src/predicate/Predicate.o \
./src/scan/Scanner.o \
./src/scan/HaoScanner.o


TEST_OBJS = \
./test/predicate/Predicate_test.o \
./test/scan/HaoScanner_test.o

MAIN = main

default: $(MAIN)

$(MAIN) : %: src/%.o $(COMMON_OBJS) $(BIN)
	@echo 'Generating $@'
	$(CXX) -o $(BIN)/$@$(EXE_SUFFIX) $(COMMON_OBJS) ./src/$@.o $(LIBS)
	@echo '$@ Generated'
	@echo ' '
	

src/%.o: ../src/%.cpp mkfolder
	@echo 'Building file: $<'
	$(CXX) $(CXX_FLAG) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	

test/%.o: ../test/%.cpp mktestfolder
	@echo 'Building file: $<'
	$(CXX) $(CXX_FLAG) $(CXX_TESTFLAG) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mkfolder:
	mkdir -p $(BIN)
	mkdir -p src/scan
	mkdir -p src/predicate

mktestfolder: mkfolder
	mkdir -p test/scan
	mkdir -p test/predicate

clean:
	-$(RM) -f $(MAIN) $(COMMON_OBJS) $(EXE_OBJS) $(C_DEPS)
	-$(RM) -rf $(BIN) src test
	-@echo ' '
	
unittest: $(COMMON_OBJS) $(TEST_OBJS)
	@echo 'Generating Unit Test'
	$(CXX) -o $(BIN)/unittest $(COMMON_OBJS) $(TEST_OBJS) $(LIB_GTEST) -lgtest_main
	@echo 'Unit Test generated'
