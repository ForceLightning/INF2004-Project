def pretty_print_maze(serialised_maze: str, rows: int, cols: int):
    """Prints a maze in a human-readable format.

    Args:
        serialised_maze: A string of single hex chars representing a maze.
        rows: The number of rows in the maze.
        cols: The number of columns in the maze.
    """
    
    # Convert the string into a 2D array.
    maze = []
    for i in range(rows):
        row = []
        for j in range(cols):
            val = int(serialised_maze[i * cols + j], 16)
            val = 0xF - val
            row.append(val)
        maze.append(row)

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
    serialised_maze_str = "6EC451397AA856AC3D412BB8"
    rows = 6
    cols = 4
    print(pretty_print_maze(serialised_maze_str, rows, cols))

if __name__ == "__main__":
    main()
