#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "pathfinding/a_star.h"

// TODO: Setup FreeRTOS.