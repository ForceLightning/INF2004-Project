/**
 * @file barcode.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Implementation of the barcode driver.
 * @version 0.1
 * @date 2023-11-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "irline/ir_sensor.h"
#include "irline/barcode.h"

// Public function definitions.
// ---------------------------------------------------------------------------
//

/**
 * @brief Gets the character from the barcode character enum. @see
 * barcode_char_t.
 *
 * @param barcode_char Barcode character enum.
 * @return Character corresponding to the barcode character enum.
 */
char
barcode_get_char (barcode_char_t barcode_char)
{
    switch (barcode_char)
    {
        case BARCODE_CHAR_0:
            return '0';
        case BARCODE_CHAR_1:
            return '1';
        case BARCODE_CHAR_2:
            return '2';
        case BARCODE_CHAR_3:
            return '3';
        case BARCODE_CHAR_4:
            return '4';
        case BARCODE_CHAR_5:
            return '5';
        case BARCODE_CHAR_6:
            return '6';
        case BARCODE_CHAR_7:
            return '7';
        case BARCODE_CHAR_8:
            return '8';
        case BARCODE_CHAR_9:
            return '9';
        case BARCODE_CHAR_A:
            return 'A';
        case BARCODE_CHAR_B:
            return 'B';
        case BARCODE_CHAR_C:
            return 'C';
        case BARCODE_CHAR_D:
            return 'D';
        case BARCODE_CHAR_E:
            return 'E';
        case BARCODE_CHAR_F:
            return 'F';
        case BARCODE_CHAR_G:
            return 'G';
        case BARCODE_CHAR_H:
            return 'H';
        case BARCODE_CHAR_I:
            return 'I';
        case BARCODE_CHAR_J:
            return 'J';
        case BARCODE_CHAR_K:
            return 'K';
        case BARCODE_CHAR_L:
            return 'L';
        case BARCODE_CHAR_M:
            return 'M';
        case BARCODE_CHAR_N:
            return 'N';
        case BARCODE_CHAR_O:
            return 'O';
        case BARCODE_CHAR_P:
            return 'P';
        case BARCODE_CHAR_Q:
            return 'Q';
        case BARCODE_CHAR_R:
            return 'R';
        case BARCODE_CHAR_S:
            return 'S';
        case BARCODE_CHAR_T:
            return 'T';
        case BARCODE_CHAR_U:
            return 'U';
        case BARCODE_CHAR_V:
            return 'V';
        case BARCODE_CHAR_W:
            return 'W';
        case BARCODE_CHAR_X:
            return 'X';
        case BARCODE_CHAR_Y:
            return 'Y';
        case BARCODE_CHAR_Z:
            return 'Z';
        case BARCODE_CHAR_DASH:
            return '-';
        case BARCODE_CHAR_PERIOD:
            return '.';
        case BARCODE_CHAR_SPACE:
            return ' ';
        case BARCODE_CHAR_DOLLAR:
            return '$';
        case BARCODE_CHAR_SLASH:
            return '/';
        case BARCODE_CHAR_PLUS:
            return '+';
        case BARCODE_CHAR_PERCENT:
            return '%';
        case BARCODE_CHAR_ASTERISK:
            return '*';
        default:
            return '~';
    }
}

/**
 * @brief Helper function to get the barcode line type as a string.
 *
 * @param line_type Barcode line type.
 * @return char* Pointer to the barcode line type string.
 */
char *
barcode_line_to_string (barcode_line_type_t line_type)
{
    switch (line_type)
    {
        case BARCODE_LINE_BLACK_THIN:
            return "BLACK_THIN";
        case BARCODE_LINE_BLACK_THICK:
            return "BLACK_THICK";
        case BARCODE_LINE_WHITE_THIN:
            return "WHITE_THIN";
        case BARCODE_LINE_WHITE_THICK:
            return "WHITE_THICK";
        default:
            return "";
    }
}

/**
 * @brief Updates the line buffer with the line type detected.
 *
 * @param p_line_buffer Pointer to the line type buffer.
 * @param line_type The line type detected.
 * @return -1 on error, 0 on success, 1 if the line type has not changed.
 *
 */
