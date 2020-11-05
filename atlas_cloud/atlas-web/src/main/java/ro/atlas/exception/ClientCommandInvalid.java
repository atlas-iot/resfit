package ro.atlas.exception;

public class ClientCommandInvalid extends RuntimeException {

	private static final long serialVersionUID = 1L;
	
	public ClientCommandInvalid(String command) {
        super(String.format("Command %s is invalid!", command));
    }
}
