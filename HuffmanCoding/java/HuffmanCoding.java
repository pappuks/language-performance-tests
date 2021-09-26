
import java.util.*;
import java.io.*;
import java.time.*;

public class HuffmanCoding {

    // alphabet size of extended ASCII
    private static final int R = 256;

    private Node trie;

    private int length;

    private ArrayList<Boolean> compressed;

    // Huffman trie node
    private static class Node implements Comparable<Node> {
        private final char ch;
        private final int freq;
        private final Node left, right;

        Node(char ch, int freq, Node left, Node right) {
            this.ch = ch;
            this.freq = freq;
            this.left = left;
            this.right = right;
        }

        // is the node a leaf node?
        private boolean isLeaf() {
            assert ((left == null) && (right == null)) || ((left != null) && (right != null));
            return (left == null) && (right == null);
        }

        // compare, based on frequency
        public int compareTo(Node that) {
            return this.freq - that.freq;
        }
    }

    private String readFromInputStream(InputStream inputStream) throws IOException {
        StringBuilder resultStringBuilder = new StringBuilder();
        try (BufferedReader br = new BufferedReader(new InputStreamReader(inputStream))) {
            String line;
            while ((line = br.readLine()) != null) {
                resultStringBuilder.append(line).append("\n");
            }
        }
        return resultStringBuilder.toString();
    }

    private String readFile(String name) {
        try {
            File file = new File(name);
            if (file.exists()) {
                FileInputStream fis = new FileInputStream(file);
                return readFromInputStream(fis);
            }
        } catch (IOException ioe) {
            System.err.println("Could not open " + name);
        }

        return "";
    }

    /**
     * Reads a sequence of 8-bit bytes from standard input; compresses them using
     * Huffman codes with an 8-bit alphabet; and writes the results to standard
     * output.
     */
    public String compress(String name) {
        // read the input
        String s = readFile(name);
        char[] input = s.toCharArray();

        // tabulate frequency counts
        int[] freq = new int[R];
        for (int i = 0; i < input.length; i++)
            freq[input[i]]++;

        // build Huffman trie
        Node root = buildTrie(freq);
        trie = root;

        // build code table
        String[] st = new String[R];
        buildCode(st, root, "");

        length = input.length;

        compressed = new ArrayList<>();

        // use Huffman code to encode input
        for (int i = 0; i < input.length; i++) {
            String code = st[input[i]];
            for (int j = 0; j < code.length(); j++) {
                if (code.charAt(j) == '0') {
                    compressed.add(false);
                } else if (code.charAt(j) == '1') {
                    compressed.add(true);
                } else
                    throw new IllegalStateException("Illegal state");
            }
        }

        System.out.println("Input length:" + length + " Compressed:" + compressed.size() / 8);
        return s;
    }

    // build the Huffman trie given frequencies
    private Node buildTrie(int[] freq) {

        // initialze priority queue with singleton trees
        PriorityQueue<Node> pq = new PriorityQueue<>();
        for (char c = 0; c < R; c++) {
            if (freq[c] > 0) {
                pq.add(new Node(c, freq[c], null, null));
            }
        }

        // merge two smallest trees
        while (pq.size() > 1) {
            Node left = pq.remove();
            Node right = pq.remove();
            Node parent = new Node('\0', left.freq + right.freq, left, right);
            pq.add(parent);
        }
        return pq.remove();
    }

    // make a lookup table from symbols and their encodings
    private void buildCode(String[] st, Node x, String s) {
        if (!x.isLeaf()) {
            buildCode(st, x.left, s + '0');
            buildCode(st, x.right, s + '1');
        } else {
            st[x.ch] = s;
        }
    }

    /**
     * Reads a sequence of bits that represents a Huffman-compressed message from
     * standard input; expands them; and writes the results to standard output.
     */
    public String expand() {

        // read in Huffman trie from input stream
        Node root = trie;

        Iterator<Boolean> iter = compressed.iterator();
        StringBuilder output = new StringBuilder();

        // decode using the Huffman trie
        for (int i = 0; i < length; i++) {
            Node x = root;
            while (!x.isLeaf()) {
                boolean bit = iter.next();// BinaryStdIn.readBoolean();
                if (bit)
                    x = x.right;
                else
                    x = x.left;
            }
            output.append(x.ch);
        }
        return output.toString();
    }

    /**
     * Sample client that calls {@code compress()} if the command-line argument is
     * "-" an {@code expand()} if it is "+".
     *
     * @param args the command-line arguments
     */
    public static void main(String[] args) {
        HuffmanCoding hc = new HuffmanCoding();
        Instant start = Instant.now();
        String input = hc.compress("bible.txt");
        String expanded = hc.expand();
        if (!input.equals(expanded))
            throw new IllegalArgumentException("Strings dont match");
        Instant end = Instant.now();
        System.out.println(Duration.between(start, end));
    }

}
