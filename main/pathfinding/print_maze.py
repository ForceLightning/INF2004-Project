"""Module for printing a maze in a human-readable format.
"""

import struct

_author = "Christopher Kok"


def pretty_print_maze(serialised_maze: bytes) -> str:
    """Prints a maze in a human-readable format.

    Args:
        serialised_maze (bytes): Compressed maze string gap bitmask.
        Each hex character represents a cell in the maze.
        rows (int): Number of rows in the maze.
        cols (int): Number of columns in the maze.

    Returns:
        str: A string representing the maze.
    """

    # Convert the string into a 2D array.
    maze = []
    rows, cols = struct.unpack(">2H", serialised_maze[:4])
    serialised_maze = serialised_maze[4:]
    num_bytes = (rows * cols) // 2 + (rows * cols) % 2
    serialised_maze = struct.unpack(f">{num_bytes}B", serialised_maze)

    for i in range(num_bytes):
        val = serialised_maze[i]
        cell_a = (val & 0xF0) >> 4
        cell_b = val & 0x0F
        cell_a = 0xF - cell_a
        cell_b = 0xF - cell_b
        maze.append(cell_a)
        if cell_b != 0xF:
            maze.append(cell_b)

    maze = [maze[i:i + cols] for i in range(0, len(maze), cols)]

    # Print the maze.
    ret_str = ""
    for i in range(rows):
        for relative_row in range(2):
            for j in range(cols):
                ret_str += draw_cell(maze[i][j], relative_row)
                if cols - 1 == j:
                    match(relative_row):
                        case 0:
                            ret_str += "+\n"
                        case 1:
                            ret_str += "|\n"
    for j in range(cols):
        ret_str += "+---"

    ret_str += "+"
    return ret_str


def draw_cell(cell_bitmask: int, relative_row: int) -> str:
    """Helper function to draw a single cell in a maze.

    Args:
        cell_bitmask (int): Bitmask representing the cell.
        relative_row (int): Relative row in the cell to draw. 0 is the top row.

    Returns:
        str: A string representing the relative row of the cell.
    """

    match(relative_row):
        case 0:
            if cell_bitmask & 0x1:
                return "+---"
            else:
                return "+   "
        case 1:
            if cell_bitmask & 0x8:
                return "|   "
            else:
                return "    "


def main():
    serialised_maze_bytes = b'\x00\x06\x00\x04\x6E\xC4\x51\x39\x7A\xA8\x56\xAC\x3D\x41\x2B\xB8'
    print(pretty_print_maze(serialised_maze_bytes))


if __name__ == "__main__":
    main()
