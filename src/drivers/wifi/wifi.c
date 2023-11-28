/**
 * @file wifi.c
 *
 * @author Jurgen Tan
 *
 * @brief This file contains the implementation of a TCP server using lwIP.
 * The TCP server is initialized using tcp_server_init() and closed using
 * tcp_server_close(). The server sends data to the client using
 * wifi_tcp_server_send_data() and receives data from the client using
 * wifi_tcp_server_recv(). Callback functions for sent data and connection
 * result are defined in tcp_server_sent() and tcp_server_result() respectively.
 *
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include "pico/cyw43_arch.h"
#include "cyw43_configport.h"
#include "cyw43_ll.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/arch.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/tcpbase.h"
#include "pico/time.h"

#include "drivers/wifi/wifi.h"

// Definitions.
// -----------------------------------------------------------------------------
//
/** @brief Port to listen on. */
#define WIFI_TCP_PORT 4242
/** @brief Poll time in seconds. */
#define WIFI_POLL_TIME_S 20
/** @brief Maximum length of a received message. */
#define WIFI_MAX_MESSAGE_LENGTH 1024

// Private function prototypes.
// -----------------------------------------------------------------------------
//

static wifi_tcp_server_t *tcp_server_init(void);
static err_t              tcp_server_close(void *p_arg);
static err_t              tcp_server_result(__unused void *p_arg, int status);
static err_t              tcp_server_sent(void                    *p_arg,
                                          __unused struct tcp_pcb *p_tpcb,
                                          u16_t                    len);
static err_t tcp_server_poll(void *p_arg, __unused struct tcp_pcb *p_tpcb);
static void  tcp_server_err(void *p_arg, err_t err);
static err_t tcp_server_accept(void           *p_arg,
                               struct tcp_pcb *p_client_pcb,
                               err_t           err);
static bool  tcp_server_open(void *p_arg);

// Public function definitions.
// -----------------------------------------------------------------------------
//

/**
 * @brief This function is a callback from lwIP, so cyw43_arch_lwip_begin is not
 * required. However, you can use this method to cause an assertion in debug
 * mode if this method is called when cyw43_arch_lwip_begin IS needed.
 *
 * @param[in] p_arg A void pointer to the wifi_tcp_server_t struct.
 * @param[in] p_tpcb A pointer to the tcp_pcb struct.
 *
 * @return err_t value indicating success or failure.
 */
err_t
wifi_tcp_server_send_data (void *p_arg, struct tcp_pcb *p_tpcb)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    for (int i = 0; i < WIFI_BUFFER_SIZE; i++)
    {
        // p_state->buffer_sent[i] = rand();
    }

    p_state->sent_len = 0;
    DEBUG_PRINT("Writing %ld bytes to client\n", WIFI_BUFFER_SIZE);
    // This method is callback from lwIP, so @ref cyw43_arch_lwip_begin is not
    // required, however you can use this method to cause an assertion in debug
    // mode, if this method is called when @ref cyw43_arch_lwip_begin IS needed.
    //
    cyw43_arch_lwip_check();
    err_t err = tcp_write(
        p_tpcb, p_state->buffer_sent, WIFI_BUFFER_SIZE, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        DEBUG_PRINT("Failed to write data %d\n", err);
        return tcp_server_result(p_arg, -1);
    }
    return ERR_OK;
}

/**
 * @brief This function is called by lwIP when data is received from a TCP
 * client. It receives the data and checks if a complete message has been
 * received (e.g., terminated by a newline character). If a complete message has
 * been received, it null-terminates the received data, prints it to the
 * console, and resets the receive buffer.
 *
 * @param[in] p_arg Pointer to the TCP server state structure.
 * @param[in] p_tpcb Pointer to the TCP protocol control block (Unused).
 * @param[in] p_buf Pointer to the received data buffer.
 * @param[in] err Error code (Unused).
 *
 * @return err_t Error code.
 */
