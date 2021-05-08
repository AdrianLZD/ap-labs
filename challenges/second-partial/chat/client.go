// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"fmt"
)

//!+
func main() {

	if len(os.Args) != 5 || os.Args[1] != "-user" || os.Args[3] != "-server" {
		fmt.Println("Wrong Usage. Usage [go run client.go -user <user> -server <server>]")
		return
	}

	conn, err := net.Dial("tcp", os.Args[4])
	if err != nil {
		log.Fatal(err)
	}

	username := os.Args[2]
	fmt.Fprintf(conn, username)

	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}
//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
