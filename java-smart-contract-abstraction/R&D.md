# Java Smart Contract Abstraction Research & Development

### To allow the building of smart contracts in Java on Ethereum (with Rust, Python, and Go as the next focus).

#### Note: *This project started as a focused version of the [New ERC Token](https://github.com/jeyakatsa/New-ERC-Token) with an overall goal of bringing more developers into the Ethereum ecosystem.*

## Estimated Roadmap:

### Month-By-Month *(2022)*
January:
- Begin building Java Abstraction. *[Status: [In-Development](https://github.com/jeyakatsa/Ethereum-Smart-Contract-Java-Abstraction)]*
- Research & Develop Solidity-Keywords to Java-Dependency conversions *[Status: In-Development]*

February - December:
- ***Java*** Smart Contract Abstraction Build. *[Status: [In-Development](https://github.com/jeyakatsa/Ethereum-Smart-Contract-Java-Abstraction)]*
- ***Rust*** Smart Contract Abstraction Build. *[Status: Announced]*
- ***Python*** Smart Contract Abstraction Build. *[Status: Announced]*
- ***Go*** Smart Contract Abstraction Build. *[Status: Announced]*

### Year-By-Year
2022
- Finish building Java Abstraction *[Status: [In-Development](https://github.com/jeyakatsa/Ethereum-Smart-Contract-Java-Abstraction)]*
- Open blog to reflect on discoveries and to also offer discussions. *[Status: Announced]*
- Open aggregate Repo to arbitrate all language abstractions. *[Status: Announced]*
- Open aggregate EIP. *[Status: Announced]*

## Solidity-Keywords to Java-Dependency Conversion Problem Solving Process

*Problems & Solutions ordered top-down from recent-to-oldest*

### Problem:
This Solidity Keyword needs to be converted into a Java Dependency:
```solidity 
msg
``` 

#### Hypothesis:
1. Refactor Dependency.

##### Findings
The *"message"* object is a global variable that holds data which will be important for access to the blockchain.

```solidity
#msg.data (bytes)
#msg.sender (address)
#msg.sig (bytes4)
#msg.value (uint)
```
`msg.sender` is most commonly used. It is the address of the original sender .

`msg.sig` will return a hash of the current function signature. 

`msg.value` this usually holds Ether ( Wei (ether / 1e18) ) that is sent along with the message.

So essentially,

```solidity
function boom( address _addr) return (bytes4 _sig) {
  return msg.sig 
}
```

If you call the function `boom(addr)` it will return a hash that would look like `33DB172D` which would represent the hash of `boom(address)`. `msg.data` will return a payload in bytes. It holds the calldata as it is referred to in EVM terminology or the parameters that are passed to the function. If you pass in complex data structures into a function, the resulting msg.data will be structured in a complex way. It organizes the incoming params in bytes and delimits them with a 32 byte integer.

##### Test Case/s:
```java
//TBD (To Be Determined)
```

### Solution:
```java
//TBD (To Be Determined)
```
-----------------------------------------------------------------------

### Problem:
This Solidity Keyword needs to be converted into a Java Dependency:
```solidity 
Event
``` 

#### Hypothesis:
1. Refactor Dependency.

##### Findings
```solidity
contract Test {
   event Deposit(address indexed _from, bytes32 indexed _id, uint _value);
   function deposit(bytes32 _id) public payable {      
      emit Deposit(msg.sender, _id, msg.value);
   }
}
```
Event essentially pulls `address`, `bytes32` and `uint` as dependencies in java, thus, need to create or import `bytes32` as a dependency ***expanded in below problem***
##### Test Case/s:
```java
public default void Event(Address from, Bytes32 id, Uint256 value) {}
```

### Solution:
Possibly sufficient:
```java
public default void Event(Address from, Bytes32 id, Uint256 value) {
  from = null;
  id = null;
  value = null;
}
```
-----------------------------------------------------------------------

### Problem:
This Solidity Keyword needs to be converted into a Java Dependency:
```solidity 
bytes32
``` 

#### Hypothesis:
1. Refactor Dependency.

##### Findings
Ethereum uses the two endianness format depending on the variable type, as follows:
- Big endian format : strings and bytes
- Little endian format : other types (numbers, addresses, etc…).
As an example, this is how we would store the string “abcd” in one full word (32 bytes):
```0x6162636400000000000000000000000000000000000000000000000000000000```
This is how the number 0x61626364 would be stored:
```0x0000000000000000000000000000000000000000000000000000000061626364```
##### Test Case/s
```java
byte[] bytes32 = new byte[32];
```

### Solution:
Creating a simple `byte[]` array,
```java
public Bytes32() {
  byte[] bytes32 = new byte[32];
}
```

-----------------------------------------------------------------------


### Problem:
This Solidity Keyword needs to be converted into a Java Dependency:
```solidity 
Uint256
``` 

#### Hypothesis:
1. Refactor Dependency.

##### Findings:
- The EVM works natively in 256-bit words. Using smaller ones usually consumes more gas because there's extra work to do: the unneeded bits need to be masked away.
- ***Java AES 256 Encryption***, seems plausible.
Decrypting information:
```java
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.security.spec.KeySpec;
import java.util.Base64;
 
public class AES256 {
  private static final String SECRET_KEY = "my_super_secret_key";
  private static final String SALT = "ssshhhhhhhhhhh!!!!";
 
  public static String decrypt(String strToDecrypt) {
    try {
      byte[] iv = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      IvParameterSpec ivspec = new IvParameterSpec(iv);
 
      SecretKeyFactory factory = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256");
      KeySpec spec = new PBEKeySpec(SECRET_KEY.toCharArray(), SALT.getBytes(), 65536, 256);
      SecretKey tmp = factory.generateSecret(spec);
      SecretKeySpec secretKey = new SecretKeySpec(tmp.getEncoded(), "AES");
 
      Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
      cipher.init(Cipher.DECRYPT_MODE, secretKey, ivspec);
      return new String(cipher.doFinal(Base64.getDecoder().decode(strToDecrypt)));
    } catch (Exception e) {
      System.out.println("Error while decrypting: " + e.toString());
    }
    return null;
  }
}
```
- Solidity cannot Encrypt information, only Decrypt information, thus, the Uint256 Dependency must Decrypt.

##### Test Case/s:
```java
static byte[] ivBytes = new byte[0];
static int iterations = 65536;
static int keySize = 256;

public static String decrypt(byte[] uint) throws Exception {

    char[] placeholderText = new char[0];

    SecretKeyFactory skf = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA1");
    PBEKeySpec spec = new PBEKeySpec(placeholderText, uint, iterations, keySize);
    SecretKey secretkey = skf.generateSecret(spec);
    SecretKeySpec secretSpec = new SecretKeySpec(secretkey.getEncoded(), "AES");

    Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
    cipher.init(Cipher.DECRYPT_MODE, secretSpec, new IvParameterSpec(ivBytes));

    byte[] decryptedTextBytes = null;

    try {
        decryptedTextBytes = cipher.doFinal();
    }   catch (IllegalBlockSizeException e) {
        e.printStackTrace();
    }   catch (BadPaddingException e) {
        e.printStackTrace();
    }

    return decryptedTextBytes.toString();

}
```
Possibly sufficient.
```java
public Uint256(){}
```
Insufficient due to Interface failing to embrace Constructors.
```java
public abstract void Uint256(decrypt);
```
Insufficient.
```java
public abstract <decrypt> void Uint256(decrypt);
```
Insufficient, indentifier at end of function needed.
```java
 public default void Uint256(String decrypt) {};
```
Sufficient if `decrypt` is instantiated.
##### Findings
Invoking:
```java
public class ChildClass implements A, C {
    @Override    
    public void foo() {
       //you could completely override the default implementations
       doSomethingElse();
       //or manage conflicts between the same method foo() in both A and C
       A.super.foo();
    }
    public void bah() {
       A.super.foo(); //original foo() from A accessed
       C.super.foo(); //original foo() from C accessed
    }
}
```
Could provide solution...
##### Test Case/s:
```java
public default void Uint256() {decrypt();}
```
Insufficient.

### Solution:
```java
public default void Uint256() throws Exception {decrypt();}

public static void decrypt() throws Exception {
    //Vast Code expanded within [Uint256 Interface](https://github.com/jeyakatsa/ethereum-smart-contract-java-abstraction/blob/main/smart-contract-dependencies/src/main/java/Uint256.java)
}
```

-----------------------------------------------------------------------

### Problem:
This Solidity Keyword needs to be converted into a Java Dependency:
```solidity 
Address
``` 

#### Hypothesis:
1. Refactor Dependency.

##### Findings:
- In Solidity, address type comes with two flavors, `Address` and `Address Payable`. Both address and address payable store the ***20-byte*** values.
- Adding a byte type reference to Address class within Java Dependencies seems plausible.
```java
byte[] byteArray;
byte byteArray[];
```
- `Address` needs to be either a `public class Address<>{}` or a `public interface Address<>{}`.
##### Test Case/s:
```java
public class Address implements ByteValue {
    //implementing said values.
}
```

### Solution:
```java
public class Address implements ByteValue {
    @Override
    public byte value() {return 0;}
    @Override
    public boolean booleanValue() {return false;}
    @Override
    public byte byteValue() {return 0;}
    @Override
    public char charValue() {return 0;}
    @Override
    public short shortValue() {return 0;}
    @Override
    public int intValue() {return 0;}
    @Override
    public long longValue() {return 0;}
    @Override
    public float floatValue() {return 0;}
    @Override
    public double doubleValue() {return 0;}
    @Override
    public Type type() {return null;}
    @Override
    public VirtualMachine virtualMachine() {return null;}
    @Override
    public int compareTo(ByteValue o) {return 0;}
}
```

-----------------------------------------------------------------------

### Problem:
This function needs to be able to accept 2 parameters:
```java
private ArrayList balances = new List<Address, Uint256>();
```
***Note: This problem has been forked to solve the `Address` problem above***

#### Hypothesis:
1. Refactor Function.

##### Findings:
- `ArrayList<String> cars = new ArrayList<String>();` a possible reference.
##### Test Case/s:
```java
private ArrayList<Object[]> balances = new ArrayList<>();
balances.add(new Object[]{Address, Uint256});
```
Insufficient.
```java
private Iterable<Object[]> balances = new Iterable<Object[]>() {
    @Override
    public Iterator<Object[]> iterator() {return null;}
};
balances.get(new Object[]{Address, Uint256});
```
Insufficient.
##### Findings:
- https://docs.oracle.com/javase/8/docs/api/java/util/package-summary.html provides a lists of all necessary utilities.
##### Test Case/s:
```java
private Map<Address, Uint256> balances = new Map<Address, Uint256>();
```
Solved the double parameter issue, but within Map, needs a Key and Value.
```java
private PrimitiveIterator<Address, Uint256> balances = new PrimitiveIterator<Address, Uint256>() {
    @Override
    public void forEachRemaining(Uint256 action) {}
    @Override
    public boolean hasNext() {return false;}
    @Override
    public Address next() {return null;}
};
```
Insufficient since types within `Map` must be wrapper types for primitive types, such as Integer for the primitive int type.
```java
private ServiceLoader<Address> balances = new ServiceLoader.Provider<Address>() {
    @Override
    public Class<? extends Address> type() {return null;}
    @Override
    public Address get() {return null;}
};
```
Insufficient since Address class cannot be accessed...
```java
private Collection<Address> balances = new Collection<Address>() {
    @Override
    public int size() {return 0;}
    @Override
    public boolean isEmpty() {return false;}
    @Override
    public boolean contains(Object o) {return false;}
    @Override
    public Iterator<Address> iterator() {return null;}
    @Override
    public Object[] toArray() {return new Object[0];}
    @Override
    public <T> T[] toArray(T[] a) {return null;}
    @Override
    public boolean add(Address address) {return false;}
    @Override
    public boolean remove(Object o) {return false;}
    @Override
    public boolean containsAll(Collection<?> c) {return false;}
    @Override
    public boolean addAll(Collection<? extends Address> c) {return false;}
    @Override
    public boolean removeAll(Collection<?> c) {return false;}
    @Override
    public boolean retainAll(Collection<?> c) {return false;}
    @Override
    public void clear() {}
};
```
Insufficient since Address class cannot be accessed...
##### Findings
```java
public class MyInterfaceImpl implements MyInterface, MyOtherInterface {
    public void sayHello() {System.out.println("Hello");}
    public void sayGoodbye() {System.out.println("Goodbye");}
}
```
Instead of implementing a Map interface, possibly create a separate class/interface and import that.
### Solution:
```java
public interface Balances extends Address, Uint256 {
}
```
Creating an interferface while extending `Address` & `Uint256` then importing interface into Smart-Contract:
```java
public Balances balances;
```

-----------------------------------------------------------------------

## Function Problem Solving Process

*Problems & Solutions ordered top-down from recent-to-oldest*

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
 revert InsufficientBalance({
    requested: amount,
    available: balances[msg.sender]
});
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
REVERT will undo all state changes, but it will be handled differently than an “invalid opcode” in two ways:
*1. It will allow you to return a value.*
*2. It will refund any remaining gas to the caller.*
1. It will allow you to return a value
Most smart contract developers are quite familiar with the notoriously unhelpful invalid opcode error. Fortunately, we’ll soon be able to return an error message, or a number corresponding to an error type. That, will look something like this: `revert(‘Something bad happened’);`or `require(condition, ‘Something bad happened’);`
2. Refund the remaining gas to the caller
Currently, when your contract throws, it uses up any remaining gas. This can result in a very generous donation to miners, and often ends up costing users a lot of money.
- Essentially, `revert` is like an error...
##### Test Case/s:
Quite possibly:
```java
InsufficientBalance(requested = amount, available = balances.get(msg.sender));
```
could work...
##### Findings:
Reminder that `:` operand is essentially "If Condition is true? Then value X : Otherwise value Y"

### Solution:
```java
InsufficientBalance(amount = requested, balances.get(msg.sender) = available);
```
*Will refactor if necessary after test against dependency importing is completed*

--------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
error InsufficientBalance(uint requested, uint available);
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- `error` within Solidity is essentially a 'dependency' within Java
##### Test Case/s:
Function:
```java
private Uint256 requested;
private Uint256 available;
public void InsufficientBalance() {
    if (requested != available){
        throw new IllegalCallerException("Insufficient Balance");
    }
}
```
is sufficient.

### Solution:
```java
private Uint256 requested;
private Uint256 available;
public void InsufficientBalance() {
    if (requested != available){
        throw new IllegalCallerException("Insufficient Balance");
    }
}
```
*Will refactor if necessary after test against dependency importing is completed*

--------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
balances[receiver] += amount;
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- `balances[receiver] += amount;` insufficient for Java as it receives the `[receiver]` function as an Array when it is currated as a Map within the original `balances` function.
- `Mapping` is a reference type as array.
##### Test Case/s:
- Implementing `private Arrays balances = new List<Address, Uint256>();` as reference insufficient.
- Implementing `private ArrayType balances = new List<Address, Uint256>();` as reference insufficient.
- Implementing `private ArrayType balances = new ArrayDeque<Address, Uint256>();` as reference insufficient.
- Implementing `balances.get(receiver) += amount;` and `private ArrayList balances = new List<Address, Uint256>();` as reference sufficient.

### Solution:
```java
private ArrayList balances = new List<Address, Uint256>();
// balances function below reflects function within overall "public" function
balances.get(receiver) += amount;
```
*Will refactor if necessary after test against dependency importing is completed*

--------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
require(msg.sender == minter);
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- `msg.sender` contains the address that has originated the call of a smart contract as spoken within this medium article (https://medium.com/coinmonks/solidity-who-the-heck-is-msg-sender-de68d3e98454)
- Solidity `require` is a convenience function that checks invariants, conditions, and throws exceptions.
- The `require` Solidity function guarantees validity of conditions that cannot be detected before execution. It checks inputs, contract state variables and return values from calls to external contracts.
- In the following cases, Solidity triggers a require-type of exception: 
- - When you call require with arguments that result in false. 
- - When a function called through a message does not end properly.
- - When you create a contract with new keyword and the process does not end properly.
- - When you target a codeless contract with an external function.
- - When your contract gets Ether through a public getter function.
- - When `.transfer()` ends in failure.
##### Test Case/s:
- `require` needs to be a possible dependency in Java.

### Solution:
```java
Require(msg.sender == minter);
```
*Will refactor if necessary after test against dependency importing is completed*

--------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
msg.sender;
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- The `msg.sender` global variable — likely the most commonly used special variable — is always the address where a current function call came from. For instance, if a function call came from a user or smart contract with the address `0xdfad6918408496be36eaf400ed886d93d8a6c180` then `msg.sender` equals `0xdfad6918408496be36eaf400ed886d93d8a6c180`.
- `msg.sender` is most likely a hex, or a `Uint256`

### Solution:
```java
// msg almost always returns a Uint function (Uint256 in the case of Java)
public Subcurrency(){
    minter = msg.sender;
}
```
*Will refactor if necessary after test against dependency importing is completed*

-----------------------------------------------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
event Sent(address from, address to, uint amount);
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- A basic syntax function in Java is as follows:
```java
 private Event sent(Address from, Address to, Uint256 ammount) {
     return null;
 }
```
needs to *not* return null.
##### Test Case/s:
- Implementing `return from;` insufficient.
- Implementing
```java
    Address from;
    Address to;
    Uint256 amount;
```
and returning each within function insufficient.
- Implementing `Address from = null;` and returning `from` insufficient.
##### Findings:
- Is `event` within Solidity an object? https://www.tutorialspoint.com/solidity/solidity_events.htm
- "Event is an inheritable member of a contract. An event is emitted, it stores the arguments passed in transaction logs. These logs are stored on blockchain and are accessible using address of the contract till the contract is present on the blockchain. An event generated is not accessible from within contracts, not even the one which have created and emitted them."
- `event` within Solidity is not an object but acts rather like a "Dependency" in Java

### Solution:
Calling dependencies within sent function and returning function as is ie:
```java
private Event sent() {
    Address from;
    Address to;
    Uint256 amount;
    return sent;
}
```
*Will refactor if necessary after test against dependency importing is completed*

-----------------------------------------------------------------------

### Problem:
This function in Solidity needs to be converted into Java:
```solidity 
mapping (address => uint) public balances;
``` 

#### Hypothesis:
1. Refactor function.

##### Findings:
- A basic syntax function for Solidity is as follows:
```solidity
function function-name(parameter-list) scope returns() {
   //statements
}
```
Question is, how do we implement the `(address => uint)` in Java?
- The function `mapping (address => uint) public balances;` in Solidity is the similar to `var mydictionary = new Dictionary(key,value);` in Java with the `=>` simply indicating an arrow.

##### Test Case/s:
- Function in Java as possibility.
```java
public Map balances = new Map<Adress, Uint256>();
```

### Solution:
```java
public Map balances = new Map<Adress, Uint256>();
```
*Will refactor if necessary after test against dependency importing is completed*

-----------------------------------------------------------------------

## General Notes/References:
### [Intro-To-Smart-Contracts-in-Solidity](https://docs.soliditylang.org/en/v0.8.10/introduction-to-smart-contracts.html)
### [Layer-2](https://github.com/ethereum/ethereum-org-website/blob/dev/src/content/developers/docs/scaling/layer-2-rollups/index.md)

---------------------------------------------------

### Abstraction:

#### *While building the [Light Client Infrastructure for Teku](https://github.com/jeyakatsa/teku/tree/master/light-client), these are the said discoveries:*

- Clients serve as arbiters or "bridges" to the Ethereum main chain.
- Such clients offer differing languages that connect with the transactions offered on Ethereum, thus concluding if said clients can offer a base layer capable of inferring transactions from Ethereum's base layer, can smart contracts in said languages also offer the same results?
