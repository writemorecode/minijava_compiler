public class Main {
    public static void main(String[] a){
        System.out.println(new C().Func());
    }
}

class C {
    public int Func(){
        boolean x;
        x = false;
        boolean y;
        y = !x;
        return 0;
    }
}
