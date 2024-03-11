public class Main {
    public static void main(String[] a){
        System.out.println(new C().Func());
    }
}

class C {
    public int Func(){
        int index;
        index = 2;
        int[] array;
        int x;
        x = array[2 * index];
        return x;
    }
}
