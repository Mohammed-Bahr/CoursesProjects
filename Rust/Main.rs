fn main() {
    println!("Welcome to Rust Learning by Practice!");

    // --- 1. Hello World & Printing ---
    // Exercise: Try printing your name using a variable.
    println!("Hello, Rust!"); 
    
    // TODO: Practice printing a formatted string using println!("Hello, {}!", name);

    // --- 2. Variables and Mutability ---
    // By default, variables are immutable.
    let x = 5; 
    // x = 6; // This would cause a compile error!

    // Use 'mut' to make a variable mutable.
    let mut y = 10;
    y = 15; 
    println!("y is now: {}", y);

    // TODO: Create an immutable variable and a mutable variable, then try to change both.

    // --- 3. Data Types ---
    // Scalar: i32 (integer), f64 (float), bool (boolean), char (character)
    let a: i32 = -42;
    let b: f64 = 3.14;
    let c: bool = true;
    let d: char = 'z';

    // Compound: Tuple, Array
    let tuple: (i32, f64, &str) = (500, 6.4, "hello");
    let array: [i32; 3] = [1, 2, 3];

    // TODO: Create a tuple containing your age and height, then print the first element.

    // --- 4. Functions ---
    // Functions use 'fn'. Parameters must have types.
    let sum = add_numbers(5, 10);
    println!("Sum: {}", sum);

    // --- 5. Control Flow ---
    // if/else
    let number = 7;
    if number < 5 {
        println!("small");
    } else if number == 7 {
        println!("lucky seven!");
    } else {
        println!("large");
    }

    // Loops: loop, while, for
    println!("Counting to 3:");
    for i in 1..4 {
        println!("{}", i);
    }

    // TODO: Write a loop that prints only even numbers from 1 to 10.

    // --- 6. Ownership (Crucial Rust Concept) ---
    // Ownership prevents memory leaks. 
    // Value is moved by default for complex types (like String).
    let s1 = String::from("hello");
    let s2 = s1; // s1 is moved to s2. s1 is no longer valid.
    // println!("{}", s1); // This would fail!
    println!("s2: {}", s2);

    // Borrowing with &
    let s3 = String::from("world");
    let len = calculate_length(&s3); // Borrowing s3
    println!("The length of '{}' is {}.", s3, len);

    // TODO: Try to move a String and then print the original variable to see the error.

    // --- 7. Structs & Enums ---
    let user = User {
        username: String::from("rustacean"),
        email: String::from("rust@example.com"),
        active: true,
    };
    println!("User: {}, Active: {}", user.username, user.active);

    let status = Status::Active;
    match status {
        Status::Active => println!("System is active"),
        Status::Inactive => println!("System is inactive"),
    }
}

fn add_numbers(a: i32, b: i32) -> i32 {
    a + b // No semicolon means this is the return value
}

fn calculate_length(s: &String) -> usize {
    s.len()
}

struct User {
    username: String,
    email: String,
    active: bool,
}

enum Status {
    Active,
    Inactive,
}
