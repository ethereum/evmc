package org.ethereum.evmc;

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.KeyDeserializer;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.fasterxml.jackson.databind.deser.std.StdDeserializer;
import com.fasterxml.jackson.databind.module.SimpleModule;
import com.fasterxml.jackson.databind.ser.std.StdSerializer;
import java.io.IOException;
import java.time.Instant;
import org.apache.tuweni.bytes.Bytes;
import org.apache.tuweni.eth.Address;
import org.apache.tuweni.eth.Hash;
import org.apache.tuweni.units.bigints.UInt256;
import org.apache.tuweni.units.ethereum.Gas;
import org.apache.tuweni.units.ethereum.Wei;

public class EthJsonModule extends SimpleModule {

  static class HashSerializer extends StdSerializer<Hash> {

    HashSerializer() {
      super(Hash.class);
    }

    @Override
    public void serialize(Hash value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeString(value.toHexString());
    }
  }

  static class AddressSerializer extends StdSerializer<Address> {

    AddressSerializer() {
      super(Address.class);
    }

    @Override
    public void serialize(Address value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeString(value.toHexString());
    }
  }

  static class BytesSerializer extends StdSerializer<Bytes> {

    BytesSerializer() {
      super(Bytes.class);
    }

    @Override
    public void serialize(Bytes value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeString(value.toHexString());
    }
  }

  static class GasSerializer extends StdSerializer<Gas> {

    GasSerializer() {
      super(Gas.class);
    }

    @Override
    public void serialize(Gas value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeString(value.toBytes().toHexString());
    }
  }

  static class UInt256Serializer extends StdSerializer<UInt256> {

    UInt256Serializer() {
      super(UInt256.class);
    }

    @Override
    public void serialize(UInt256 value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeString(value.toHexString());
    }
  }

  static class InstantSerializer extends StdSerializer<Instant> {

    InstantSerializer() {
      super(Instant.class);
    }

    @Override
    public void serialize(Instant value, JsonGenerator gen, SerializerProvider provider)
        throws IOException {
      gen.writeNumber(value.toEpochMilli());
    }
  }

  static class AddressDeserializer extends StdDeserializer<Address> {

    AddressDeserializer() {
      super(Address.class);
    }

    @Override
    public Address deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
      return Address.fromHexString(p.getValueAsString());
    }
  }

  static class GasDeserializer extends StdDeserializer<Gas> {

    GasDeserializer() {
      super(Gas.class);
    }

    @Override
    public Gas deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
      return Gas.valueOf(UInt256.fromHexString(p.getValueAsString()));
    }
  }

  static class WeiDeserializer extends StdDeserializer<Wei> {

    WeiDeserializer() {
      super(Wei.class);
    }

    @Override
    public Wei deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
      return Wei.valueOf(UInt256.fromHexString(p.getValueAsString()));
    }
  }

  static class BytesDeserializer extends StdDeserializer<Bytes> {

    BytesDeserializer() {
      super(Bytes.class);
    }

    @Override
    public Bytes deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
      return Bytes.fromHexString(p.getValueAsString());
    }
  }

  static class UInt256Deserializer extends StdDeserializer<UInt256> {

    UInt256Deserializer() {
      super(UInt256.class);
    }

    @Override
    public UInt256 deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
      return UInt256.fromHexString(p.getValueAsString());
    }
  }

  static class AddressKeyDeserializer extends KeyDeserializer {

    @Override
    public Address deserializeKey(String key, DeserializationContext ctxt) throws IOException {
      return Address.fromHexString(key);
    }
  }

  static class BytesKeyDeserializer extends KeyDeserializer {

    @Override
    public Bytes deserializeKey(String key, DeserializationContext ctxt) throws IOException {
      return Bytes.fromHexString(key);
    }
  }

  public EthJsonModule() {
    addSerializer(Hash.class, new HashSerializer());
    addSerializer(Address.class, new AddressSerializer());
    addSerializer(Bytes.class, new BytesSerializer());
    addSerializer(Gas.class, new GasSerializer());
    addSerializer(UInt256.class, new UInt256Serializer());
    addSerializer(Instant.class, new InstantSerializer());
    addKeyDeserializer(Bytes.class, new BytesKeyDeserializer());
    addKeyDeserializer(Address.class, new AddressKeyDeserializer());
    addDeserializer(Address.class, new AddressDeserializer());
    addDeserializer(Gas.class, new GasDeserializer());
    addDeserializer(Wei.class, new WeiDeserializer());
    addDeserializer(UInt256.class, new UInt256Deserializer());
    addDeserializer(Bytes.class, new BytesDeserializer());
  }
}
