"""Module for printing a maze in a human-readable format.
"""

import struct

_author = "Christopher Kok"


def pretty_print_maze(serialised_maze: bytes) -> str:
    """Prints a maze in a human-readable format.

    Args:
        serialised_maze (bytes): Compressed maze string gap bitmask.
        Each hex character represents a cell in the maze.

    Returns:
        str: A string representing the maze.
    """

    # Convert the string into a 2D array.
    maze = []
    rows, cols = struct.unpack(">2H", serialised_maze[:4])
    serialised_maze = serialised_maze[4:]
    num_bytes = (rows * cols) // 2 + (rows * cols) % 2
    print(rows, cols)
    maze_bytes = struct.unpack(
        f">{num_bytes}B", serialised_maze[:num_bytes])

    for i in range(num_bytes):
        val = maze_bytes[i]
        cell_a = (val & 0xF0) >> 4
        cell_b = val & 0x0F
        cell_a = 0xF - cell_a
        cell_b = 0xF - cell_b
        maze.append(cell_a)
        if cell_b != 0xF:
            maze.append(cell_b)

    maze = [maze[i:i + cols] for i in range(0, len(maze), cols)]

    ret_str = draw_grid(maze)

    # Draw the path if it exists.
    # Step 1: Look for the delimiter.

    serialised_maze = serialised_maze[num_bytes:]
    if len(serialised_maze) > 0:
        path_header = struct.unpack(">H", serialised_maze[:2])
        if path_header[0] != 0xFFFF:
            raise ValueError("Invalid path header")
        else:
            _, path_length = struct.unpack(">2H", serialised_maze[:4])
            num_bytes = path_length * 2
            print(path_length)
            print(serialised_maze)
            serialised_maze = serialised_maze[4:]
            path = struct.unpack(f">{num_bytes}B", serialised_maze[:num_bytes])
            print(serialised_maze)
            print(serialised_maze[:num_bytes])
            path = [path[i:i + 2] for i in range(0, len(path), 2)]
            ret_str = draw_path(ret_str, rows, cols, path)

    # Draw the navigator if it exists.
    serialised_maze = serialised_maze[num_bytes:]
    if len(serialised_maze) > 0:
        navigator_header = struct.unpack(">H", serialised_maze[:2])
        if navigator_header[0] != 0xFFFF:
            print(ret_str)
            print(serialised_maze)
            raise ValueError("Invalid navigator header")
        else:
            serialised_maze = serialised_maze[2:]
            current, orientation, _, _ = struct.unpack(
                ">2HB2H2H", serialised_maze[:13])
            ret_str = draw_navigator(ret_str, rows, cols, current, orientation)

    return ret_str


def draw_grid(maze: list[list[int]]) -> str:
    """Draws a grid of the maze.

    Args:
        maze (list[list[int]]): A 2D array representing the maze.

    Returns:
        str: A string representing the maze.
    """

    ret_str = ""
    for i in range(len(maze)):
        for relative_row in range(2):
            for j in range(len(maze[i])):
                ret_str += draw_cell(maze[i][j], relative_row)
                if len(maze[i]) - 1 == j:
                    match(relative_row):
                        case 0:
                            ret_str += "+\n"
                        case 1:
                            ret_str += "|\n"
    for j in range(len(maze[0])):
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


def get_direction_from_coords(point_a: tuple, point_b: tuple) -> int:
    """Gets the direction from point A to point B.

    Args:
        point_a (tuple): Point A. Coordinates (X, Y)
        point_b (tuple): Point B. Coordinates (X, Y)

    Returns:
        int: The direction from point A to point B.
    """

    if max(abs(point_a[0] - point_b[0]), abs(point_a[1] - point_b[1])) != 1:
        return None

    if point_a[0] == point_b[0]:  # Check x coordinates
        if point_a[1] < point_b[1]:
            return 1  # east
        else:
            return 3  # west
    else:
        if point_a[0] < point_b[0]:  # Check y coordinates
            return 2  # south
        else:
            return 0  # north


