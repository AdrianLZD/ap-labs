
// Package geometry defines simple types for plane geometry.
package main

import (
	"math"
	"math/rand"
	"os"
	"fmt"
	"strconv"
	"time"
	"sort"
)

const MinCoord = -100
const MaxCoord = 100

type Point struct{x, y float64 }

func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

// A Path is a journey connecting the points with straight lines.
type Path []Point

func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	sum += path[len(path)-1].Distance(path[0])
	return sum
}

func createFigure(sides int) Path{
	path := make(Path, sides)
	createPoints(path, sides)
	polarPath := findPolarPath(path)
	sortPolarPath(polarPath)
	path = polarPathToCartesian(polarPath)

	return path
}

func createPoints(path Path, sides int){
	rand.Seed(time.Now().UnixNano())
	for i:=0; i<sides; i++ {
		point := Point{MinCoord + rand.Float64() * (MaxCoord -MinCoord),
					   MinCoord + rand.Float64() * (MaxCoord -MinCoord)}
		path[i] = point
	}
}

func findPolarPath(path Path) Path{
	polarPath := make(Path, len(path));
	var angle, radius float64

	for i, point := range path{
		angle = math.Atan2(point.Y(), point.X())
		radius = math.Sqrt(math.Pow(point.X(), 2) + math.Pow(point.Y(), 2))
		polarPath[i] = Point{radius, angle}
	}
	
	return polarPath
}

func sortPolarPath(polarPath Path){
	sort.Slice(polarPath, func(i, j int) bool{
		if polarPath[i].Y() < polarPath[j].Y() {
			return true
		}
		if polarPath[i].Y() > polarPath[j].Y() {
			return false
		}
		return polarPath[i].X() > polarPath[j].X()
	})
}

func polarPathToCartesian(polarPath Path) Path{
	cPath := make(Path, len(polarPath));
	var newX, newY float64
	for i,point := range polarPath {
		newX = point.X() * math.Cos(point.Y())
		newY = point.X() * math.Sin(point.Y())
		cPath[i] = Point{newX,newY}
	}
	return cPath
}

func printResults(path Path){
	sides := len(path)
	fmt.Printf("- Generating a [%d] sides figure\n", sides)
	fmt.Println("- Figure's vertices")
	for i:=0; i<sides; i++ {
		fmt.Printf("  - (%f, %f)\n", path[i].X(), path[i].Y())
	}
	fmt.Printf("- Figure's Perimeter\n  -")
	for i:=0; i<sides-1; i++ {
		fmt.Printf(" %f +", path[i].Distance(path[i+1]))
	}
	fmt.Printf(" %f", path[sides-1].Distance(path[0]))
	fmt.Printf(" = %f\n", path.Distance())
}

func main() {
	if len(os.Args)!=2 {
		fmt.Println("[ERROR] Wrong usage syntax.")
		os.Exit(0)
	}

	sides, err := strconv.Atoi(os.Args[1])
	
	if err != nil {
		fmt.Println("[ERROR] The first parameter should be an integer.")
		os.Exit(0)
	}

	if sides < 3 {
		fmt.Println("[ERROR] A figure must have more than two sides.")
		os.Exit(0)
	}
	
	figure := createFigure(sides)
	printResults(figure)

}


