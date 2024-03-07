public class InvalidReturn {
    public static void main(String[] args) {
        System.out.println(new MyClass().xyFunc());
    }
}

class classtest {
    int a;
}

class MyClass {
    int x;
    boolean y;
    int[] z;
    MyClass xyz;

    public int xyFunc() {// @error - semantic (invalid return type)
        return y;
    }

    public int xzFunc() {// @error - semantic (invalid return type)
        return z;
    }

    public boolean yxFunc() {// @error - semantic ('yxFunc' and its return are of different types)
        return x;
    }

    public boolean yzFunc() { // @error - semantic (invalid return type)
        return this.zxFunc();
    }

    public int[] zxFunc() { // @error - semantic (invalid return type)
        return x;
    }

    public int[] zyFunc() { // @error - semantic (invalid return type)
        return this.yxFunc();
    }

    public int swFunc() { // @error - semantic (invalid return type)
        return z[this.yzFunc()];
    }

}