def draw_path(
    ret_str: str,
    rows: int,
    cols: int,
    path: list[list[int]]
) -> str:
    str_num_cols = cols * 4 + 2
    for i in range(len(path)):
        if i == 0:
            print(path[i])
            ret_str = insert_centre_path_char(
                ret_str, str_num_cols, path[i], "%")
        else:
            in_direction = get_direction_from_coords(path[i], path[i - 1])
            if in_direction:
                insert_path_in_direction(
                    ret_str, str_num_cols, path[i], in_direction)
                if i == len(path) - 1:
                    ret_str = insert_centre_path_char(
                        ret_str, str_num_cols, path[i], "X")
                    return ret_str
                out_direction = get_direction_from_coords(path[i], path[i + 1])
                if out_direction:
                    if abs(in_direction - out_direction) == 2:
                        if max(in_direction, out_direction) == 2:
                            ret_str = insert_centre_path_char(
                                ret_str, str_num_cols, path[i], "|")
                        else:
                            ret_str = insert_centre_path_char(
                                ret_str, str_num_cols, path[i], "-")
                    else:
                        ret_str = insert_centre_path_char(
                            ret_str, str_num_cols, path[i], "O")

    return ret_str


def insert_centre_path_char(
    ret_str: str,
    str_num_cols: int,
    coords: tuple,
    char: str
) -> str:
    """Inserts a character into the centre of a cell in the maze.

    Args:
        ret_str (str): The current maze string.
        str_num_cols (int): The number of columns in the maze string.
        coords (tuple): The coordinates of the cell to insert the character into.
        char (str): The character to insert into the cell.

    Returns:
        str: The updated maze string.
    """

    str_row = coords[1] * 2 + 1
    str_col = coords[0] * 4 + 2
    ret_str = list(ret_str)
    ret_str[str_row * str_num_cols + str_col] = char
    ret_str = "".join(ret_str)
    return ret_str


def insert_path_in_direction(
    ret_str: str,
    str_num_cols: int,
    coords: tuple,
    direction: int
) -> str:

    str_row = coords[1] * 2 + 1
    str_col = coords[0] * 4 + 2
    match (direction):
        case 0:  # north
            ret_str = list(ret_str)
            ret_str[(str_row - 1) * str_num_cols + str_col] = "|"
            ret_str = "".join(ret_str)
            return ret_str
        case 1:  # east
            ret_str = list(ret_str)
            for i in range(3):
                ret_str[str_row * str_num_cols + str_col + i + 1] = "-"
            ret_str = "".join(ret_str)
            return ret_str
        case 2:  # south
            ret_str = list(ret_str)
            ret_str[(str_row + 1) * str_num_cols + str_col] = "|"
            ret_str = "".join(ret_str)
            return ret_str
        case 3:  # west
            ret_str = list(ret_str)
            for i in range(3):
                ret_str[str_row * str_num_cols + str_col - i - 1] = "-"
            ret_str = "".join(ret_str)
            return ret_str

    return ret_str


def draw_navigator(
    ret_str: str,
    rows: int,
    cols: int,
    current: tuple,
    orientation: int,
) -> str:
    str_num_cols = cols * 4 + 2
    navigator_char = ""
    match(orientation):
        case 0:
            navigator_char = "^"
        case 1:
            navigator_char = ">"
        case 2:
            navigator_char = "v"
        case 3:
            navigator_char = "<"
    ret_str = insert_centre_path_char(
        ret_str, str_num_cols, current, navigator_char)
    return ret_str


def main():
    serialised_maze_bytes = b'\x00\x06\x00\x04\x6E\xC4\x51\x39\x7A\xA8\x56\xAC\x3D\x41\x2B\xB8\xFF\xFF\x00\x09\x00\x05\x01\x05\x01\x04\x00\x04\x00\x03\x00\x02\x00\x01\x00\x00\x01\x00\xFF\xFF\x00\x05\x30\x20\x50\x10'
    # TODO: fix the uint32_t insertions (maze header, path header)
    print(pretty_print_maze(serialised_maze_bytes))


if __name__ == "__main__":
    main()
