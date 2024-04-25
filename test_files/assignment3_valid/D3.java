public class D3 {
  public static void main(String[] a) {
    // Replaced argument 10000 with 100 to prevent stack overflow
    System.out.println(new Sum().calcSum(100));
  }
}

class Sum {
  public int calcSum(int num) {
    int sum;
    if (num < 2)
      sum = 1;
    else
      sum = num + this.calcSum(num - 1);
    return sum;
  }
}
