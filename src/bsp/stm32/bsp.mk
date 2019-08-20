
BSP_INCLUDE_PATHS :=											\
  -I$(SRCDIR)/../bsp/stm32 										\
  -I$(SRCDIR)/$(BSP)/CMSIS/CM3/DeviceSupport/ST/STM32F10x 		\
  -I$(SRCDIR)/$(BSP)/CMSIS/CM3/CoreSupport/ 					\
  -I$(SRCDIR)/$(BSP)/STM32F10x_StdPeriph_Driver/inc/

BSP_SRCS := debug.c												\
       timer.c													\
       CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c	\
       STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c			\
       STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c			\
       STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c

BSP_LDFLAGS := -D STM32F10X_MD_VL
BSP_CFLAGS := -D STM32F10X_MD_VL
