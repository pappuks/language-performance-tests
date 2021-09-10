import java.util.*;
import java.time.*;

public class StepsTraversal {
	
	public static void main(String[] args) {
		Instant start = Instant.now();
		System.out.println(jump(30));
		Instant end = Instant.now();
		System.out.println(Duration.between(start, end));
	}
	
	public static long jump(int remainingSteps) {
		if (remainingSteps > 0) {
			return jump(remainingSteps - 1) + jump(remainingSteps - 2) + jump(remainingSteps - 3);
		} else {
			return 1;
		}
	}
	
}