int8_t
barcode_update_line_buffer (barcode_line_buffer_t *p_line_buffer,
                            barcode_line_type_t    line_type)
{
    // If the line buffer doesn't exist, return an error.
    //
    if (NULL == p_line_buffer)
    {
        return BARCODE_READ_ERROR;
    }

    // If the line buffer index has reached the maximum number of lines return
    // an error.
    //
    if (BARCODE_MAX_LINES <= p_line_buffer->line_buffer_index)
    {
        return BARCODE_READ_ERROR;
    }

    // Make sure that the first line is always a black line.
    //
    if (0u == p_line_buffer->line_buffer_index)
    {
        if (BARCODE_LINE_WHITE_THIN == line_type
            || BARCODE_LINE_WHITE_THICK == line_type)
        {
            return BARCODE_READ_NO_OP;
        }
        p_line_buffer->line_buffer[0u] = line_type;
        p_line_buffer->line_buffer_index++;
        return BARCODE_READ_CONTINUE;
    }

    // If the line is the same colour but thicker than the previous line,
    // override the previous line. We can do this because the thick lines are
    // always bigger than the thin lines by the next biggest power of two. So
    // checking if the line type is the same colour is as simple as doing a
    // bitwise AND, and checking if it is bigger is just a simple greater than
    // check.
    //
    barcode_line_type_t *p_previous_line
        = &p_line_buffer->line_buffer[p_line_buffer->line_buffer_index - 1];

    bool is_same_colour     = line_type & *p_previous_line;
    bool is_new_line_bigger = line_type > *p_previous_line;

    if (is_same_colour && is_new_line_bigger)
    {
        *p_previous_line = line_type;
        return BARCODE_READ_CONTINUE;
    }

    // If the line type is the same colour as the last line type, or no line
    // type was detected, return 1 to indicate that the line type has not
    // changed.
    //
    if (is_same_colour || BARCODE_LINE_NONE == line_type)
    {
        return BARCODE_READ_NO_OP;
    }

    p_line_buffer->line_buffer[p_line_buffer->line_buffer_index] = line_type;
    p_line_buffer->line_buffer_index++;

    if (BARCODE_MAX_LINES > p_line_buffer->line_buffer_index)
    {
        return BARCODE_READ_CONTINUE;
    }

    return BARCODE_READ_SUCCESS;
}

/**
 * @brief Initialises or clears the barcode line buffer.
 *
 * @param p_line_buffer Pointer to the barcode line buffer.
 */
void
barcode_clear_line_buffer (barcode_line_buffer_t *p_line_buffer)
{
    if (NULL == p_line_buffer)
    {
        return;
    }

    memset(p_line_buffer, 0, sizeof(barcode_line_buffer_t));
    p_line_buffer->line_buffer_index = 0u;
}
/**
 * @brief Decodes the barcode character enum from the line buffer.
 *
 * @param p_line_buffer Pointer to the barcode line buffer.
 * @return barcode_char_t Enum representing the decoded character.
 */

barcode_char_t
barcode_decode_barcode_char (barcode_line_buffer_t *p_line_buffer)
{
    uint16_t barcode_binarised = 0u;

    for (uint16_t idx = 0u; p_line_buffer->line_buffer_index > idx; idx++)
    {
        // Shift the barcode to the left by 1 bit to prepare for insertion of
        // LSB.
        //
        barcode_binarised <<= 1u;
        barcode_line_type_t line_type = p_line_buffer->line_buffer[idx];

        if (BARCODE_LINE_BLACK_THICK == line_type
            || BARCODE_LINE_WHITE_THICK == line_type)
        {
            barcode_binarised |= 1u;
        }
    }

    return barcode_binarised;
}

/**
 * @brief Returns a binary string representation of the barcode line buffer.
 *
 * @param p_line_buffer Pointer to the barcode line buffer.
 * @return char* Pointer to the binary string.
 *
 * @warning The returned string needs to be freed.
 */
char *
barcode_buffer_to_binary_string (barcode_line_buffer_t *p_line_buffer)
{
    // If the line buffer is empty, return NULL.
    //
    if (0u == p_line_buffer->line_buffer_index)
    {
        return NULL;
    }

    // Allocate memory for the string.
    //
    uint16_t num_chars = p_line_buffer->line_buffer_index + 2u;
    char    *p_string  = NULL;
    p_string           = (char *)malloc(num_chars * sizeof(char));

    // If the string failed to allocate, return NULL.
    //
    if (NULL == p_string)
    {
        DEBUG_PRINT(
            "DEBUG: Failed to allocate memory for barcode buffer string.\n");
        return NULL;
    }

    memset(p_string, 0, num_chars * sizeof(char));

    // Insert into the string in reverse. (MSB first).
    //
    for (uint16_t idx = 0u; p_line_buffer->line_buffer_index > idx; idx++)
    {
        barcode_line_type_t line_type = p_line_buffer->line_buffer[idx];
        if (BARCODE_LINE_BLACK_THICK == line_type
            || BARCODE_LINE_WHITE_THICK == line_type)
        {
            strcat(p_string, "1");
        }
        else if (BARCODE_LINE_BLACK_THIN == line_type
                 || BARCODE_LINE_WHITE_THIN == line_type)
        {
            strcat(p_string, "0");
        }
    }

    return p_string;
}

// End of file driver/barcode/barcode.c.
