

use std::{thread, time};

fn main() {
    use whiskey_manipulator::*;
    let pid=555366; 
    println!("attaching");
    let process=attach(pid,byteorder::LITTLE_ENDIAN).unwrap();
    println!("donw");
    let addr:usize=0x564b2b433294;
    println!("{}",read_32bit_address(&process,addr).unwrap());
    println!("keeping health at 100");
    /*
    loop{
        thread::sleep(time::Duration::from_millis(600));
        println!("reading");
        println!("{}",read_32bit_address(&process,addr).unwrap());
        if read_32bit_address(&process,addr).unwrap()!=100{
            println!("fixing");
            write_32bit_address(&process,addr,100).unwrap();
        }
        
    }
    */
    
    dettach(&process);
     
    
}
