package ro.atlas.exception;

public class ClientNotFoundException extends RuntimeException {

	private static final long serialVersionUID = 1L;

	public ClientNotFoundException(String clientIdentity) {
        super(String.format("Client with identity %s nor found!", clientIdentity));
    }
}
