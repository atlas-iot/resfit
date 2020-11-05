package ro.atlas.commands;

public enum AtlasClientCommandType {
	/* Commands sent from cloud to client (via gateway) */
	ATLAS_CMD_CLIENT_DEVICE_RESTART("ATLAS_CMD_CLIENT_DEVICE_RESTART"),
	ATLAS_CMD_CLIENT_DEVICE_SHUTDOWN("ATLAS_CMD_CLIENT_DEVICE_SHUTDOWN");

	/* Command type */
	private String commandType;

	AtlasClientCommandType(String commandType) {
		this.commandType = commandType;
	}

	public String getCommandType() {
		return commandType;
	}
}
