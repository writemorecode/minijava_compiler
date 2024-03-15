public class Main {
    public static void main(String[] a){
        System.out.println(new C().Func(50));
    }
}

class C {
    public int Add(int a, int b) {
        int c;
        c = a+b;
        return c;
    }
    public int Func(int n) {
        return this.Add(n, 10);
    }
}
