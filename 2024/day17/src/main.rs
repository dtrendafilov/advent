use std::fs::read_to_string;

#[derive(Debug, Clone)]
enum OpCode {
    Adv, // 0
    Bxl, // 1
    Bst, // 2
    Jnz, // 3
    Bxc, // 4
    Out, // 5
    Bdv, // 6
    Cdv, // 7
}

impl OpCode {
    fn from_i32(value: i32) -> Option<OpCode> {
        match value {
            0 => Some(OpCode::Adv),
            1 => Some(OpCode::Bxl),
            2 => Some(OpCode::Bst),
            3 => Some(OpCode::Jnz),
            4 => Some(OpCode::Bxc),
            5 => Some(OpCode::Out),
            6 => Some(OpCode::Bdv),
            7 => Some(OpCode::Cdv),
            _ => None,
        }
    }
}

#[derive(Debug, Clone)]
struct Instruction {
    opcode: OpCode,
    argument: i32,
}

#[derive(Debug, Clone)]
struct ProgramState {
    register_a: usize,
    register_b: usize,
    register_c: usize,
    instructions: Vec<Instruction>,
    pc: usize,
}

impl ProgramState {
    fn combo_arg(&self, arg: i32) -> usize {
        match arg {
            4 => self.register_a,
            5 => self.register_b,
            6 => self.register_c,
            _ => arg as usize,
        }
    }

    fn run(&mut self) -> Vec<i32> {
        let instruction_count = self.instructions.len();
        let mut output = Vec::new();
        
        while self.pc < instruction_count {

            
            // Get current instruction
            let instruction = &self.instructions[self.pc];
            
            // Execute instruction
            match instruction.opcode {
                OpCode::Adv => {
                    self.register_a /= 1 << self.combo_arg(instruction.argument);
                    self.pc += 1;
                },
                OpCode::Bxl => {
                    self.register_b ^= instruction.argument as usize;
                    self.pc += 1;
                },
                OpCode::Bst => {
                    self.register_b = self.combo_arg(instruction.argument) % 8;
                    self.pc += 1;
                },
                OpCode::Jnz => {
                    if self.register_a != 0 {
                        self.pc = (instruction.argument / 2) as usize;
                    } else {
                        self.pc += 1;
                    }
                },
                OpCode::Bxc => {
                    self.register_b ^= self.register_c;
                    self.pc += 1;
                },
                OpCode::Out => {
                    output.push((self.combo_arg(instruction.argument) % 8) as i32);
                    self.pc += 1;
                },
                OpCode::Bdv => {
                    self.register_b = self.register_a / (1 << self.combo_arg(instruction.argument));
                    self.pc += 1;
                },
                OpCode::Cdv => {
                    self.register_c = self.register_a / (1 << self.combo_arg(instruction.argument));
                    self.pc += 1;
                },
            }
        }
        
        output
    }
}

fn read_program_file(file_path: &str) -> (ProgramState, Vec<i32>) {
    let content = read_to_string(file_path).expect("Failed to read file");
    let mut register_a = 0;
    let mut register_b = 0;
    let mut register_c = 0;
    let mut instructions = Vec::new();
    let mut values = Vec::new();
    
    for line in content.lines() {
        if line.starts_with("Register A:") {
            register_a = line.split(':').nth(1).unwrap_or("0").trim().parse::<usize>().unwrap_or(0);
        } else if line.starts_with("Register B:") {
            register_b = line.split(':').nth(1).unwrap_or("0").trim().parse::<usize>().unwrap_or(0);
        } else if line.starts_with("Register C:") {
            register_c = line.split(':').nth(1).unwrap_or("0").trim().parse::<usize>().unwrap_or(0);
        } else if line.starts_with("Program:") {
            // Parse program line like "Program: 0,1,5,4,3,0"
            let program_part = line.split(':').nth(1).unwrap_or("").trim();
            values = program_part
                .split(',')
                .filter_map(|s| s.trim().parse::<i32>().ok())
                .collect();
            
            // Convert the flat list of numbers into pairs of opcode and argument
            for chunk in values.chunks(2) {
                if chunk.len() == 2 {
                    if let Some(opcode) = OpCode::from_i32(chunk[0]) {
                        instructions.push(Instruction {
                            opcode,
                            argument: chunk[1],
                        });
                    } else {
                        eprintln!("Invalid opcode: {}", chunk[0]);
                    }
                }
            }
        }
    }
    
    return (ProgramState { 
        register_a,
        register_b,
        register_c,
        instructions,
        pc: 0, 
    }, values);
}

