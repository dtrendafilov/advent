use std::fs;

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
        
        // If the new position contains a box, try to push it
        if maze[new_pos.row][new_pos.col] == 'O' {
            // Define offsets for movement in each direction
            let (row_delta, col_delta) = match direction {
                Direction::Up => (-1, 0),
                Direction::Down => (1, 0),
                Direction::Left => (0, -1),
                Direction::Right => (0, 1),
            };
            
            // Find boxes to push
            let mut check_row = ((new_pos.row as isize) + row_delta) as usize;
            let mut check_col = ((new_pos.col as isize) + col_delta) as usize;
            
            // Collect all consecutive boxes in the direction
            while maze[check_row][check_col] == 'O' {
                check_row = ((check_row as isize) + row_delta) as usize;
                check_col = ((check_col as isize) + col_delta) as usize;
            }
            
            // Check if there's an empty space after the last box
            if maze[check_row][check_col] == '.' {
                
                // Move all boxes one position in the push direction
                maze[check_row][check_col] = 'O';                
                
                // Clear the original box position that the robot will move into
                maze[new_pos.row][new_pos.col] = '@';
                maze[self.row][self.col] = '.'; // Clear the old position
                return new_pos;
            }
        }
        // Return the original position if movement is blocked
        *self
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

fn compute_score(maze: &Vec<Vec<char>>) -> usize {
    let mut score = 0;
    
    // Loop through the maze and find all boxes ('O')
    for (row, line) in maze.iter().enumerate() {
        for (col, &cell) in line.iter().enumerate() {
            if cell == 'O' {
                // Calculate position value: row * 100 + col
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

fn main() {
    // Read maze file and get the score
    let score = solve_maze("sample.txt");
    println!("Final score for sample: {}", score);
    
    // Solve the actual input file
    let input_score = solve_maze("input.txt");
    println!("Final score for input: {}", input_score);
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
}
