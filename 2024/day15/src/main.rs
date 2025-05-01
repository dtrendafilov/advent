use std::fs;
use std::collections::HashSet;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Direction {
    Up,
    Down,
    Left,
    Right,
}

impl Direction {
    fn from_char(c: char) -> Option<Direction> {
        match c {
            '^' => Some(Direction::Up),
            'v' => Some(Direction::Down),
            '<' => Some(Direction::Left),
            '>' => Some(Direction::Right),
            _ => None,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
struct Position {
    row: usize,
    col: usize,
}

impl Position {
    fn new(row: usize, col: usize) -> Self {
        Position { row, col }
    }

    fn move_in_direction(&self, direction: Direction, maze: &mut Vec<Vec<char>>) -> Position {
        // Calculate the new position based on direction
        let new_pos = match direction {
            Direction::Up => Position::new(self.row - 1, self.col),
            Direction::Down => Position::new(self.row + 1, self.col),
            Direction::Left => Position::new(self.row, self.col - 1),
            Direction::Right => Position::new(self.row, self.col + 1),
        };
        
        // If the new position is an empty space, just move there
        if maze[new_pos.row][new_pos.col] == '.' {
            maze[self.row][self.col] = '.'; // Clear the old position
            maze[new_pos.row][new_pos.col] = '@'; // Mark the new position
            return new_pos;
        }

        if maze[new_pos.row][new_pos.col] == '#' {
            return *self; // Can't move into a wall
        }
        
        // Define offsets for movement in each direction
        let (row_delta, col_delta) = match direction {
            Direction::Up => (-1, 0),
            Direction::Down => (1, 0),
            Direction::Left => (0, -1),
            Direction::Right => (0, 1),
        };

        let mut box_rows: Vec<HashSet<Position>> = vec![];
        box_rows.push(HashSet::new());
        match maze[new_pos.row][new_pos.col] {
            'O' => {
                box_rows[0].insert(new_pos);
            }
            '[' => {
                box_rows[0].insert(new_pos);
                if row_delta != 0 {
                    let new_pos2 = Position::new(new_pos.row, new_pos.col + 1);
                    box_rows[0].insert(new_pos2);
                }
            }
            ']' => {
                box_rows[0].insert(new_pos);
                if row_delta != 0 {
                    let new_pos2 = Position::new(new_pos.row, new_pos.col - 1);
                    box_rows[0].insert(new_pos2);
                }
            }
            _ => {}
        }

        let mut can_push = true;
        while can_push {
            let mut new_boxes: HashSet<Position> = HashSet::new();
            for box_pos in box_rows[box_rows.len() - 1].iter() {
                let next_pos = Position::new(((box_pos.row as isize) + row_delta) as usize, ((box_pos.col as isize) + col_delta) as usize);
                
                match maze[next_pos.row][next_pos.col] {
                    '#' => {
                        can_push = false;
                        break;
                    }
                    'O' => {
                        new_boxes.insert(next_pos);
                    }
                    '[' => {
                        new_boxes.insert(next_pos);
                        if row_delta != 0 {
                            let new_pos2 = Position::new(next_pos.row, next_pos.col + 1);
                            new_boxes.insert(new_pos2);
                        }
                    }
                    ']' => {
                        new_boxes.insert(next_pos);
                        if row_delta != 0 {
                            let new_pos2 = Position::new(next_pos.row, next_pos.col - 1);
                            new_boxes.insert(new_pos2);
                        }
                    }
                    _ => {}
                }
            }
            if !new_boxes.is_empty() {
                box_rows.push(new_boxes);
            } else {
                break;
            }
        }
        if can_push {
            for box_pos in box_rows.iter().rev() {
                for pos in box_pos.iter() {
                    let box_row = ((pos.row as isize) + row_delta) as usize;
                    let box_col = ((pos.col as isize) + col_delta) as usize;
                    maze[box_row][box_col] = maze[pos.row][pos.col]; // Move the box
                    maze[pos.row][pos.col] = '.'; // Clear the old position
                }
            }
            maze[new_pos.row][new_pos.col] = '@'; // Mark the new position
            maze[self.row][self.col] = '.'; // Clear the old position
            return new_pos;
        } else {
            return *self; // Can't push the boxes
        }
    }
}

struct MazeData {
    maze: Vec<Vec<char>>,
    instructions: Vec<Direction>,
    start_position: Position,
}

fn read_maze_data(file_path: &str) -> MazeData {
    // Read the file content
    let content = fs::read_to_string(file_path)
        .expect("Failed to read the file");
    
    // Split content into lines
    let lines: Vec<&str> = content.lines().collect();
    
    // Find the blank line that separates the maze from instructions
    let mut blank_line_index = 0;
    for (i, &line) in lines.iter().enumerate() {
        if line.trim().is_empty() {
            blank_line_index = i;
            break;
        }
    }
    
    // Parse the maze (lines before the blank line)
    let maze_lines = &lines[0..blank_line_index];
    let maze: Vec<Vec<char>> = maze_lines
        .iter()
        .map(|line| line.chars().collect())
        .collect();
    
    // Parse instructions (lines after the blank line, joined)
    let instructions_str: String = lines[blank_line_index + 1..]
        .join("");
    
    // Find the robot's starting position
    let mut robot_pos = None;
    
    for (row, line) in maze.iter().enumerate() {
        for (col, &cell) in line.iter().enumerate() {
            if cell == '@' {
                robot_pos = Some(Position::new(row, col));
                break;
            }
        }
        if robot_pos.is_some() {
            break;
        }
    }
    
    let start_position = robot_pos.expect("Robot not found in the maze");
    
    // Parse movement instructions
    let instructions: Vec<Direction> = instructions_str
        .chars()
        .filter_map(Direction::from_char)
        .collect();
    
    MazeData {
        maze,
        instructions,
        start_position,
    }
}

fn double_maze_width(maze_data: &mut MazeData) {
    let rows = maze_data.maze.len();
    let original_cols = maze_data.maze[0].len();
    let new_cols = original_cols * 2;
    
    // Create a new maze with doubled width
    let mut new_maze: Vec<Vec<char>> = vec![vec!['.'; new_cols]; rows];
    
    // Fill the new maze
    for row in 0..rows {
        for col in 0..original_cols {
            let cell = maze_data.maze[row][col];
            let new_col = col * 2;
            
            match cell {
                '@' => {
                    // Robot position is doubled in width
                    new_maze[row][new_col] = '@';
                    new_maze[row][new_col + 1] = '.';
                    maze_data.start_position = Position::new(row, new_col);
                },
                'O' => {
                    // Convert box 'O' to '[]'
                    new_maze[row][new_col] = '[';
                    new_maze[row][new_col + 1] = ']';
                },
                '.' => {
                    // Empty space is doubled
                    new_maze[row][new_col] = '.';
                    new_maze[row][new_col + 1] = '.';
                },
                '#' => {
                    // Wall is doubled
                    new_maze[row][new_col] = '#';
                    new_maze[row][new_col + 1] = '#';
                },
                _ => {
                    // Any other character is copied as is
                    new_maze[row][new_col] = cell;
                    new_maze[row][new_col + 1] = cell;
                }
            }
        }
    }
    
    // Update the maze data
    maze_data.maze = new_maze;
}

fn compute_score(maze: &Vec<Vec<char>>) -> usize {
    let mut score = 0;
    
    // Loop through the maze and find all boxes ('O' or '[')
    for (row, line) in maze.iter().enumerate() {
        for (col, &cell) in line.iter().enumerate() {
            if cell == 'O' {
                // Calculate position value: row * 100 + col for single-width boxes
                score += row * 100 + col;
            } else if cell == '[' {
                // For double-width boxes, we use the position of the '[' to calculate the score
                // This ensures compatibility with the original scoring system
                score += row * 100 + col;
            }
        }
    }
    
    score
}

fn solve_maze(file_path: &str) -> usize {
    // Read maze and instructions from file
    let maze_data = read_maze_data(file_path);
    let mut maze = maze_data.maze;
    let instructions = maze_data.instructions;
    let mut robot_pos = maze_data.start_position;
    
    // Simulate robot movement
    for &direction in instructions.iter() {
        robot_pos = robot_pos.move_in_direction(direction, &mut maze);
    }
    
    // Compute and return the final score
    compute_score(&maze)
}

fn solve_maze_double_width(file_path: &str) -> usize {
    // Read maze and instructions from file
    let mut maze_data = read_maze_data(file_path);
    
    // Apply the transformation to double the maze width and replace boxes
    double_maze_width(&mut maze_data);    
    let mut maze = maze_data.maze;
    let instructions = maze_data.instructions;
    let mut robot_pos = maze_data.start_position;
    
    // Simulate robot movement
    for &direction in instructions.iter() {
        robot_pos = robot_pos.move_in_direction(direction, &mut maze);
    }
    
    // Compute and return the final score
    compute_score(&maze)
}

fn main() {
    // Read maze file and get the score with original representation
    let score = solve_maze("sample.txt");
    println!("Final score for sample (original): {}", score);
    
    // Solve the actual input file with original representation
    let input_score = solve_maze("input.txt");
    println!("Final score for input (original): {}", input_score);
    
    // Now solve with doubled width and new box representation
    let double_score = solve_maze_double_width("sample.txt");
    println!("Final score for sample (double width): {}", double_score);
    
    let double_input_score = solve_maze_double_width("input.txt");
    println!("Final score for input (double width): {}", double_input_score);
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_sample_maze() {
        let score = solve_maze("sample.txt");
        assert_eq!(score, 10092, "Sample test failed");
    }
    
    #[test]
    fn test_input_maze() {
        let input_score = solve_maze("input.txt");
        assert_eq!(input_score, 1526018, "Input test failed");
    }

    #[test]
    fn test_double_width_sample() {
        let double_score = solve_maze_double_width("sample.txt");
        assert_eq!(double_score, 9021, "Double width sample test failed");
    }

    #[test]
    fn test_double_width_input() {
        let double_input_score = solve_maze_double_width("input.txt");
        assert_eq!(double_input_score, 1550677, "Double width input test failed");
    }
}
