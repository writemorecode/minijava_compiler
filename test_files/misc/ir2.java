public class Main {
    public static void main(String[] a){
        System.out.println(new C().Func());
    }
}

class C {
    public int Func(){
        int a;
        int b;
        a = 1;
        b = 2;

        boolean x;
        boolean y;

        x = a < b;
        y = a > b;

        return 0;
    }
}
