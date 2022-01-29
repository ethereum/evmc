public class SimpleStorage {

    //Uint256 dependency to be imported
        private Uint256 storedData;

        public void setStoredData (Uint256 storedData) {
            this.storedData = storedData;
        }

        public Uint256 getStoredData () {
            return storedData;
        }

}