err_t
wifi_tcp_server_recv (void                    *p_arg,
                      __unused struct tcp_pcb *p_tpcb,
                      struct pbuf             *p_buf,
                      __unused err_t           err)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    if (!p_buf)
    {
        return tcp_server_result(p_arg, -1);
    }
    // This method is callback from lwIP, so @ref cyw43_arch_lwip_begin is not
    // required, however you can use this method to cause an assertion in debug
    // mode, if this method is called when @ref cyw43_arch_lwip_begin IS needed.
    //
    cyw43_arch_lwip_check();
    if (p_buf->tot_len > 0)
    {
        // Receive the buffer
        const uint16_t buffer_left = WIFI_BUFFER_SIZE - p_state->recv_len;
        p_state->recv_len += pbuf_copy_partial(
            p_buf,
            p_state->buffer_recv + p_state->recv_len,
            p_buf->tot_len > buffer_left ? buffer_left : p_buf->tot_len,
            0);
        tcp_recved(p_tpcb, p_buf->tot_len);

        // Check if a complete message has been received (e.g., terminated by a
        // newline character).
        //
        if (p_state->recv_len > 0
            && p_state->buffer_recv[p_state->recv_len - 1] == '\n')
        {
            p_state->buffer_recv[p_state->recv_len]
                = '\0'; // Null-terminate the received data

            // Process the received message or respond to it as needed
            // Here, we're just printing it to the console
            printf("Received message: %s\n", p_state->buffer_recv);

            // Reset the receive buffer
            p_state->recv_len = 0;
        }
    }
    pbuf_free(p_buf);
    return wifi_tcp_server_send_data(p_arg, p_state->p_client_pcb);
}

/**
 * @brief Runs a TCP server and opens it. Then, it enters a loop to check for
 * Wi-Fi driver or lwIP work that needs to be done. If using
 * pico_cyw43_arch_poll, it polls periodically from the main loop to check for
 * work. Otherwise, it sleeps for 1000ms.
 *
 * @note The code contains a note that assumes certain conditions have been met
 * before the function can be executed. These conditions include the inclusion
 * of necessary libraries and header files, definition of macros, structs and
 * functions, allocation of memory, setup of hardware and network
 * configurations, granting of permissions, opening of necessary ports,
 * implementation of error handling, and cleanup.
 */
void
wifi_run_tcp_server_test (void)
{
    wifi_tcp_server_t *p_state = tcp_server_init();
    if (!p_state)
    {
        return;
    }
    if (!tcp_server_open(p_state))
    {
        tcp_server_result(p_state, -1);
        return;
    }
    while (!p_state->is_complete)
    {
        // the following #ifdef is only here so this same example can be used in
        // multiple modes; you do not need it in your code
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll
        // periodically from your main loop (not from a timer) to check for
        // Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        // you can poll as often as you like, however if you have nothing else
        // to do you can choose to sleep until either a specified time, or
        // cyw43_arch_poll() has work to do:
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
        // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP
        // work is done via interrupt in the background. This sleep is just an
        // example of some (blocking) work you might be doing.
        sleep_ms(1000);
#endif
    }
    free(p_state);
}

/**
 * @brief Initializes the TCP server by initializing standard input/output,
 * initializing the CYW43 architecture, and enabling station mode.
 */
void
wifi_tcp_server_begin_init (void)
{
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
}

/**
 * @brief Begins a TCP server by connecting to Wi-Fi with the given SSID and
 * password.
 */
void
wifi_tcp_server_begin (void)
{

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
    }
    else
    {
        printf("Connected.\n");
    }
    wifi_run_tcp_server_test();
    cyw43_arch_deinit();
}

//  Private functions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Initializes a TCP server state.
 *
 * @return A pointer to the initialized wifi_tcp_server_t struct, or NULL if
 * allocation failed.
 */
static wifi_tcp_server_t *
tcp_server_init (void)
{
    wifi_tcp_server_t *p_state = calloc(1, sizeof(wifi_tcp_server_t));
    if (!p_state)
    {
        DEBUG_PRINT("failed to allocate state\n");
        return NULL;
    }
    return p_state;
}

/**
 * @brief Closes the TCP server connection and frees the memory allocated for
 * the connection state.
 *
 * @param p_arg Pointer to the TCP server connection state.
 *
 * @return err_t Returns ERR_OK if the connection was closed successfully,
 * ERR_ABRT if the connection was aborted, or an error code if the connection
 * could not be closed.
 */
static err_t
tcp_server_close (void *p_arg)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    err_t              err     = ERR_OK;
    if (p_state->p_client_pcb != NULL)
    {
        tcp_arg(p_state->p_client_pcb, NULL);
        tcp_poll(p_state->p_client_pcb, NULL, 0);
        tcp_sent(p_state->p_client_pcb, NULL);
        tcp_recv(p_state->p_client_pcb, NULL);
        tcp_err(p_state->p_client_pcb, NULL);
        err = tcp_close(p_state->p_client_pcb);
        if (err != ERR_OK)
        {
            DEBUG_PRINT("close failed %d, calling abort\n", err);
            tcp_abort(p_state->p_client_pcb);
            err = ERR_ABRT;
        }
        p_state->p_client_pcb = NULL;
    }
    if (p_state->p_server_pcb)
    {
        tcp_arg(p_state->p_server_pcb, NULL);
        tcp_close(p_state->p_server_pcb);
        p_state->p_server_pcb = NULL;
    }
    return err;
}

