package main

import (
    "net/http"
	"fmt"
	"time"
)

type User struct {
    UserId  string
    Name string
    Mobile string
    WxId string
    Status int
	Type int
	CarId string
	Online bool
}

type Car struct {
	CarId string
	Plate string
	Brand string
	Model string
	Type int
	Seats int
	Volume float32
}

type Position struct {
    Longtitude float64 //x
    Latitude float64 //y
    Altitude float64 //z
    Description string
}

type PositionLog struct {
	 UserId string
	 Pos string
	 Timestamp string	 
}

func (p *Position) toString() string {
    return "22.13323232,112.888888,0,'深圳'"
}


func main() {
    http.Handle("/doc/",http.StripPrefix("/doc",http.FileServer(http.Dir("C:\\Go\\doc"))))
    fmt.Println("Server Running...\nhttp://localhost:8080/admin/")
    http.ListenAndServe(":8080", nil)
}