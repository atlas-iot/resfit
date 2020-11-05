package ro.atlas.commands;

public class AtlasGatewayCommand {
	/* Command type */
	private AtlasGatewayCommandType commandType;
	
	/* Command payload */
	private Object commandPayload;

	public AtlasGatewayCommandType getCommandType() {
		return commandType;
	}

	public void setCommandType(AtlasGatewayCommandType commandType) {
		this.commandType = commandType;
	}

	public Object getCommandPayload() {
		return commandPayload;
	}

	public void setCommandPayload(Object commandPayload) {
		this.commandPayload = commandPayload;
	}
}
