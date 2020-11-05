package ro.atlas.commands;

public enum AtlasClientCommandState {
	/* Client command is being delivered to owner for approval */
	ATLAS_CMD_CLIENT_DELIVERING_TO_OWNER_FOR_APPROVAL(0),
	/* Client command is being delivered to gateway */
	ATLAS_CMD_CLIENT_DELIVERING_TO_GATEWAY(1),
	/* Client command is being delivered to client */
	ATLAS_CMD_CLIENT_DELIVERING_TO_CLIENT(2),
	/* Client command was executed by the client */
	ATLAS_CMD_CLIENT_EXECUTED_BY_CLIENT(3),
	/* Client command was rejected by owner */
	ATLAS_CMD_CLIENT_REJECTED_BY_OWNER(4);
	
	/* Client command state */
	private int state;
	
	AtlasClientCommandState(int state) {
		this.state = state;
	}

	public int getState() {
		return state;
	}

	public void setState(int cmdState) {
		this.state = cmdState;
	}
	
}
