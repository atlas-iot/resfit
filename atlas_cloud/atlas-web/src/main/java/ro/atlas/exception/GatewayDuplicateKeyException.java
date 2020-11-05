package ro.atlas.exception;

public class GatewayDuplicateKeyException extends RuntimeException {

	private static final long serialVersionUID = 1L;

	/* The duplicate key */
    private String key;

    /* Duplicate key value */
    private String value;

    public GatewayDuplicateKeyException(String message) {
        super("Duplicate values for fields! This field must be unique!");

        String keyValue = message.substring(message.indexOf('{') + 1, message.indexOf('}'));
        key = keyValue.substring(1, keyValue.indexOf(':'));
        value = keyValue.substring(keyValue.indexOf(':') + 1);
    }

    public String getKey() {
        return key;
    }

    public String getValue() {
        return value;
    }
}
