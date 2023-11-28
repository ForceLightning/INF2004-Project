/**
 * @file wifi.h
 * @author Jurgen Tan
 *
 * @brief Header file for WiFi module.
 * This file contains the function prototypes and data structures for the WiFi
 * module. The module provides a TCP server that can be initialized and started
 * using the functions wifi_tcp_server_begin_init() and wifi_tcp_server_begin().
 * The TCP server uses lwIP library for network communication and has a buffer
 * size of 2048 bytes.
 *
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef WIFI_H // Include guard.
#define WIFI_H

#include <stdio.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

/**
 * @defgroup wifi_constants WiFi Constants
 * @brief Constants for the WiFi module.
 * @{
 */

/** @brief Buffer size for messages sent out via wifi. */
#define WIFI_BUFFER_SIZE 2048
/** @brief Port to listen on. */
#define WIFI_TCP_PORT 4242
/** @brief Poll time in seconds. */
#define WIFI_POLL_TIME_S 20
/** @brief Maximum length of a received message. */
#define WIFI_MAX_MESSAGE_LENGTH 1024
/**
 @} */ // End of wifi_constants group.

#ifndef NDEBUG
/**
 * @def DEBUG_PRINT(...)
 * @brief Macro for printing debug messages.
 * @param ... Variable number of arguments to be printed.
 */
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief Struct representing a TCP server connection.
 *
 * @note The struct contains information about a TCP server connection,
 * including the server and client PCBs, flags indicating whether the connection
 * is complete, buffers for sent and received data, and various counters for
 * tracking the state of the connection.
 */
typedef struct wifi_tcp_server
{
    struct tcp_pcb *p_server_pcb; ///< Pointer to the server PCB
    struct tcp_pcb *p_client_pcb; ///< Pointer to the client PCB
    bool    is_complete; ///< Flag indicating whether the connection is complete
    uint8_t buffer_sent[WIFI_BUFFER_SIZE]; ///< Buffer for sent data
    uint8_t buffer_recv[WIFI_BUFFER_SIZE]; ///< Buffer for received data
    int     sent_len;                      ///< Length of sent data
    int     recv_len;                      ///< Length of received data
    int     run_count; ///< Counter for tracking the state of the connection
} wifi_tcp_server_t;

// Function prototypes
// ----------------------------------------------------------------------------
//

void  wifi_tcp_server_begin_init(void);
void  wifi_tcp_server_begin(void);
err_t wifi_tcp_server_send_data(void *p_arg, struct tcp_pcb *p_tpcb);
err_t wifi_tcp_server_recv(void                    *p_arg,
                           __unused struct tcp_pcb *p_tpcb,
                           struct pbuf             *p_buf,
                           __unused err_t           err);
void  wifi_run_tcp_server_test(void);

#endif // WIFI_H

// End of file driver/wifi/wifi.h.
