## Java Bindings

This bindings have been tested with `openjdk64-11.0.1` on OSX and debian:latest.
For examples of how to use these bindings take a look at the JUnit tests.

### Prerequisites 

You need to have [Gradle](https://www.gradle.org/installation) and [Java](https://www.oracle.com/technetwork/java/javase/downloads/index.html) installed.

> Note: Requires Gradle 5.x

### Build and test

from project root:

```bash
cd bindings/java && make clean build test
```

### Build and test (debug with debug printouts)

from project root: 

```bash
cd bindings/java && make clean debug test
```
