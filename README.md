# Json parser

This is a simple json parser, that can parse a json string and evaluate basic expressions, using values from the json object.

## Usage

To build the CLI tool, run the following command:

```bash
make
```

This creates a binary called `parser` in the current directory.

Example usage:

```bash
./parser example.json "a.b"
> [1, 2, {"c": "test"}, [11, 12]]
```

## Running tests

You can build and run the unit test binary with the following command:

```bash
make test && ./parser_test
```

To run the benchmark tests, you can use the script `test/benchmark.sh`, which runs the tests listed in `test/bench` and measures elapsed time.
