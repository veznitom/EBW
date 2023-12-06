pub enum Token {
    Number(i64),
    Text(String),
    Function(Func),
    Symbol(Oper),
    Equal,
    Colon,
    Mark,
    Comma,
}

pub enum Oper {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
}

pub enum Func {
    Sum,
    Sin,
    Cos,
    Avg,
    Pow,
    Sqr,
}

enum State {
    Number,
    Text,
    Other,
}

fn lex_expression(seq: &str) -> Result<Vec<Token>, String> {
    let mut state: State = State::Other;
    let mut tint: i64 = 0;
    let mut tstr: String = String::new();
    let mut index: usize = 0;
    let chars: Vec<char> = seq.chars().collect();
    let mut tokens: Vec<Token> = vec![];
    while index < seq.len() {
        let c = chars[index];
        match (c, &state) {
            (c, State::Other) if c.is_digit(10) => {
                state = State::Number;
                tint = c.to_digit(10).unwrap() as i64;
                index += 1;
            }
            (c, State::Number) if c.is_digit(10) => {
                tint *= 10;
                tint += c.to_digit(10).unwrap() as i64;
                index += 1;
            }
            (c, State::Number) => {
                tokens.push(Token::Number(tint));
                tint = 0;
                state = if c.is_alphabetic() {
                    State::Text
                } else {
                    State::Other
                }
            }
            (c, State::Text) if c.is_alphabetic() => {
                tstr.push(c);
                index += 1;
            }
            (c, State::Text) => {
                match tstr.as_str() {
                    "sum" => tokens.push(Token::Function(Func::Sum)),
                    "sin" => todo!(),
                    "cos" => todo!(),
                    "avg" => todo!(),
                    "pow" => todo!(),
                    "sqr" => todo!(),
                    _ => return Err("Fuck you, invalid function!".to_string()),
                }
                tstr = String::new();
                state = if c.is_digit(10) {
                    State::Number
                } else {
                    State::Other
                }
            }
            (c, State::Other) => {
                match c {
                    '+' => tokens.push(Token::Symbol(Oper::Add)),
                    '-' => todo!(),
                    '*' => todo!(),
                    '/' => todo!(),
                    '%' => todo!(),
                    _ => return Err("Fuck you, invalid char!".to_string()),
                }
                index += 1;
            }
            _ => return Err("Fuck you, total!".to_string()),
        }
    }
    todo!()
}

pub fn lex(seq: &str) -> Result<Vec<Token>, String> {
    match seq.chars().next() {
        Some('=') => lex_expression(seq),
        _ => Ok(vec![Token::Text(seq.to_string())]),
    }
}
