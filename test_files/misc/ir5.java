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
        array[index] = array[index + 1];
        return 0;
    }
}
