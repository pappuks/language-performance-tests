using System;
using System.Diagnostics;

namespace dotnet
{
    class Program
    {
        static void Main(string[] args)
        {
            Stopwatch watch = new Stopwatch();
            watch.Start();
            Console.WriteLine(jump(30));
            watch.Stop();
	    Console.WriteLine(watch.Elapsed);
        }
        
        public static long jump(int remainingSteps) {
		if (remainingSteps > 0) {
			return jump(remainingSteps - 1) + jump(remainingSteps - 2) + jump(remainingSteps - 3);
		} else {
			return 1;
		}
	}
    }
}
