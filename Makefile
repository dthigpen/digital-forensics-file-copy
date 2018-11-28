#include the make.h
include ./make.h

MODULE_OBJECT_LIST =	$(MODULE_OBJ_DIR)/recoverdocmain.o \
						$(MODULE_OBJ_DIR)/recoverdoc_util.o \
						$(MODULE_OBJ_DIR)/copyfileutil.o \
						$(MODULE_OBJ_DIR)/inodeutil.o \
						$(MODULE_OBJ_DIR)/inodeinit.o \
						$(MODULE_OBJ_DIR)/inodedir.o \
						
MODULE_FINAL_EXE = $(MODULE_EXE_DIR)/recoverdoc
#MODULE_STATIC_LIBS_DIR = -L$(BASE_DIR)/common/lib/

MODULE_LIBS_LIST = -lm
#MODULE_LIBS_LIST = -lcmn \
					-lm \
					-lrt \
					-lpthread

$(MODULE_FINAL_EXE): $(MODULE_OBJECT_LIST)
	$(CC) -g -O0 $(MODULE_OBJECT_LIST) $(MODULE_STATIC_LIBS_DIR) $(MODULE_LIBS_LIST) -o $(MODULE_FINAL_EXE)

$(MODULE_OBJ_DIR)/recoverdocmain.o: $(MODULE_SRC_DIR)/recoverdocmain.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/recoverdocmain.c -o $(MODULE_OBJ_DIR)/recoverdocmain.o
$(MODULE_OBJ_DIR)/recoverdoc.o: $(MODULE_SRC_DIR)/recoverdoc.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/recoverdoc.c -o $(MODULE_OBJ_DIR)/recoverdoc.o
$(MODULE_OBJ_DIR)/recoverdoc_util.o: $(MODULE_SRC_DIR)/recoverdoc_util.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/recoverdoc_util.c -o $(MODULE_OBJ_DIR)/recoverdoc_util.o
# $(MODULE_OBJ_DIR)/copyfilemain.o: $(MODULE_SRC_DIR)/copyfilemain.c
# 	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/copyfilemain.c -o $(MODULE_OBJ_DIR)/copyfilemain.o
$(MODULE_OBJ_DIR)/copyfileutil.o: $(MODULE_SRC_DIR)/copyfileutil.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/copyfileutil.c -o $(MODULE_OBJ_DIR)/copyfileutil.o
$(MODULE_OBJ_DIR)/inodedir.o: $(MODULE_SRC_DIR)/inodedir.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodedir.c -o $(MODULE_OBJ_DIR)/inodedir.o
$(MODULE_OBJ_DIR)/inodeinit.o: $(MODULE_SRC_DIR)/inodeinit.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodeinit.c -o $(MODULE_OBJ_DIR)/inodeinit.o
$(MODULE_OBJ_DIR)/inodeutil.o: $(MODULE_SRC_DIR)/inodeutil.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodeutil.c -o $(MODULE_OBJ_DIR)/inodeutil.o
clean:
	rm -f $(MODULE_OBJ_DIR)/*
	rm -f $(MODULE_EXE_DIR)/*						   
