#pragma once

#include "Support/wait.h"

#define WAIT_FOR(pred, time)                                                   \
  ASSERT_TRUE(support::waitFor<bool>(pred, true, time))
