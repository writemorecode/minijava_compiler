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

        int c;
        int d;
        c = 3;
        d = 4;

        int e;
        e = 5;

        int f;
        f = (a+b)*(d-c)/e;
        return e;
    }
}
