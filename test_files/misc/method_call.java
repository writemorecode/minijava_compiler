public class Main {
    public static void main(String[] a){
        System.out.println(new C().Add(50,30));
    }
}

class C {
    public int Add(int a, int b) {
        int c;
        c = a+b;
        return c;
    }
}
