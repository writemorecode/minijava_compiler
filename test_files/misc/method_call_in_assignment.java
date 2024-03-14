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
        int x;
        x = this.Add(n, 10);
        return x;
    }
}
