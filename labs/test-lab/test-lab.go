package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
  if(len(os.Args)==1){
    fmt.Println("Error. Please enter a name.")
    os.Exit(0);
  }
  
  argsNoBrackets := strings.Join(os.Args[1:], " ")
	fmt.Print("Hello ",argsNoBrackets, ", Welcome to the Jungle\n")
}

