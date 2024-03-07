public class UnmatchedParentheses {
    public static void main(String[] args) {
        System.out.println(new Foo().baz());
    }
}

class Foo {
	public int baz() {
		int result;
        result = (10 * (5 + 3); //@error - syntax (Expression with unmatched parentheses)
		return result;
	}
}
