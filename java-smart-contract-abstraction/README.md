## Java Abstraction for Ethereum Smart-Contracts.

### *Goal: To onboard Java Developers into the Ethereum Ecosystem by allowing the building of smart contracts in Java.*

### Abstract:

Currently, there are ***200,000 Solidity/Ethereum Developers (Worldwide*** *(source: [TrustNodes](https://www.trustnodes.com/2018/07/22/ethereums-ecosystem-estimated-200000-developers-truffle-seeing-80000-downloads-month))* ***)*** and ***7.1million Java Developers (Worldwide)****(source: [Daxx](https://www.daxx.com/blog/development-trends/number-software-developers-world#:~:text=According%20to%20SlashData%2C%20the%20number,%2C6%20million%20(source)))* respectfully.

What if all those Java developers (currently in the millions), could be onboarded into the Ethereum Ecosystem?

![](https://imgur.com/jjooSpr.jpg)

----------------------------------------------------

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



