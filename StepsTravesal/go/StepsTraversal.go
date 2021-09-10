package main

import "fmt"
import "time"

func jump(steps int) int64 {
	if steps > 0 {
	return jump(steps - 1) + jump(steps - 2) + jump(steps - 3)
	} else {
		return 1
	}
}

func main() {
	start := time.Now()
	fmt.Println(jump(30))
	elapsed := time.Since(start)
    	fmt.Println("Took ", elapsed)
}
