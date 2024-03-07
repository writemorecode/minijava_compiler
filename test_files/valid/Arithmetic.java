public class Arithmetic{
    public static void main(String[] a){
	System.out.println(new Foo().Bar());
    }
}

class Foo {
    public int Bar(){
        int a;
        a = 5;
        int b;
        b = 1;
        int c;
        if (a>b) {
            c = 0;
        } else {
            c = 1;
        }
        return c;
    }
}
