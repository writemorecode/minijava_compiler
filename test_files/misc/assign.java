public class MainClass {
    public static void main(String[] a){
        System.out.println(new C().F());
    }
}

class C {
    public int F(){
        boolean a;
		a = 5+false;
        return 1;
    }
    public int G(){
		int x;
		x = 7;
		boolean y;
		y = false;
        boolean a;
		a = x/y;
        return 1;
    }
    public int H() { return 3 * true; }
}
