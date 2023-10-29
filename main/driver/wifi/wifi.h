/**
 * @file wifi.h
 * 
 * @brief Header file for WiFi module.
 * This file contains the function prototypes and data structures for the WiFi
 * module. The module provides a TCP server that can be initialized and started
 * using the functions tcp_server_begin_init() and tcp_server_begin(). The TCP
 * server uses lwIP library for network communication and has a buffer size of
 * 2048 bytes.
 */
#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define BUF_SIZE 2048

/**
 * @brief Struct representing a TCP server connection.
 *
 * @param server_pcb Pointer to the server PCB.
 * @param client_pcb Pointer to the client PCB.
 * @param complete Flag indicating whether the connection is complete.
 * @param buffer_sent Buffer for sent data.
 * @param buffer_recv Buffer for received data.
 * @param sent_len Length of sent data.
 * @param recv_len Length of received data.
 * @param run_count Counter for tracking the state of the connection.
 *
 * @note struct contains information about a TCP server connection, including
 * the server and client PCBs,
 * @note flags indicating whether the connection is complete, buffers for sent
 * and received data, and various
 * @note counters for tracking the state of the connection.
 */
typedef struct TCP_SERVER_T_
{
    struct tcp_pcb *server_pcb; /**< Pointer to the server PCB */
    struct tcp_pcb *client_pcb; /**< Pointer to the client PCB */
    bool    complete; /**< Flag indicating whether the connection is complete */
    uint8_t buffer_sent[BUF_SIZE]; /**< Buffer for sent data */
    uint8_t buffer_recv[BUF_SIZE]; /**< Buffer for received data */
    int     sent_len;              /**< Length of sent data */
    int     recv_len;              /**< Length of received data */
    int     run_count; /**< Counter for tracking the state of the connection */
} TCP_SERVER_T;

// Function prototypes
void tcp_server_begin_init();
void tcp_server_begin();

#endif // WIFI_H
