/**
 * @file wifi.c
 * 
 * @author Jurgen Tan
 * 
 * @brief This file contains the implementation of a TCP server using lwIP.
 * The TCP server is initialized using tcp_server_init() and closed using tcp_server_close().
 * The server sends data to the client using tcp_server_send_data() and receives data from the client using tcp_server_recv().
 * Callback functions for sent data and connection result are defined in tcp_server_sent() and tcp_server_result() respectively.
 * 
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 */

#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "wifi.h"

#define TCP_PORT        4242
#define DEBUG_printf    printf
#define TEST_ITERATIONS 10
#define POLL_TIME_S     20

// Define the maximum length of a received message
#define MAX_MESSAGE_LENGTH 1024

/**
 * @brief Initializes a TCP server state.
 *
 * @return A pointer to the initialized TCP_SERVER_T struct, or NULL if
 * allocation failed.
 */
static TCP_SERVER_T *
tcp_server_init (void)
{
    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state)
    {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    return state;
}

/**
 * @brief Closes the TCP server connection and frees the memory allocated for
 * the connection state.
 *
 * @param arg Pointer to the TCP server connection state.
 *
 * @return err_t Returns ERR_OK if the connection was closed successfully,
 * ERR_ABRT if the connection was aborted, or an error code if the connection
 * could not be closed.
 */
static err_t
tcp_server_close (void *arg)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    err_t         err   = ERR_OK;
    if (state->client_pcb != NULL)
    {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        err = tcp_close(state->client_pcb);
        if (err != ERR_OK)
        {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->client_pcb);
            err = ERR_ABRT;
        }
        state->client_pcb = NULL;
    }
    if (state->server_pcb)
    {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    return err;
}

/**
 * @brief Callback function for TCP server connection result.
 *
 * @param arg pointer to the argument passed to tcp_connect.
 * @param status indicates the status of the connection result.
 * 0 indicates success, while other values indicate failure.
 *
 * @return ERR_OK if the function executes successfully.
 */
