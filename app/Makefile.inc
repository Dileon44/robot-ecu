COMPILER_FLAGS += -Iapp
COMPILER_FLAGS += -Ilib
COMPILER_FLAGS += -Ilib/FreeRTOS_V10.4.4/inc -Ilib/FreeRTOS_V10.4.4/portable/ARM_CM4F

SRC += $(call rwildcard,lib,*.c)
