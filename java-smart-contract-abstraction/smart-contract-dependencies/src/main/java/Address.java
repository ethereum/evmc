import com.sun.jdi.ByteValue;
import com.sun.jdi.Type;
import com.sun.jdi.VirtualMachine;

public interface Address extends ByteValue {



    @Override
    public default byte value() {
        return 0;
    }

    @Override
    public default boolean booleanValue() {
        return false;
    }

    @Override
    public default byte byteValue() {
        return 0;
    }

    @Override
    public default char charValue() {
        return 0;
    }

    @Override
    public default short shortValue() {
        return 0;
    }

    @Override
    public default int intValue() {
        return 0;
    }

    @Override
    public default long longValue() {
        return 0;
    }

    @Override
    public default float floatValue() {
        return 0;
    }

    @Override
    public default double doubleValue() {
        return 0;
    }

    @Override
    public default Type type() {
        return null;
    }

    @Override
    public default VirtualMachine virtualMachine() {
        return null;
    }

    @Override
    public default int compareTo(ByteValue o) {
        return 0;
    }
}
