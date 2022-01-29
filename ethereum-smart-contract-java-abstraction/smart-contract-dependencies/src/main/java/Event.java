public interface Event {
    public default void Event(Address from, Bytes32 id, Uint256 value) {
        from = null;
        id = null;
        value = null;
    }
}
