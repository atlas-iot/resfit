package ro.atlas.entity;

import org.springframework.data.mongodb.core.mapping.Document;

@Document
public class AtlasGatewayInfo {
	/* Holds the owner identifier */
	private String owner;

	public void updateInfo(AtlasGatewayInfo gatewayInfo) {
		if (gatewayInfo == null) {
			return;
		}
		
		if (gatewayInfo.getOwner() != null) {
			this.setOwner(gatewayInfo.getOwner());
		}
	}
	
	public String getOwner() {
		return owner;
	}

	public void setOwner(String owner) {
		this.owner = owner;
	}
}
