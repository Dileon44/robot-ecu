set(FLASH_BOOT_ADDR 0x08000000)
set(FLASH_BOOT_SIZE 0x4000) # 16K
set(FLASH_APP_ADDR 0x08000000)
set(FLASH_APP_SIZE 0x18000)  # 98K

include_directories(
	${CMAKE_SOURCE_DIR}/platform/m0
	# ${CMAKE_SOURCE_DIR}/platform/m0/c1
	${CMAKE_SOURCE_DIR}/platform/m0/c0
	${CMAKE_SOURCE_DIR}/platform/m0/core
	${CMAKE_SOURCE_DIR}/platform/m0/core/CMSIS
	${CMAKE_SOURCE_DIR}/platform/m0/core/HAL/inc
	${CMAKE_SOURCE_DIR}/platform/m0/core/HAL/inc/Legacy
	${CMAKE_SOURCE_DIR}/platform/m0/core/LL/inc
	${CMAKE_SOURCE_DIR}/platform/m0/core/USB_Device/Class/CDC/Inc
	${CMAKE_SOURCE_DIR}/app/usb
	${CMAKE_SOURCE_DIR}/app/debug
	${CMAKE_SOURCE_DIR}/platform/m0/core/USB_Device/Core/Inc
	${CMAKE_SOURCE_DIR}/app/motor/connection
)

add_compile_options(
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
	-mthumb
)

add_compile_definitions(
	USE_FULL_ASSERT
	HSE_VALUE=24000000U
	STM32G431xx
	USE_FULL_LL_DRIVER
)

add_link_options(
	-mcpu=cortex-m4
	-mfloat-abi=hard
	-mfpu=fpv4-sp-d16
	-Wl,-cref
	-Wl,-u,Reset_Handler
	-Wl,--defsym=malloc_getpagesize_P=0x80
	-Wl,-Map=${CMAKE_SOURCE_DIR}/${BUILD_DIR}/${FW_PLATFORM}.map
)
