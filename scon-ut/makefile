################################################################################
## CONFIGURATION
################################################################################
CC		= gcc -ggdb3 -DCMOCKA -Wall -Wno-implicit-function-declaration -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-comment -std=c99
GCOV 		= gcov
COVFLAGS	= -ggdb3 -O0 --coverage -fno-exceptions -fno-inline
COMMON_DIR      = sub

LIBS		= $(COMMON_DIR)/cmocka/libcmocka-static.a


INC_DIRS  = $(COMMON_DIR)/cmocka
# INC_DIRS += $(COMMON_DIR)/cmocka/autoGenHeaders
# INC_DIRS += $(COMMON_DIR)/cmocka/Scom
# INC_DIRS += $(COMMON_DIR)/cmocka/Scom/Sciopta
# INC_DIRS += $(COMMON_DIR)/codegen

# -------------------------------------
# Filter for c-header files
# -------------------------------------

# BLK_LIST_INC += $(COMMON_DIR)/src/sciopta/bsp/ppc/mpc5xxx/include  #(types.h)not required need to remove at last

INC_DIRS_FILTERED = $(filter-out , $(INC_DIRS))

CPPFLAGS =
CPPFLAGS += @$(INCLUDE_PATHS_INC)

MAKE_DIR   = $(COMMON_DIR)/src/mk
BUILD_DIR  = build/tests
CMOCKA_DIR = cmocka

-include src/mk/sources.mk

TESTS =

include $(MAKE_DIR)/colours.mk
include $(MAKE_DIR)/functions.mk

include $(COMMON_DIR)/cmocka/tests.mk
# include $(COMMON_DIR)/cmocka/wrap.mk
# -include cmocka/tests_pcb.mk


# ------------------------------------------------------------------------------
# Filter for c files only
# ------------------------------------------------------------------------------
PROD_SRC_C   = $(filter %.c, $(HWC_SRCS))
TESTS_SRC_C  = $(filter %.c, $(TESTS_SRC))
# WRAP_SRC_C   = $(addprefix $(COMMON_DIR)/, $(filter %.c, $(WRAP_SRC)))

# ------------------------------------------------------------------------------
# Derive test modules for each c-file
# ------------------------------------------------------------------------------
TESTS_UNIT_SRC_C   =  $(addprefix $(COMMON_DIR)/$(CMOCKA_DIR)/, $(patsubst %.c, %_test.c, $(TESTS_SRC_C)))

# ------------------------------------------------------------------------------
# C files for tests
# ------------------------------------------------------------------------------
PROD_OBJS_C       = $(addprefix $(BUILD_DIR)/,  $(PROD_SRC_C:.c=.o))
# WRAP_OBJS_C       = $(addprefix $(BUILD_DIR)/,  $(WRAP_SRC_C:.c=.o))
TESTS_OBJS_C  	  = $(addprefix $(BUILD_DIR)/$(COMMON_DIR)/,  $(TESTS_SRC_C:.c=.o))
TESTS_UNIT_OBJS_C = $(addprefix $(BUILD_DIR)/,  $(TESTS_UNIT_SRC_C:.c=.o))

# Removed object files already covered by TESTS_OBJS_C
PROD_OBJS_C_F       = $(filter-out $(TESTS_OBJS_C), $(PROD_OBJS_C))

# ------------------------------------------------------------------------------
# Object files for tests
# ------------------------------------------------------------------------------
TESTS_OBJS 		= $(TESTS_OBJS_C)
TESTS_UNIT_OBJS = $(TESTS_UNIT_OBJS_C)

# ------------------------------------------------------------------------------
# Get name for test executable
# ------------------------------------------------------------------------------
TESTS_EXE       = $(TESTS_UNIT_OBJS_C:.o=.exe)

# ------------------------------------------------------------------------------
# IncludePaths
# ------------------------------------------------------------------------------
INCLUDE_PATHS_INC=$(BUILD_DIR)/include_paths.inc

# ------------------------------------------------------------------------------
# Create object files
# ------------------------------------------------------------------------------
# $(WRAP_OBJS_C) : $(BUILD_DIR)/%.o : %.c | $(INCLUDE_PATHS_INC)
# 	$(call print_command,CC, $<,-o, $@ ,-c $(CPPFLAGS))
# 	@mkdir -p $(patsubst %/,%,$(dir $@)) & \
# 	$(CC) $(CPPFLAGS) $< -o $@ -c

$(TESTS_UNIT_OBJS_C) : $(BUILD_DIR)/%.o : %.c | $(INCLUDE_PATHS_INC)
	$(call print_command,CC, $<,-o, $@ ,-c $(CPPFLAGS))
	@mkdir -p $(patsubst %/,%,$(dir $@)) & \
	$(CC) $(CPPFLAGS) $< -o $@ -c

