package main

import (
	"container/heap"
	"fmt"
	"io/ioutil"
	"runtime"
	"strings"
	"time"
)

type Node struct {
	ch   rune
	freq int
	// The index is needed by update and is maintained by the heap.Interface methods.
	index       int // The index of the item in the heap.
	left, right *Node
}

type CompressedPart struct {
	compressed []bool
	index      int
	length     int
}

type UncompressedPart struct {
	output string
	index  int
}

// A PriorityQueue implements heap.Interface and holds Items.
type PriorityQueue []*Node

func (pq PriorityQueue) Len() int { return len(pq) }

func (pq PriorityQueue) Less(i, j int) bool {
	return pq[i].freq < pq[j].freq
}

func (pq PriorityQueue) Swap(i, j int) {
	pq[i], pq[j] = pq[j], pq[i]
	pq[i].index = i
	pq[j].index = j
}

func (pq *PriorityQueue) Push(x interface{}) {
	n := len(*pq)
	item := x.(*Node)
	item.index = n
	*pq = append(*pq, item)
}

func (pq *PriorityQueue) Pop() interface{} {
	old := *pq
	n := len(old)
	item := old[n-1]
	old[n-1] = nil  // avoid memory leak
	item.index = -1 // for safety
	*pq = old[0 : n-1]
	return item
}

// update modifies the priority and value of an Item in the queue.
func (pq *PriorityQueue) update(item *Node, ch rune, freq int) {
	item.ch = ch
	item.freq = freq
	heap.Fix(pq, item.index)
}

func (n *Node) isLeaf() bool {
	return (n.left == nil) && (n.right == nil)
}

func buildTrie(freq []int) *Node {
	var pq PriorityQueue

	for i, v := range freq {
		if v > 0 {
			heap.Push(&pq, &Node{ch: rune(i), freq: v, left: nil, right: nil})
		}
	}

	for pq.Len() > 1 {
		left := heap.Pop(&pq).(*Node)
		right := heap.Pop(&pq).(*Node)
		parent := &Node{
			ch:    '0',
			freq:  left.freq + right.freq,
			left:  left,
			right: right,
		}
		heap.Push(&pq, parent)
	}

	return heap.Pop(&pq).(*Node)
}

func buildCode(st []string, x *Node, s string) {
	if !x.isLeaf() {
		buildCode(st, x.left, s+string('0'))
		buildCode(st, x.right, s+string('1'))
	} else {
		st[x.ch] = s
	}
}

func compress(chars []rune, root *Node, st []string, index int, ch chan CompressedPart) {

	var compressed []bool
	for _, v := range chars {
		code := []rune(st[v])
		for _, c := range code {
			if c == '0' {
				compressed = append(compressed, false)
			} else if c == '1' {
				compressed = append(compressed, true)
			}
		}
	}

	ch <- CompressedPart{compressed: compressed, index: index, length: len(chars)}
}

func buildTrieAndCode(chars []rune) (root *Node, str []string) {
	var freq [256]int

	for _, v := range chars {
		freq[v]++
	}

	root = buildTrie(freq[0:256])

	var st [256]string

	buildCode(st[0:256], root, "")

	str = st[0:256]

	return
}

func expand(root *Node, compressed []bool, length int, index int, out chan UncompressedPart) {
	builder := strings.Builder{}

	j := 0

	for i := 0; i < length; i++ {
		x := root
		for !x.isLeaf() {
			bit := compressed[j]
			j++
			if bit {
				x = x.right
			} else {
				x = x.left
			}
		}
		builder.WriteRune(x.ch)
	}

	out <- UncompressedPart{output: builder.String(), index: index}
}

func printStats(mem runtime.MemStats) {
	runtime.ReadMemStats(&mem)
	fmt.Println("mem.Alloc:", mem.Alloc)
	fmt.Println("mem.TotalAlloc:", mem.TotalAlloc)
	fmt.Println("mem.HeapAlloc:", mem.HeapAlloc)
	fmt.Println("mem.NumGC:", mem.NumGC)
	fmt.Println("-----")
}

func main() {
	var mem runtime.MemStats
	printStats(mem)
	start := time.Now()
	content, err := ioutil.ReadFile("bible.txt") // the file is inside the local directory
	if err != nil {
		fmt.Println("Err")
	}

	chars := []rune(string(content))

	fmt.Println("Input ", len(chars))

	elapsedFile := time.Since(start)
	fmt.Println("File Read Took ", elapsedFile)

	root, st := buildTrieAndCode(chars)

	ch := make(chan CompressedPart)

	var div8 float32 = float32(len(chars)) / 8

	for i := 0; i < 8; i++ {
		start := i * int(div8)
		end := (i + 1) * int(div8)
		if i == 7 {
			end = len(chars)
		}
		go compress(chars[start:end], root, st, i, ch)
	}

	var compressed []CompressedPart
	for i := 0; i < 8; i++ {
		compressed = append(compressed, <-ch)
	}

	compressedLength := 0

	for _, val := range compressed {
		fmt.Println(val.index, val.length)
		compressedLength += len(val.compressed)
	}

	fmt.Println("Compressed length: ", compressedLength/8)

	elapsedCompress := time.Since(start)
	fmt.Println("Compress Took ", elapsedCompress)

	out := make(chan UncompressedPart)

	for _, v := range compressed {
		go expand(root, v.compressed, v.length, v.index, out)
	}

	var allOut []UncompressedPart

	for i := 0; i < 8; i++ {
		allOut = append(allOut, <-out)
	}

	builder := strings.Builder{}

	for i := 0; i < 8; i++ {
		for _, u := range allOut {
			if u.index == i {
				builder.WriteString(u.output)
				break
			}
		}
	}

	output := builder.String()

	if string(content) == output {
		fmt.Println("Success")
	}

	elapsed := time.Since(start)
	fmt.Println("Total Took ", elapsed)
	printStats(mem)
}
