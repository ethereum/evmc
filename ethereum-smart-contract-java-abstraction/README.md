## Java Abstraction work for Ethereum Smart-Contracts.

### *Goal: To allow the building of smart contracts in Java on Ethereum (with Rust, Python, and Go as the next focus).*

#### Note: *This project started as a focused version of the [New ERC Token](https://github.com/jeyakatsa/New-ERC-Token) with an overall goal of bringing more developers into the Ethereum ecosystem.*

## *Step-By-Step Guide for build*

*Brief Description:* This process is ever so evolving as need be but should provide a straight forward methodical approach into building it (open to more efficient methods).

### Step 1: Download/Install Necessities

| Language Needed   | Links                   |
| ------------------|:----------------------- |
| Java 11           | https://www.oracle.com/java/technologies/javase/jdk11-archive-downloads.html |

| Build/Test Tool Needed   | Links                   |
| -------------------------|:----------------------- |
| Gradle             | https://gradle.org/install/ |

| Coding Tool Needed   | Links                   |
| -----------------------|:----------------------- |
| IntelliJ         | https://www.jetbrains.com/idea/ |

### Step 2: Fork this repo & clone.

Java Abstraction repo: https://github.com/jeyakatsa/New-ERC-Token-Java-Abstraction

```shell script
git clone https://github.com/ConsenSys/teku.git
```

### Step 3: Build

Before building, run `./gradlew spotlessApply`. Then run,`./gradlew`. Then run, `./gradlew distTar installDist`.