/**
 * @brief Callback function for TCP server connection result.
 *
 * @param p_arg pointer to the argument passed to tcp_connect.
 * @param status indicates the status of the connection result. 0 indicates
 * success, while other values indicate failure.
 *
 * @return ERR_OK if the function executes successfully.
 */
static err_t
tcp_server_result (__unused void *p_arg, int status)
{
    if (status == 0)
    {
        DEBUG_PRINT("test success\n");
    }
    else
    {
        // handle failure
    }
    return ERR_OK;
}

/**
 * @brief Callback function for TCP server to handle sent data.
 *
 * @param p_arg pointer to the wifi_tcp_server_t struct.
 * @param p_tpcb pointer to the TCP control block.
 * @param len length of the sent data.
 *
 * @return ERR_OK if successful.
 */
static err_t
tcp_server_sent (void *p_arg, __unused struct tcp_pcb *p_tpcb, u16_t len)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    DEBUG_PRINT("tcp_server_sent %u\n", len);
    p_state->sent_len += len;

    if (p_state->sent_len >= WIFI_BUFFER_SIZE)
    {
        // We should get the data back from the client
        p_state->recv_len = 0;
        DEBUG_PRINT("Waiting for buffer from client\n");
    }

    return ERR_OK;
}

/**
 * @brief Polls the TCP server.
 *
 * @param p_arg The argument to pass to the callback function.
 * @param p_tpcb The TCP control block.
 *
 * @return err_t The result of the TCP server.
 */
static err_t
tcp_server_poll (void *p_arg, __unused struct tcp_pcb *p_tpcb)
{
    return tcp_server_result(p_arg, -1); // no response is an error?
}

/**
 * @brief This function handles the errors that occurs in the TCP server. If the
 * error is not an abort error, it calls the tcp_server_result() function with
 * the given arguments.
 *
 * @param p_arg Pointer to the argument passed to the TCP server.
 * @param err Error code returned by the TCP server.
 */
static void
tcp_server_err (void *p_arg, err_t err)
{
    if (err != ERR_ABRT)
    {
        tcp_server_result(p_arg, err);
    }
}

/**
 * @brief Function called when a TCP client connects to the server.
 *
 * @param p_arg Pointer to the TCP server state structure.
 * @param p_client_pcb Pointer to the client's PCB (Protocol Control Block).
 * @param err Error code returned by the TCP stack.
 *
 * @return err_t Error code returned by the TCP stack.
 */
static err_t
tcp_server_accept (void *p_arg, struct tcp_pcb *p_client_pcb, err_t err)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    if (err != ERR_OK || p_client_pcb == NULL)
    {
        DEBUG_PRINT("Failure in accept\n");
        tcp_server_result(p_arg, err);
        return ERR_VAL;
    }
    DEBUG_PRINT("Client connected\n");

    p_state->p_client_pcb = p_client_pcb;
    tcp_arg(p_client_pcb, p_state);
    tcp_sent(p_client_pcb, tcp_server_sent);
    tcp_recv(p_client_pcb, wifi_tcp_server_recv);
    tcp_poll(p_client_pcb, tcp_server_poll, WIFI_POLL_TIME_S * 2);
    tcp_err(p_client_pcb, tcp_server_err);

    return wifi_tcp_server_send_data(p_arg, p_state->p_client_pcb);
}

/**
 * @brief Opens a TCP server and listens for incoming connections on a specified
 * port.
 *
 * @param p_arg A pointer to a wifi_tcp_server_t struct that contains the server
 * state.
 *
 * @return true if the server was successfully started, false otherwise.
 */
static bool
tcp_server_open (void *p_arg)
{
    wifi_tcp_server_t *p_state = (wifi_tcp_server_t *)p_arg;
    DEBUG_PRINT("Starting server at %s on port %u\n",
                ip4addr_ntoa(netif_ip4_addr(netif_list)),
                WIFI_TCP_PORT);

    struct tcp_pcb *p_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!p_pcb)
    {
        DEBUG_PRINT("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(p_pcb, NULL, WIFI_TCP_PORT);
    if (err)
    {
        DEBUG_PRINT("failed to bind to port %u\n", WIFI_TCP_PORT);
        return false;
    }

    p_state->p_server_pcb = tcp_listen_with_backlog(p_pcb, 1);
    if (!p_state->p_server_pcb)
    {
        DEBUG_PRINT("failed to listen\n");
        if (p_pcb)
        {
            tcp_close(p_pcb);
        }
        return false;
    }

    tcp_arg(p_state->p_server_pcb, p_state);
    tcp_accept(p_state->p_server_pcb, tcp_server_accept);

    return true;
}

// End of file driver/wifi/wifi.c.