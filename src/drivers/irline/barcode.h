/**
 * @file barcode.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Header file for the barcode driver.
 * @version 0.1
 * @date 2023-11-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdint.h>

#ifndef BARCODE_H // Include guard.
#define BARCODE_H

// Definitions.
// ----------------------------------------------------------------------------
//

/**
 * @defgroup barcode_constants Barcode Constants
 * @brief Constants for the barcode driver.
 * @{
 */

/** @brief Maximum number of barcode lines. */
#define BARCODE_MAX_LINES 9
/**
 @}*/ // End of barcode_definitions group.

#ifndef BARCODE_DEBUG_VERBOSE // Verbosity level for debug print.
/**
 * @def BARCODE_DEBUG_VERBOSE
 * @brief Verbosity level for debug print.
 */
#define BARCODE_DEBUG_VERBOSE 0
#endif

#ifndef NDEBUG
/**
 * @def PICO_DEBUG_MALLOC
 * @brief Enables malloc debugging.
 */
#define PICO_DEBUG_MALLOC 1

/**
 * @def BARCODE_DEBUG_VERBOSE
 * @brief Verbosity level for debug print.
 */
#define BARCODE_DEBUG_VERBOSE 0
#ifndef DEBUG_PRINT
/**
 * @def DEBUG_PRINT(...)
 * @brief Enables debug print.
 * @param ... Variable arguments.
 */
#define DEBUG_PRINT(...) printf(__VA_ARGS__) // Enable debug print.
#endif
#else
#define DEBUG_PRINT(...) // Disable debug print.
#endif

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @typedef barcode_char_t
 * @brief Enum for the barcode characters. A 0 bit represents a thin line (white
 * or black), while a 1 bit represents a thick line.
 *
 */
typedef enum
{
    BARCODE_CHAR_0        = 0b000110100,
    BARCODE_CHAR_1        = 0b100100001,
    BARCODE_CHAR_2        = 0b001100001,
    BARCODE_CHAR_3        = 0b101100000,
    BARCODE_CHAR_4        = 0b000110001,
    BARCODE_CHAR_5        = 0b100110000,
    BARCODE_CHAR_6        = 0b001110000,
    BARCODE_CHAR_7        = 0b000100101,
    BARCODE_CHAR_8        = 0b100100100,
    BARCODE_CHAR_9        = 0b001100100,
    BARCODE_CHAR_A        = 0b100001001,
    BARCODE_CHAR_B        = 0b001001001,
    BARCODE_CHAR_C        = 0b101001000,
    BARCODE_CHAR_D        = 0b000011001,
    BARCODE_CHAR_E        = 0b100011000,
    BARCODE_CHAR_F        = 0b001011000,
    BARCODE_CHAR_G        = 0b000001101,
    BARCODE_CHAR_H        = 0b100001100,
    BARCODE_CHAR_I        = 0b001001100,
    BARCODE_CHAR_J        = 0b000011100,
    BARCODE_CHAR_K        = 0b100000011,
    BARCODE_CHAR_L        = 0b001000011,
    BARCODE_CHAR_M        = 0b101000010,
    BARCODE_CHAR_N        = 0b000010011,
    BARCODE_CHAR_O        = 0b100010010,
    BARCODE_CHAR_P        = 0b001010010,
    BARCODE_CHAR_Q        = 0b000000111,
    BARCODE_CHAR_R        = 0b100000110,
    BARCODE_CHAR_S        = 0b001000110,
    BARCODE_CHAR_T        = 0b000010110,
    BARCODE_CHAR_U        = 0b110000001,
    BARCODE_CHAR_V        = 0b011000001,
    BARCODE_CHAR_W        = 0b111000000,
    BARCODE_CHAR_X        = 0b010010001,
    BARCODE_CHAR_Y        = 0b110010000,
    BARCODE_CHAR_Z        = 0b011010000,
    BARCODE_CHAR_DASH     = 0b010000101,
    BARCODE_CHAR_PERIOD   = 0b110000100,
    BARCODE_CHAR_SPACE    = 0b011000100,
    BARCODE_CHAR_DOLLAR   = 0b010101000,
    BARCODE_CHAR_SLASH    = 0b010100010,
    BARCODE_CHAR_PLUS     = 0b010001010,
    BARCODE_CHAR_PERCENT  = 0b000101010,
    BARCODE_CHAR_ASTERISK = 0b010010100 ///< Used as a delimiter.
} barcode_char_t;

/**
 * @typedef barcode_line_type_t
 * @brief Enum for the barcode line types (black or white, thin or thick). None
 * for error handling.
 *
 */
typedef enum
{
    BARCODE_LINE_NONE        = 0,      ///< No barcode line detected.
    BARCODE_LINE_BLACK_THIN  = 0b0001, ///< Black thin barcode line.
    BARCODE_LINE_BLACK_THICK = 0b0011, ///< Black thick barcode line.
    BARCODE_LINE_WHITE_THIN  = 0b0100, ///< White thin barcode line.
    BARCODE_LINE_WHITE_THICK = 0b1100  ///< White thick barcode line.
} barcode_line_type_t;

/**
 * @brief Struct that holds the 9 most recent barcode lines.
 *
 */
typedef struct barcode_line_buffer
{
    barcode_line_type_t line_buffer[9]; ///< Holds the 9 most recent barcode
                                        ///< lines.
    uint16_t line_buffer_index;         ///< Index of the line buffer.

} barcode_line_buffer_t;

/**
 * @brief Enum for the barcode read response. Success or error.
 *
 */
typedef enum
{
    BARCODE_READ_ERROR    = -1, ///< Error reading barcode.
    BARCODE_READ_SUCCESS  = 0,  ///< Successfully read barcode.
    BARCODE_READ_CONTINUE = 1,  ///< Successfully read barcode,
                                ///< but buffer is still not full.
    BARCODE_READ_NO_OP = 2      ///< No operation performed.
} barcode_read_response_t;

// Public function prototypes.
// ----------------------------------------------------------------------------
//

char           barcode_get_char(barcode_char_t barcode_char);
char          *barcode_line_to_string(barcode_line_type_t line_type);
int8_t         barcode_update_line_buffer(barcode_line_buffer_t *p_line_buffer,
                                          barcode_line_type_t    line_type);
void           barcode_clear_line_buffer(barcode_line_buffer_t *p_line_buffer);
barcode_char_t barcode_decode_barcode_char(
    barcode_line_buffer_t *p_line_buffer);
char *barcode_buffer_to_binary_string(barcode_line_buffer_t *p_line_buffer);

#endif // BARCODE_H

// End of file driver/irline/barcode.h.