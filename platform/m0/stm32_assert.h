#ifndef __STM32_ASSERT_H
#define __STM32_ASSERT_H

#include "main.h"

#ifdef USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ASSERT_CHECK(expr)
#else /* USE_FULL_ASSERT */
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32_ASSERT_H */