static err_t
tcp_server_result (void *arg, int status)
{
    if (status == 0)
    {
        DEBUG_printf("test success\n");
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
 * @param arg pointer to the TCP_SERVER_T struct.
 * @param tpcb pointer to the TCP control block.
 * @param len length of the sent data.
 *
 * @return ERR_OK if successful.
 */
static err_t
tcp_server_sent (void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    DEBUG_printf("tcp_server_sent %u\n", len);
    state->sent_len += len;

    if (state->sent_len >= BUF_SIZE)
    {

        // We should get the data back from the client
        state->recv_len = 0;
        DEBUG_printf("Waiting for buffer from client\n");
    }

    return ERR_OK;
}

/**
 * @brief This function is a callback from lwIP, so cyw43_arch_lwip_begin is not
 * required. However, you can use this method to cause an assertion in debug
 * mode if this method is called when cyw43_arch_lwip_begin IS needed.
 *
 * @param arg A void pointer to the TCP_SERVER_T struct.
 * @param tpcb A pointer to the tcp_pcb struct.
 * 
 * @return err_t value indicating success or failure.
 */
err_t
tcp_server_send_data (void *arg, struct tcp_pcb *tpcb)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    for (int i = 0; i < BUF_SIZE; i++)
    {
        state->buffer_sent[i] = rand();
    }

    state->sent_len = 0;
    DEBUG_printf("Writing %ld bytes to client\n", BUF_SIZE);
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not
    // required, however you can use this method to cause an assertion in debug
    // mode, if this method is called when cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    err_t err
        = tcp_write(tpcb, state->buffer_sent, BUF_SIZE, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        DEBUG_printf("Failed to write data %d\n", err);
        return tcp_server_result(arg, -1);
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
 * @param arg Pointer to the TCP server state structure.
 * @param tpcb Pointer to the TCP control block.
 * @param p Pointer to the received data buffer.
 * @param err Error code.
 *
 * @return err_t Error code.
 */
err_t
tcp_server_recv (void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    if (!p)
    {
        return tcp_server_result(arg, -1);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not
    // required, however you can use this method to cause an assertion in debug
    // mode, if this method is called when cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    if (p->tot_len > 0)
    {
        // DEBUG_printf("tcp_server_recv %d/%d err %d\n", p->tot_len,
        // state->recv_len, err);

        // Receive the buffer
        const uint16_t buffer_left = BUF_SIZE - state->recv_len;
        state->recv_len += pbuf_copy_partial(
            p,
            state->buffer_recv + state->recv_len,
            p->tot_len > buffer_left ? buffer_left : p->tot_len,
            0);
        tcp_recved(tpcb, p->tot_len);

        // Check if a complete message has been received (e.g., terminated by a
        // newline character)
        if (state->recv_len > 0
            && state->buffer_recv[state->recv_len - 1] == '\n')
        {
            state->buffer_recv[state->recv_len]
                = '\0'; // Null-terminate the received data
            printf("Received message: %s\n", state->buffer_recv);

            // Process the received message or respond to it as needed
            // Here, we're just printing it to the console

            // Reset the receive buffer
            state->recv_len = 0;
        }
    }
    pbuf_free(p);
    return tcp_server_send_data(arg, state->client_pcb);
}

/**
 * @brief Polls the TCP server.
 *
 * @param arg The argument to pass to the callback function.
 * @param tpcb The TCP control block.
 *
 * @return err_t The result of the TCP server.
 */
static err_t
tcp_server_poll (void *arg, struct tcp_pcb *tpcb)
{
    return tcp_server_result(arg, -1); // no response is an error?
}

/**
 * @brief This function handles the errors that occurs in the TCP server. If the
 * error is not an abort error, it calls the tcp_server_result() function with
 * the given arguments.
 *
 * @param arg Pointer to the argument passed to the TCP server.
 * @param err Error code returned by the TCP server.
 */
static void
tcp_server_err (void *arg, err_t err)
{
    if (err != ERR_ABRT)
    {
        tcp_server_result(arg, err);
    }
}

/**
 * @brief Function called when a TCP client connects to the server.
 *
 * @param arg Pointer to the TCP server state structure.
 * @param client_pcb Pointer to the client's PCB (Protocol Control Block).
 * @param err Error code returned by the TCP stack.
 *
 * @return err_t Error code returned by the TCP stack.
 */
static err_t
tcp_server_accept (void *arg, struct tcp_pcb *client_pcb, err_t err)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    if (err != ERR_OK || client_pcb == NULL)
    {
        DEBUG_printf("Failure in accept\n");
        tcp_server_result(arg, err);
        return ERR_VAL;
    }
    DEBUG_printf("Client connected\n");

    state->client_pcb = client_pcb;
    tcp_arg(client_pcb, state);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcp_server_err);

    return tcp_server_send_data(arg, state->client_pcb);
}

/**
 * @brief Opens a TCP server and listens for incoming connections on a specified
 * port.
 *
 * @param arg A pointer to a TCP_SERVER_T struct that contains the server state.
 *
 * @return true if the server was successfully started, false otherwise.
 */
static bool
tcp_server_open (void *arg)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    DEBUG_printf("Starting server at %s on port %u\n",
                 ip4addr_ntoa(netif_ip4_addr(netif_list)),
                 TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb)
    {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err)
    {
        DEBUG_printf("failed to bind to port %u\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb)
    {
        DEBUG_printf("failed to listen\n");
        if (pcb)
        {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    return true;
}

/**
 * @brief Runs a TCP server and opens it. Then, it enters a loop to check for
 * Wi-Fi driver or lwIP work that needs to be done. If using
 * pico_cyw43_arch_poll, it polls periodically from the main loop to check for
 * work. Otherwise, it sleeps for 1000ms.
 *
 * @param None
 * 
 * @return void
 *
 * @note This function assumes that the necessary libraries and header files
 * have been included. This function assumes that the necessary macros have been
 * defined. This function assumes that the necessary structs and functions have
 * been defined. This function assumes that the necessary memory has been
 * allocated. This function assumes that the necessary hardware has been set up.
 * This function assumes that the necessary network configurations have been
 * made. This function assumes that the necessary permissions have been granted.
 * This function assumes that the necessary ports have been opened.
 * This function assumes that the necessary security measures have been taken.
 * This function assumes that the necessary error handling has been implemented.
 * This function assumes that the necessary cleanup has been done.
 */

void
run_tcp_server_test (void)
{
    TCP_SERVER_T *state = tcp_server_init();
    if (!state)
    {
        return;
    }
    if (!tcp_server_open(state))
    {
        tcp_server_result(state, -1);
        return;
    }
    while (!state->complete)
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
    free(state);
}

/**
 * @brief Initializes the TCP server by initializing standard input/output,
 * initializing the CYW43 architecture, and enabling station mode.
 *
 * @param None
 * 
 * @return void
 */
void
tcp_server_begin_init ()
{
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
}

/**
 * @brief Begins a TCP server by connecting to Wi-Fi with the given SSID and
 * password.
 * 
 * @param None
 * 
 * @return void
 */
void
tcp_server_begin ()
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
    run_tcp_server_test();
    cyw43_arch_deinit();
}