$(PROD_OBJS_C_F) : $(BUILD_DIR)/%.o : %.c | $(INCLUDE_PATHS_INC)
	$(call print_command,CC, $<,-o, $@ ,-c $(CPPFLAGS))
	@mkdir -p $(patsubst %/,%,$(dir $@))  & \
	$(CC) $(COVFLAGS) $(CPPFLAGS) -w $< -o $@ -c

$(TESTS_OBJS_C) : $(BUILD_DIR)/%.o : %.c | $(INCLUDE_PATHS_INC)
	$(call print_command,CC, $<,-o, $@ ,-c $(CPPFLAGS))
	@mkdir -p $(patsubst %/,%,$(dir $@))  & \
	$(CC) $(COVFLAGS) $(CPPFLAGS) -w $< -o $@ -c

# ------------------------------------------------------------------------------
# Create executables
# ------------------------------------------------------------------------------

$(TESTS_EXE): %.exe : %.o $(TESTS_OBJS_C) $(TESTS_UNIT_OBJS_C) #$(WRAP_OBJS_C)
	$(call print_command,CC $(COVFLAGS), -o $@, $(@:.exe=.o)  $(patsubst $(BUILD_DIR)/$(COMMON_DIR)/$(CMOCKA_DIR)/%,$(BUILD_DIR)/$(COMMON_DIR)/%, $(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,$(BUILD_DIR)/%, $(@:_test.exe=.o))) $(LIBS) WRAP_OBJECTS STUB_FUNCTIONS)
	@bash -c "$(patsubst $(BUILD_DIR)/%.exe, ./%.sh, $@) $(COMMON_DIR)/ $(CC) $(COVFLAGS) -o $@ $(@:.exe=.o) $(patsubst $(BUILD_DIR)/$(COMMON_DIR)/$(CMOCKA_DIR)/%,$(BUILD_DIR)/$(COMMON_DIR)/%, $(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,$(BUILD_DIR)/%, $(@:_test.exe=.o))) $(LIBS)"
	@mkdir -p $(BUILD_DIR)/testresults
	@rm -f $(BUILD_DIR)/testresults/$(subst /,_,$(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,%,$(@:.exe=.xml)))
	-@CMOCKA_XML_FILE=$(BUILD_DIR)/testresults/$(subst /,_,$(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,%,$(@:.exe=.xml))) ./$@  \
	$(GCOV) -b -c $(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,%, $(@:_test.exe=.c)) -o $(patsubst $(BUILD_DIR)/$(CMOCKA_DIR)/%,$(BUILD_DIR)/%, $(@:_test.exe=.o))

# ------------------------------------------------------------------------------
# get coverage report
# ------------------------------------------------------------------------------
$(BUILD_DIR)/coverage.info :  $(PROD_OBJS_C_F) $(TESTS_OBJS_C) $(TESTS_UNIT_OBJS_C) $(TESTS_EXE)
	lcov --rc lcov_branch_coverage=1 --capture --directory ./ --output-file $(BUILD_DIR)/coverage.info
	genhtml --branch-coverage $(BUILD_DIR)/coverage.info --output-directory $(BUILD_DIR)/covreport

# ------------------------------------------------------------------------------
# IncludePaths
# ------------------------------------------------------------------------------
$(INCLUDE_PATHS_INC): src/mk/sources.mk
	$(call print_command,echo, ,-o, $@ ,)
	@mkdir -p $(patsubst %/,%,$(dir $@))
	@echo $(addprefix -I,$(INC_DIRS_FILTERED)) | sed -e 's/ /\n/g' > $@



# .PHONY : wrap_only
# wrap_only: $(WRAP_OBJS_C)

.PHONY : prod_only
prod_only: $(PROD_OBJS_C_F)

# ------------------------------------------------------------------------------
# execute all tests
# ------------------------------------------------------------------------------
.PHONY : tests
tests: $(TESTS_EXE) $(TESTS_OBJS_C)

# ------------------------------------------------------------------------------
# create report by xunit-viewer
# ------------------------------------------------------------------------------
.PHONY : report
report: $(BUILD_DIR)/coverage.info
	mkdir -p $(BUILD_DIR)/testreport/ & \
	xunit-viewer -r $(BUILD_DIR)/testresults/ -o $(BUILD_DIR)/testreport/report.html
# ------------------------------------------------------------------------------
# do all
# ------------------------------------------------------------------------------

.PHONY : tests
all: $(BUILD_DIR)/coverage.info
# ------------------------------------------------------------------------------
# clean
# ------------------------------------------------------------------------------
.PHONY : clean
clean:
	rm -R build/*

	@mkdir -p $(BUILD_DIR)/cmocka
	@mkdir -p $(BUILD_DIR)/src/
	@mkdir -p $(BUILD_DIR)/cmocka/
	@mkdir -p $(BUILD_DIR)/testresults
	@mkdir -p $(BUILD_DIR)/testreport/
	@mkdir -p $(BUILD_DIR)/covreport/

