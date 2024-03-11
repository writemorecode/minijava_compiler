public class Main {
    public static void main(String[] a){
        System.out.println(new C().Func());
    }
}

class C {
    public int Func(){
        boolean a;
        boolean b;
        a = true;
        b = false;

        boolean c;
        c = a && b;
        boolean d;
        d = a || b;

        return 0;
    }
}
