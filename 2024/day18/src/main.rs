use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::collections::{VecDeque, HashSet};

/// Represents a 2D maze with obstacles
#[derive(Debug)]
struct Maze {
    width: usize,
    height: usize,
    obstacles: Vec<(usize, usize)>,
}

impl Maze {
    /// Creates a new maze with the specified dimensions
    fn new(width: usize, height: usize) -> Self {
        Maze {
            width,
            height,
            obstacles: Vec::new(),
        }
    }
    
    /// Loads obstacles from a file where each line contains "x,y" coordinates
    fn load_obstacles_from_file<P: AsRef<Path>>(&mut self, file_path: P) -> io::Result<()> {
        let file = File::open(file_path)?;
        let reader = io::BufReader::new(file);
        
        for line in reader.lines() {
            let line = line?;
            let coords: Vec<&str> = line.trim().split(',').collect();
            
            if coords.len() == 2 {
                if let (Ok(x), Ok(y)) = (coords[0].parse::<usize>(), coords[1].parse::<usize>()) {
                    // Check if coordinates are within maze bounds
                    if x < self.width && y < self.height {
                        self.obstacles.push((x, y));
                    }
                }
            }
        }
        
        Ok(())
    }
    
    /// Find the shortest path from (0,0) to the bottom-right corner using BFS
    /// The `obstacle_count` parameter determines how many obstacles to consider
    /// Returns the length of the shortest path if found, None otherwise
    fn find_path_bfs(&self, obstacle_count: usize) -> Option<usize> {
        // Start at top-left corner (0,0)
        let start = (0, 0);
        // Target is bottom-right corner
        let target = (self.width - 1, self.height - 1);
        
        // Consider only the first `obstacle_count` obstacles
        let active_obstacles: HashSet<(usize, usize)> = self.obstacles
            .iter()
            .take(obstacle_count)
            .cloned()
            .collect();
        
        // BFS queue
        let mut queue = VecDeque::new();
        // Keep track of visited positions
        let mut visited = HashSet::new();
        
        // (x, y, steps)
        queue.push_back((start.0, start.1, 0));
        visited.insert(start);
        
        // Possible moves: right, down, left, up
        let directions = [(1, 0), (0, 1), (-1, 0), (0, -1)];
        
        while let Some((x, y, steps)) = queue.pop_front() {
            // If we reached the target, return the number of steps
            if (x, y) == target {
                return Some(steps);
            }
            
            // Try each direction
            for (dx, dy) in directions.iter() {
                let nx = match x.checked_add_signed(*dx) {
                    Some(val) => val,
                    None => continue, // Skip if would cause underflow
                };
                
                let ny = match y.checked_add_signed(*dy) {
                    Some(val) => val,
                    None => continue, // Skip if would cause underflow
                };
                
                // Skip if out of bounds
                if nx >= self.width || ny >= self.height {
                    continue;
                }
                
                // Skip if this position is an active obstacle
                if active_obstacles.contains(&(nx, ny)) {
                    continue;
                }
                
                // Skip if already visited
                let next_pos = (nx, ny);
                if visited.contains(&next_pos) {
                    continue;
                }
                
                visited.insert(next_pos);
                queue.push_back((nx, ny, steps + 1));
            }
        }
        
        // No path found
        None
    }
    
    /// Display the maze with the shortest path highlighted if available
    fn display_with_path(&self, obstacle_count: usize) -> String {
        // First find the path
        let path_coords = self.find_path_coords(obstacle_count);
        
        let mut result = String::new();
        let active_obstacles: HashSet<(usize, usize)> = self.obstacles
            .iter()
            .take(obstacle_count)
            .cloned()
            .collect();
        
        for y in 0..self.height {
            for x in 0..self.width {
                if path_coords.contains(&(x, y)) {
                    result.push('*'); // Path
                } else if active_obstacles.contains(&(x, y)) {
                    result.push('#'); // Obstacle
                } else {
                    result.push('.'); // Empty space
                }
            }
            result.push('\n');
        }
        
        result
    }
    
    /// Find the coordinates of the shortest path
    fn find_path_coords(&self, obstacle_count: usize) -> HashSet<(usize, usize)> {
        let start = (0, 0);
        let target = (self.width - 1, self.height - 1);
        let active_obstacles: HashSet<(usize, usize)> = self.obstacles
            .iter()
            .take(obstacle_count)
            .cloned()
            .collect();
        
        // BFS queue with (x, y, path)
        let mut queue = VecDeque::new();
        let mut visited = HashSet::new();
        
        queue.push_back((start.0, start.1, vec![start]));
        visited.insert(start);
        
        // Possible moves: right, down, left, up
        let directions = [(1, 0), (0, 1), (-1, 0), (0, -1)];
        
        while let Some((x, y, path)) = queue.pop_front() {
            if (x, y) == target {
                return path.into_iter().collect();
            }
            
            for (dx, dy) in directions.iter() {
                let nx = match x.checked_add_signed(*dx) {
                    Some(val) => val,
                    None => continue,
                };
                
                let ny = match y.checked_add_signed(*dy) {
                    Some(val) => val,
                    None => continue,
                };
                
                if nx >= self.width || ny >= self.height {
                    continue;
                }
                
                if active_obstacles.contains(&(nx, ny)) {
                    continue;
                }
                
                let next_pos = (nx, ny);
                if visited.contains(&next_pos) {
                    continue;
                }
                
                visited.insert(next_pos);
                let mut new_path = path.clone();
                new_path.push(next_pos);
                queue.push_back((nx, ny, new_path));
            }
        }
        
        HashSet::new()
    }
    
