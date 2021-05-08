// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)


type client chan<- string // an outgoing message channel

type User struct{
	channel client
	connection net.Conn
	username string
	address string
	loginTime string
	kicked bool
}

var (
	entering = make(chan client)
	leaving  = make(chan client)
	sender = make(chan string)
	messages = make(chan string)
	users = make(map[string] *User)
	firstUser = true
	adminActive = false
	adminUsername string
	serverName string
	lineCleaner string
	lineForward string
)


func broadcaster() {
	clients := make(map[client]bool)
	for {
		select {
		case msg := <-messages:
			for cli := range clients {			
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
	
}

func handleConn(conn net.Conn) {

	ch := make(chan string) // outgoing client messages

	// Get the connection username
	buff := make([]byte, 128)
	conn.Read(buff)
	who := string(buff[:])
	who = strings.Trim(who, "\x00")

	go clientWriter(conn, ch, who)

	if _, inServer := users[who]; inServer {
		ch <- fmt.Sprintf("%s > The username [%s] is already at use, try another one", serverName, who)
		conn.Close()
		return
	}

	var newClient = User{
		username : who,
		address : conn.RemoteAddr().String(),
		loginTime : strings.Split(time.Now().String(),".")[0],
		channel : ch,
		connection : conn,
	}

	users[who] = &newClient
	
	ch <- fmt.Sprintf("%s > Welcome to the Simple IRC Server\n%s > Your user [%s] is succesfully logged", serverName, serverName, who)
	fmt.Printf("%s > New connected user [%s]\n", serverName, who)
	messages <- fmt.Sprintf("%s > New connected user [%s]", serverName, who)
	entering <- ch

	if firstUser {
		ch <- fmt.Sprintf("%s > Congrats, you were the first user", serverName)
		firstUser = false
	}

	if !adminActive {
		promoteAdmin(who)
	}
	
	// Create the scanner for the user
	input := bufio.NewScanner(conn)
	for input.Scan() {
		trimMsg := strings.TrimSpace(input.Text())
		// If user sent an empty message, discard it
		if len(trimMsg) < 1 {
			ch <- fmt.Sprintf("%s > ", who)
			continue
		}

		if trimMsg[0] == '/' {
			msgSplit := strings.SplitN(trimMsg, " ", 2)
			cmd := msgSplit[0]
			
			switch cmd{
			case "/users":
				if len(msgSplit) > 1 {
					ch <-  fmt.Sprintf("%s > Wrong usage for [/users]. Usage /users", serverName)
				}else{
					sendUsersList(ch)
				}
			case "/msg":
				if len(msgSplit) < 2 {
					ch <- fmt.Sprintf("%s > Wrong usage for [/msg]. Usage /msg <user> <msg>", serverName)
				}else{
					sendPrivateMessage(who, msgSplit[1])
				}
			case "/time":
				if len(msgSplit) > 1 {
					ch <-  fmt.Sprintf("%s > Wrong usage for [/time]. Usage /time", serverName)
				}else{
					sendLocalTime(ch)
				}
			case "/user":
				if len(msgSplit) != 2 {
					ch <-  fmt.Sprintf("%s > Wrong usage for [/user]. Usage /user <user>", serverName)
				}else{
					sendUserInfo(ch, msgSplit[1])
				}
			case "/kick":
				if who == adminUsername {
					if len(msgSplit) != 2 {
						ch <-  fmt.Sprintf("%s > Wrong usage for [/kick]. Usage /user <kick>", serverName)
					}else{
						kickUser(ch, msgSplit[1])
					}
				}else{
					ch <- fmt.Sprintf("%s > You are not admin of the server", serverName)
				}
			default:
				ch <- fmt.Sprintf("%s > The command %s is invalid", serverName, trimMsg)
			}
		}else{
			messages <- fmt.Sprintf("%s > %s", who, trimMsg)
		}
	}
	// NOTE: ignoring potential errors from input.Err()
	removeClient(who)
}

func promoteAdmin(username string){
	if user, inServer := users[username]; inServer {
		adminUsername = username
		user.channel <- fmt.Sprintf("%s > You're the new IRC Server ADMIN", serverName)
		fmt.Printf("%s > [%s] was promoted as the channel ADMIN\n", serverName, username)
		adminActive = true
	}
}

func sendUsersList(ch client) {
	msg := ""
	for user := range users {
		msg = msg + fmt.Sprintf("%s > %s - connected since %s\n", serverName, users[user].username, users[user].loginTime )
	}
	ch <- msg[:len(msg)-1]
}

func sendPrivateMessage(from string, content string){
	cntSplit := strings.SplitN(content, " ", 2)
	if len(cntSplit) != 2 {
		users[from].channel <- fmt.Sprintf("%s > Wrong usage for [/msg]. Usage /msg <user> <msg>", serverName)
		return
	}

	if user, inServer := users[cntSplit[0]]; inServer {
		user.channel <- fmt.Sprintf("[PM] %s > %s", from, cntSplit[1])
		fmt.Fprintf(users[from].connection,fmt.Sprintf("%s > " , from ))
	}else{
		users[from].channel <- fmt.Sprintf("%s > %s is not in the server", serverName, cntSplit[0])
	}
}

func sendLocalTime(ch client) {
	timeZone := "America/Mexico_City"
	loc, err := time.LoadLocation(timeZone)
	if err != nil {
		ch <- fmt.Sprintf("%s > Could not get local time", serverName)
		return
	}
	curTime := time.Now().In(loc).Format("15:04")
	ch <- fmt.Sprintf("%s > Local Time: %s %s", serverName, timeZone, curTime)
}

func sendUserInfo(ch client, username string) {
	if user, inServer := users[username]; inServer {
		msg := ""
		msg += "Username: " + user.username
		msg += ", IP: " + user.address
		msg += " Connected since: " + user.loginTime
		ch <- fmt.Sprintf("%s > %s", serverName, msg)
	}else{
		ch <- fmt.Sprintf("%s > %s is not in the server", serverName, username)
	}
}

func kickUser(ch client, username string){
	if user, inServer := users[username]; inServer {
		user.channel <- fmt.Sprintf("%s > You are kicked from this channel", serverName)
		user.channel <- fmt.Sprintf("%s > Bad language is not allowed on this channel", serverName)
		users[username].kicked = true
		fmt.Fprintf(user.connection, "\r")
		fmt.Fprintf(user.connection, "   \n")
		user.connection.Close()
		messages <- fmt.Sprintf("%s > [%s] was kicked from the channel for bad language policy violation", serverName, username)
		fmt.Printf("%s > [%s] was kicked\n", serverName, username)
	}else{
		ch <- fmt.Sprintf("%s > %s is not in the server", serverName, username)
	}
}

func removeClient(username string) {
	if !users[username].kicked {
		fmt.Printf("%s > [%s] left channel\n", serverName, username)
		messages <- fmt.Sprintf("%s > [%s] left channel", serverName, username)
		users[username].connection.Close()
	}
	
	delete(users, username)
	if username == adminUsername {
		for user := range users {
			promoteAdmin(users[user].username)
			adminUsername = users[user].username
			return
		}
		adminActive = false
	}
}

func clientWriter(conn net.Conn, ch <-chan string, who string) {
	for msg := range ch {
		sender := strings.Split(msg, " ")[0]
		if sender != who{
			fmt.Fprintf(conn, lineCleaner)
			fmt.Fprintf(conn, fmt.Sprintf("%s\n%s > " ,msg, who)) // NOTE: ignoring network errors
		}else{
			fmt.Fprintf(conn, fmt.Sprintf("%s > " , who))
		}
		
	}
}

//!-handleConn

//!+main
func main() {

	if len(os.Args) != 5 || os.Args[1] != "-host" || os.Args[3] != "-port" {
		fmt.Println("Wrong Usage. Usage [go run server.go -host <host> -port <port>]")
		return
	}
	
	address := os.Args[2] + ":" + os.Args[4]
	listener, err := net.Listen("tcp", address)
	
	if err != nil {
		log.Fatal(err)
	}

	for i := 0; i< 512; i++ {
		lineCleaner += "\b \b"
		lineForward += "\x1b[C"
	}


	serverName = "irc-server"

	fmt.Printf("%s > Simple IRC Server started at %s\n", serverName, address)
	fmt.Printf("%s > Ready for receiving new clients\n", serverName)

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
