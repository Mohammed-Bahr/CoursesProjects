fn divide(numerator: f64, denominator: f64) -> Result<f64, String> {
    if denominator == 0.0 {
        return Err("Division by zero is not allowed".to_string());
    }
    Ok(numerator / denominator)
}

fn main() {
    let result = divide(10.0, 2.0);
    match result {
        Ok(value) => println!("Result: {value}"),
        Err(error) => println!("Error: {error}"),
    }
}