#[cfg(test)]
fn self_print_sample(expected_code: Vec<i32>) -> usize {
    let mut a: usize = 0;
    /*
    do {
        a = a / 8;
        out(a % 8);
    } while (a != 0);
    */

    for d in expected_code.iter().rev() {
        a = (a * 8) + *d as usize;
    }
    a * 8
}

fn self_print_input(expected_code: &Vec<i32>, index: i32, a: usize) -> Option<usize> {
    /*
    do {
        b = a % 8
        b1 = b ^ 5
        c = a / (1 << b1)
        a = a / 8
        b2 = b1 ^ c
        b3 = b2 ^ 6
        out(b3 % 8)
    } while (a != 0);
    */

    if index < 0 {
        return Some(a);
    }

    let d = expected_code[index as usize];
    for b in 0..8 {
        let candidate = a * 8 + b as usize;
        let b1 = b ^ 5;
        let c = candidate / (1 << b1);
        let b2 = b1 ^ c;
        let b3 = b2 ^ 6;
        let out = b3 % 8;
        if out == d as usize {
            if let Some(final_a) = self_print_input(expected_code, index - 1, candidate) {
                return Some(final_a);
            }
        }
    }
    Option::None
}

fn main() {
    // Read program from input.txt
    let (mut program_state, code) = read_program_file("input.txt");
    
    println!("Register A: {}", program_state.register_a);
    println!("Register B: {}", program_state.register_b);
    println!("Register C: {}", program_state.register_c);
    println!("Instructions:");
    for (i, instruction) in program_state.instructions.iter().enumerate() {
        println!("  {}: {:?} {}", i, instruction.opcode, instruction.argument);
    }
    println!("Program code: {:?}", code);

    // Run the program with its default register values
    let output = program_state.run();
    
    // Format the output as comma-separated numbers
    let output_str = output.iter()
        .map(|num| num.to_string())
        .collect::<Vec<String>>()
        .join(",");

    println!("Part one: {}", output_str);
    
    // Use self_print to find the register A value that makes the program output equal to the code
    println!("\nPart two: Finding register A value that produces self-printing program...");
    if let Some(result) = self_print_input(&code, code.len() as i32 - 1, 0) {
        println!("Found register A value: {}", result);
        println!("Part two answer: {}", result);
        program_state.register_a = result;
        program_state.pc = 0;
        let output = program_state.run();
        println!("Part two output: {:?}", output);
    } else {
        println!("No valid register A value found.");
    }
    
    
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sample() {
        let (mut program_state, _) = read_program_file("sample.txt");
        let output = program_state.run();
        let expected = vec![4, 6, 3, 5, 6, 3, 5, 2, 1, 0];
        
        assert_eq!(output, expected, "Program output doesn't match expected sequence");
    }

    #[test]
    fn test_sample2() {
        let (mut program_state, code) = read_program_file("sample2.txt");
        let output = program_state.run();
        
        assert_eq!(output, code, "Program did not self-print correctly");
    }

    #[test]
    fn test_input() {
        let (mut program_state, _) = read_program_file("input.txt");
        let output = program_state.run();
        let expected = vec![7, 1, 3, 4, 1, 2, 6, 7, 1];
        
        assert_eq!(output, expected, "Program output doesn't match expected sequence");
    }
    
    #[test]
    fn test_self_print_sample() {
        let (program_state, code) = read_program_file("sample2.txt");
        let result = self_print_sample(code);
        
        assert_eq!(result, program_state.register_a, "Self-printing register A value is incorrect");
    }

    #[test]
    fn test_self_print_input() {
        let (_, code) = read_program_file("input.txt");
        let result = self_print_input(&code, code.len() as i32 - 1, 0).unwrap_or(0);
        
        assert_eq!(result, 109019476330651, "Self-printing register A value is incorrect");
    }
}
