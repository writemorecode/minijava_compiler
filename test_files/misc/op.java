public class MainClass {
    public static void main(String[] a){
	System.out.println(0);
    }
}

class C {
    public int F(){
        return 1+false;
    }
    public int G(){
		return 2 - true;
    }
    public int H(){
        return 4 * false;
    }
    public int I(){
        return 5 / true;
    }
    public int J(){
        return true - false;
    }
    public boolean K(){
        return 1 < 2;
    }
    public boolean L(){
        return 1 < false;
    }

}