    /// Finds the lowest number of obstacles that make the end unreachable
    /// Returns the minimum number of obstacles required to block all paths to the target
    fn find_min_blocking_obstacles(&self) -> usize {
        // Binary search to find the threshold
        let mut low = 0;
        let mut high = self.obstacles.len();
        
        // If the maze is already unsolvable with no obstacles or always solvable with all obstacles
        if self.find_path_bfs(low).is_none() {
            return 0; // Already blocked with no obstacles
        }
        
        if self.find_path_bfs(high).is_some() {
            return high + 1; // Not blockable with available obstacles
        }
        
        while high - low > 1 {
            let mid = low + (high - low) / 2;
            
            if self.find_path_bfs(mid).is_some() {
                // Path exists with mid obstacles, so the threshold is higher
                low = mid;
            } else {
                // Path doesn't exist with mid obstacles, so threshold is at most mid
                high = mid;
            }
        }
        
        // The low value is the last obstacle count where a path exists
        // The high value is the first obstacle count where no path exists
        high
    }
}

/// Creates a maze with specified dimensions and loads obstacles from a file
fn create_maze<P: AsRef<Path>>(width: usize, height: usize, obstacle_file: P) -> io::Result<Maze> {
    let mut maze = Maze::new(width, height);
    maze.load_obstacles_from_file(obstacle_file)?;
    Ok(maze)
}

/// Solves the maze by creating it with given dimensions, loading obstacles from a file,
/// and finding paths with different obstacle counts
fn solve_maze(width: usize, height: usize, obstacle_file: impl AsRef<Path>, obstacle_count: usize) -> io::Result<()> {
    let maze = create_maze(width, height, obstacle_file)?;
    
    println!("\nTrying with {} obstacles:", obstacle_count);
    match maze.find_path_bfs(obstacle_count) {
        Some(steps) => {
            println!("Path found! Length: {}", steps);
            println!("Path visualization:");
            print!("{}", maze.display_with_path(obstacle_count));
        },
        None => println!("No path found!"),
    }
    
    Ok(())
}

/// Finds the minimum number of obstacles needed to make the maze unsolvable
fn find_min_blocking_obstacles(width: usize, height: usize, obstacle_file: impl AsRef<Path>) -> io::Result<(usize, usize)> {
    let maze = create_maze(width, height, obstacle_file)?;
    let min_obstacles = maze.find_min_blocking_obstacles();
    
    Ok(maze.obstacles[min_obstacles - 1])
}

fn main() {
    // Example usage with default values
    if let Err(e) = solve_maze(71, 71, "input.txt", 1024) {
        eprintln!("Error solving maze: {}", e);
    }
    
    // Find minimum number of obstacles to block all paths
    let min_obstacles = find_min_blocking_obstacles(71, 71, "input.txt");
    match min_obstacles {
        Ok(count) => println!("First blocking obstacle: {:?}", count),
        Err(e) => eprintln!("Error finding minimum blocking obstacles: {}", e),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_solve_maze_with_sample() -> io::Result<()> {
        // Redirect stdout to capture the output
        let maze = create_maze(7, 7, "sample.txt")?;

        let path_result = maze.find_path_bfs(12);
        assert_eq!(path_result, Some(22), "Path length should be 22 with 12 obstacles");
        
        Ok(())
    }

    #[test]
    fn test_solve_maze_with_input() -> io::Result<()> {
        // Redirect stdout to capture the output
        let maze = create_maze(71, 71, "input.txt")?;

        let path_result = maze.find_path_bfs(1024);
        assert_eq!(path_result, Some(268), "Path length mismatch with 1024 obstacles");
        
        Ok(())
    }
    
    #[test]
    fn test_min_blocking_obstacles() -> io::Result<()> {
        let maze = create_maze(7, 7, "sample.txt")?;
        let min_obstacles = maze.find_min_blocking_obstacles();
        
        // Based on the sample data, we expect this many obstacles are needed to block all paths
        assert!(min_obstacles > 0, "There should be a minimum number of obstacles needed");
        assert!(min_obstacles <= maze.obstacles.len(), "Min obstacles should not exceed total obstacles");
        
        // Verify that with min_obstacles-1, a path exists
        assert!(maze.find_path_bfs(min_obstacles - 1).is_some(), 
            "Path should exist with {} obstacles", min_obstacles - 1);
        
        // Verify that with min_obstacles, no path exists
        assert!(maze.find_path_bfs(min_obstacles).is_none(), 
            "No path should exist with {} obstacles", min_obstacles);

        assert_eq!(maze.obstacles[min_obstacles - 1], (6, 1), 
            "First blocking obstacle should be at (6, 1)");
        
        Ok(())
    }

    #[test]
    fn test_min_blocking_obstacles_input() -> io::Result<()> {
        let maze = create_maze(71, 71, "input.txt")?;
        let min_obstacles = maze.find_min_blocking_obstacles();
        
        // Based on the sample data, we expect this many obstacles are needed to block all paths
        assert!(min_obstacles > 0, "There should be a minimum number of obstacles needed");
        assert!(min_obstacles <= maze.obstacles.len(), "Min obstacles should not exceed total obstacles");
        
        // Verify that with min_obstacles-1, a path exists
        assert!(maze.find_path_bfs(min_obstacles - 1).is_some(), 
            "Path should exist with {} obstacles", min_obstacles - 1);
        
        // Verify that with min_obstacles, no path exists
        assert!(maze.find_path_bfs(min_obstacles).is_none(), 
            "No path should exist with {} obstacles", min_obstacles);

        assert_eq!(maze.obstacles[min_obstacles - 1], (64, 11), 
            "First blocking obstacle should be at (64, 11)");
        
        Ok(())
    }
}


