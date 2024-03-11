public class Main {
    public static void main(String[] a){
        System.out.println(0);
    }
}

class D {}

class C {
    public int Func(){
        D dObj;
        dObj = new D();
        return 0;
    }
}
