# YAPL

Yet Another Progamming Language

### Build Dependencies

* [LLVM 11.0.0 or Higher](https://github.com/llvm/llvm-project)

## Example Code
```
func add(int x, int y) -> int {
    return x + y;
}

func doStuff() -> void {
    for(int i in 0 ... 4) {
        int a = i;
    }

    int x[3] = [0, 1, 2];

    x = [2, 3, 4];
}

struct exampleStruct {
    int a;
    double b;
    func addA(int b) -> int {
        a = a + b;
        return a + b;
    }
}

func main() -> int {
    exampleStruct example(1, 2.5);

    int test = example.addA(12);

    example = {3, 1.5};

    example.a = 8;

    int b = example.a;
    
    return 0;
}
```
