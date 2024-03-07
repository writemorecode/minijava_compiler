public class MainClass {
    public static void main(String[] a){
		System.out.println(new C().F(5,5));
    }
}

class C {
	// Correct
    public int F(int a, boolean b){
		return 5;
    }
	// Incorrect
    public int G(int a, int b){
		return 5;
    }
	// Incorrect
	public int H(int a, int b, int c) {
		return a*b*c;
	}
}
