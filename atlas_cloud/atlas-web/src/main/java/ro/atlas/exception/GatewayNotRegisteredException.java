package ro.atlas.exception;

public class GatewayNotRegisteredException extends RuntimeException {

	private static final long serialVersionUID = 1L;

	public GatewayNotRegisteredException(String gatewayIdentity) {
        super(String.format("Gateway with identity %s is not online! Synchronization is not possible!", gatewayIdentity));
    }
}
