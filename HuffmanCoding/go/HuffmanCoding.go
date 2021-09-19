package main

import (
	"container/heap"
	"fmt"
	"io/ioutil"
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

func compress(chars []rune) (root *Node, compressed []bool) {
	var freq [256]int

	for _, v := range chars {
		freq[v]++
	}

	root = buildTrie(freq[0:256])

	var st [256]string

	buildCode(st[0:256], root, "")

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

	fmt.Println("Compressed length: ", len(compressed)/8)

	return
}

func expand(root *Node, compressed []bool, length int) string {
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

	return builder.String()
}

func main() {
	start := time.Now()
	content, err := ioutil.ReadFile("tale.txt") // the file is inside the local directory
	if err != nil {
		fmt.Println("Err")
	}

	chars := []rune(string(content))

	fmt.Println("Input ", len(chars))

	root, compressed := compress(chars)

	fmt.Println("Compressed length: ", len(compressed)/8)

	output := expand(root, compressed, len(chars))

	if string(content) == output {
		fmt.Println("Success")
	}

	elapsed := time.Since(start)
	fmt.Println("Took ", elapsed)
}
