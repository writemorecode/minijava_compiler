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

        int[] intArr;
        intArr = new int[4];
        intArr[0] = 1;
        int x;
        x = intArr[0];

        return x;
    }
}
