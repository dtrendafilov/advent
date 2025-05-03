use std::fs;
use std::collections::{BinaryHeap, HashSet};

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Ord, PartialOrd)]
struct Position {
    row: usize,
    col: usize,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Ord, PartialOrd)]
enum Direction {
    Up,
    Right,
    Down,
    Left,
}

impl Direction {
    fn move_position(&self, pos: &Position, grid_size: (usize, usize)) -> Option<Position> {
        let (rows, cols) = grid_size;
        match self {
            Direction::Up if pos.row > 0 => {
                Some(Position { row: pos.row - 1, col: pos.col })
            },
            Direction::Right if pos.col < cols - 1 => {
                Some(Position { row: pos.row, col: pos.col + 1 })
            },
            Direction::Down if pos.row < rows - 1 => {
                Some(Position { row: pos.row + 1, col: pos.col })
            },
            Direction::Left if pos.col > 0 => {
                Some(Position { row: pos.row, col: pos.col - 1 })
            },
            _ => None // Movement would go out of bounds
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Ord, PartialOrd)]
struct State {
    position: Position,
    direction: Direction,
}

#[derive(Debug)]
struct Maze {
    grid: Vec<Vec<char>>,
    start: Position,
    exit: Position,
}

fn read_maze(file_path: &str) -> Maze {
    let contents = fs::read_to_string(file_path)
        .expect("Should have been able to read the file");
    
    let mut grid = Vec::new();
    let mut start = Position { row: 0, col: 0 };
    let mut exit = Position { row: 0, col: 0 };
    
    for (row, line) in contents.lines().enumerate() {
        for (col, ch) in line.chars().enumerate() {            
            if ch == 'S' {
                start = Position { row, col };
            } else if ch == 'E' {
                exit = Position { row, col };
            }
        }
        grid.push(line.chars().collect());
    }
    
    Maze { grid, start, exit }
}

#[derive(Debug, PartialEq, Eq, Clone)]
struct MazeState {
    steps: usize,
    state: State,
    path: Vec<Position>,
}

impl Ord for MazeState {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        // Reverse the comparison for steps to create a min-heap
        // This makes the BinaryHeap prioritize states with fewer steps
        other.steps.cmp(&self.steps)
    }
}

impl PartialOrd for MazeState {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

/// Finds the shortest path from start to exit in the maze.
/// Returns the number of steps in the path if found, or None if no path exists.
fn walk_maze(maze: &Maze) -> (Option<usize>, usize) {
    let grid_size = (maze.grid.len(), maze.grid[0].len());
    let mut queue = BinaryHeap::new();
    let mut visited = HashSet::<State>::new();
    let mut spectate_from = HashSet::<Position>::new();

    let mut min_distance = Option::<usize>::None;
    spectate_from.insert(maze.start);
    spectate_from.insert(maze.exit);
    
    queue.push(MazeState { steps: 0, state: State{position: maze.start, direction: Direction::Left}, path: Vec::<Position>::new() });

    while let Some(maze_step) = queue.pop() {
        let state = maze_step.state;
        
        visited.insert(state);

        if min_distance.is_some() && maze_step.steps > min_distance.unwrap() {
            continue;
        }
        
        // Check if we've reached the exit
        if maze_step.state.position == maze.exit {
            if min_distance.is_none() {
                min_distance = Some(maze_step.steps);
            }
            if min_distance.unwrap() == maze_step.steps {
                for pos in maze_step.path.iter() {
                    spectate_from.insert(*pos);
                }
            }
            continue;
        }
        
        if let Some(new_pos) = state.direction.move_position(&state.position, grid_size) {
            let new_state = State { position: new_pos, direction: state.direction };
            if maze.grid[new_pos.row][new_pos.col] != '#' && !visited.contains(&new_state) {
                let mut path = maze_step.path.clone();
                path.push(state.position);
                queue.push(MazeState { steps: maze_step.steps + 1, state: new_state, path: path });
            }
        }
        
        // Try turning left and right
        let left_dir = match state.direction {
            Direction::Up => Direction::Left,
            Direction::Right => Direction::Up,
            Direction::Down => Direction::Right,
            Direction::Left => Direction::Down,
        };
        
        let right_dir = match state.direction {
            Direction::Up => Direction::Right,
            Direction::Right => Direction::Down,
            Direction::Down => Direction::Left,
            Direction::Left => Direction::Up,
        };

        {
            let left_turn = State { position: state.position, direction: left_dir };
            if !visited.contains(&left_turn) {
                queue.push(MazeState { steps: maze_step.steps + 1000, state: left_turn, path: maze_step.path.clone()  });
            }
        }

        {
            let right_turn = State { position: state.position, direction: right_dir };
            if !visited.contains(&right_turn) {
                queue.push(MazeState { steps: maze_step.steps + 1000, state: right_turn, path: maze_step.path.clone()  });
            }
        }
    }
    // dbg!(&spectate_from);
    
    return (min_distance, spectate_from.len());
}

fn solve_maze(file_path: &str) -> (Option<usize>, usize) {
    let maze = read_maze(file_path);
    walk_maze(&maze)
}

fn main() {
    let (result, paths) = solve_maze("input.txt");
    match result {
        Some(score) => println!("Shortest path found: {} score and {} paths", score, paths),
        _ => println!("No path found from start to exit"),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_solve_maze_sample() {
        let (result, paths) = solve_maze("sample.txt");
        assert_eq!(paths, 45);
        assert!(result.is_some(), "Expected to find a path in the sample maze");
        assert_eq!(result.unwrap(), 7036);
    }

    #[test]
    fn test_solve_maze_sample2() {
        let (result, paths) = solve_maze("sample2.txt");
        assert_eq!(paths, 64);
        assert!(result.is_some(), "Expected to find a path in the sample maze");
        assert_eq!(result.unwrap(), 11048);
    }

    #[test]
    fn test_solve_maze_input() {
        let (result, paths) = solve_maze("input.txt");
        assert_eq!(paths, 248);
        assert!(result.is_some(), "Expected to find a path in the sample maze");
        assert_eq!(result.unwrap(), 130536);
    }
}
