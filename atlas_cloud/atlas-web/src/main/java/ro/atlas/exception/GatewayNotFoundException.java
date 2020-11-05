package ro.atlas.exception;

public class GatewayNotFoundException extends RuntimeException {

	private static final long serialVersionUID = 1L;

	public GatewayNotFoundException(String gatewayIdentity) {
        super(String.format("Gateway with identity %s not found!", gatewayIdentity));
    }
}
